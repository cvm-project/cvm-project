import base64
from concurrent.futures import ThreadPoolExecutor
import json
import logging
import os
from pathlib import Path
import shutil
import subprocess
import tarfile
import tempfile
import time
import traceback

import boto3
import botocore

# pylint: disable=import-error  # pylint doesn't know how this script is
#                               # run (with this path in the Python path)
from utils import compute_config_from_endpoint


class Handler:
    # pylint: disable=too-many-instance-attributes  # XXX: fixme

    def __init__(self, event, context):
        # pylint: disable=too-many-locals  # XXX: fixme

        self.event = event
        self.context = context
        self.worker_id = 'unknown'
        self.num_workers = 'unknown'
        self.query_id = 'unknown'

        # Read result queue parameters early. If we don't have those, we can't
        # communicate back the result or errors
        sqs_endpoint = os.environ['AWS_SQS_ENDPOINT']
        self.sqs_config = compute_config_from_endpoint(sqs_endpoint)
        self.result_queue_name = \
            os.environ['JITQ_LAMBDA_RUNNER_RESULT_QUEUE_NAME']
        self.result_queue_url = \
            os.environ['JITQ_LAMBDA_RUNNER_RESULT_QUEUE_URL']

        try:
            self.temp_dir = tempfile.mkdtemp(prefix='jitq-query-')
            self.local_start_time = time.time()

            # Read remaining input parameters
            self.worker_id = event['worker_id']
            self.num_workers = event['num_workers']
            self.query_id = event['query_id']
            self.plan = event.get('plan', None)
            self.runner_env_vars = event.get('runner_env_vars', {})

            # Return immediately if we are late
            global_start_time = self.query_id / 1000
            startup_max_delay = 20
            startup_delay = time.time() - global_start_time
            if startup_delay > startup_max_delay:
                raise RuntimeError(
                    'I took {:.3g}s to start. That\'s too long; giving up.'
                    .format(startup_delay))

            max_workers = 32
            # pylint: disable=consider-using-with  # managed manually
            self.executor = ThreadPoolExecutor(max_workers=max_workers)

            # Load S3 config
            self.s3_bucket = os.environ['JITQ_LAMBDA_RUNNER_BUCKET_NAME']
            s3_endpoint = os.environ['AWS_S3_ENDPOINT']
            s3_config = compute_config_from_endpoint(s3_endpoint)

            # Load Lambda config
            self.lambda_function_name = \
                os.environ['JITQ_LAMBDA_FUNCTION_NAME'] or \
                os.environ['AWS_LAMBDA_FUNCTION_NAME']
            lambda_endpoint = os.environ['AWS_LAMBDA_ENDPOINT']
            lambda_config = compute_config_from_endpoint(lambda_endpoint)

            # Load logging config
            log_config = event.get('log_config', {})
            log_level = log_config.get('level', logging.WARNING)
            self.enable_log_upload = log_config.get('enable_log_upload', False)

            # Setup additional logging to file
            logging.getLogger().setLevel(log_level)
            if self.enable_log_upload:
                formatter = logging.Formatter(
                    '%(asctime)s %(levelname)s %(message)s')
                self.log_file_path = os.path.join(self.temp_dir, 'runner.log')

                handler = logging.FileHandler(self.log_file_path)
                handler.setFormatter(formatter)

                logging.getLogger().addHandler(handler)

            logging.debug('sqs_config: %s', self.sqs_config)
            logging.debug('s3_config: %s', s3_config)

            # Set up Lambda client
            self.lambda_client = boto3.client(
                'lambda', **lambda_config,
                config=botocore.client.Config(
                    read_timeout=70,
                    retries={'max_attempts': 0},
                ),
            )

            # Set up S3 client
            self.s3_client = boto3.client('s3', **s3_config)

            # Set up query
            self.s3_prefix = 'jitq/query-{}/'.format(self.query_id)
            self.input = self.event['inputs']

            logging.info('Worker %d started at %s (%s):\n  Input: %s'
                         '\n  Region: %s\n  Log name: %s',
                         self.worker_id, self.local_start_time,
                         time.ctime(self.local_start_time),
                         self.input, os.environ.get('AWS_REGION', ''),
                         os.environ.get('AWS_LAMBDA_LOG_STREAM_NAME', ''))

        except BaseException:
            self._handle_exception()

    def _invoke_worker(self, invocation_details):
        worker_id, inputs = invocation_details

        logging.info('Invoking worker %s.', worker_id)

        invocation_event = self.event.copy()
        invocation_event['worker_id'] = worker_id
        invocation_event['num_workers'] = self.num_workers
        invocation_event['inputs'] = inputs

        self.lambda_client.invoke(
            FunctionName=self.lambda_function_name,
            Payload=json.dumps(invocation_event).encode('utf-8'),
            InvocationType='Event',
        )

    def _handle_invocations(self):
        if 'invocations' in self.event:
            invocations = self.event.pop('invocations')
            invocations = zip(invocations['worker_ids'], invocations['inputs'])

            list(self.executor.map(self._invoke_worker, invocations))

    def _handle_exception(self):
        logging.error('Exception in worker %d:\n%s',
                      self.worker_id, traceback.format_exc())
        self._return_result({
            'exception': traceback.format_exc(),
            'event': self.event,
            'query_id': self.query_id,
            'context': str(self.context),
            'worker_id': self.worker_id,
            'log_stream_name': os.environ.get(
                'AWS_LAMBDA_LOG_STREAM_NAME', ''),
        })

        # Attempt to upload log
        self._finalize()

    def _return_result(self, result):
        try:
            sqs_client = boto3.client('sqs', **self.sqs_config)
            if not self.result_queue_url or \
                    self.result_queue_url == self.result_queue_name:
                res = sqs_client.get_queue_url(
                    QueueName=self.result_queue_name,
                )
                self.result_queue_url = res['QueueUrl']
            sqs_client.send_message(
                QueueUrl=self.result_queue_url,
                MessageBody=json.dumps(result),
            )
            logging.info('Worker %d done sending result message',
                         self.worker_id)
        except BaseException:
            logging.error('Exception in worker %d when trying to send '
                          'result message:\n%s',
                          self.worker_id, traceback.format_exc())

    def _finalize(self):
        self.executor.shutdown()

        if self.enable_log_upload:
            self.s3_client.upload_file(
                Bucket=self.s3_bucket,
                Key=self.s3_prefix + 'log/worker-{}.log'.format(
                    self.worker_id),
                Filename=self.log_file_path,
            )

            logging.info('Worker %d done uploading log', self.worker_id)

    def _extract_plan(self):
        temp_gen_dir = os.path.join(self.temp_dir, 'backend', 'gen')
        dag_file_path = os.path.join(temp_gen_dir, 'dag.json')
        tar_file_path = os.path.join(self.temp_dir, 'plan.tar.xz')

        # Write compressed plan data to file
        with open(tar_file_path, 'wb') as tar_file:
            tar_file.write(base64.b64decode(self.plan))

        # Extract dag.json and generated executables
        with tarfile.open(tar_file_path) as tar_file:
            tar_file.extractall(temp_gen_dir)

        logging.info('Worker %d has extracted the plan.', self.worker_id)

        return dag_file_path

    def _run_runner(self, dag_file_path):
        # Run plan with runner
        paths = [os.path.join(os.path.dirname(__file__), 'lib')]
        if 'JITQPATH' in os.environ:
            paths.insert(0, os.path.join(os.environ['JITQPATH'],
                                         'backend', 'build'))
        path = ':'.join(paths)
        runner = shutil.which('runner', path=path)

        if runner is None:
            raise RuntimeError('Could not find runner executable')

        logging.info('Worker %d is using the runner %s.',
                     self.worker_id, runner)

        env = os.environ.copy()
        env['JITQPATH'] = self.temp_dir
        env['JITQ_LAMBDA_WORKER_ID'] = str(self.worker_id)
        env['JITQ_LAMBDA_NUM_WORKERS'] = str(self.num_workers)
        env['JITQ_EXCHANGE_S3_BUCKET_NAME'] = self.s3_bucket
        env['JITQ_QUERY_ID'] = str(self.query_id)
        env['OMP_NUM_THREADS'] = '1'
        env['HOME'] = str(Path.home())
        env.update(self.runner_env_vars)

        # Compute remaining time
        timeout = 20
        try:
            timeout = self.context.get_remaining_time_in_millis() / 1000.0 - 1
        except BaseException:
            pass
        timeout = str(timeout) + 's'

        logging.info('Running runner with timeout of %s.', timeout)

        # Run
        try:
            result = subprocess \
                .run(['timeout', timeout, 'catchsegv',
                      runner, '--dag', dag_file_path],
                     env=env, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                     input=self.input.encode('utf-8'), check=True)
            output = result.stdout.decode('utf-8')
            if result.stderr:
                logging.warning('Runner produced output on stderr:\n%s',
                                result.stderr.decode('utf-8'))
        except subprocess.CalledProcessError as ex:
            output = ex.stdout.decode('utf-8')
            errors = ex.stderr.decode('utf-8')
            logging.error('Runner failed with the following output.\n'
                          '=== stdout: ===\n%s\n\n=== stderr: ===\n%s\n',
                          output, errors)
            raise RuntimeError(
                'Runner failed. '
                'Output on stdout:\n{}\nOutput on stderr:\n{}'.format(
                    '\n'.join(output.splitlines()[:100]),
                    '\n'.join(errors.splitlines()[:100]))) from ex

        logging.info('Worker %d has run the plan. Result: %s',
                     self.worker_id, output)

        # Read output file
        result_values = json.loads(output)
        result_file_path = result_values[0]['fields'][0]['value']
        with open(result_file_path, 'rb') as result_file:
            result_file_content = base64.b64encode(result_file.read())
        result_values[0]['fields'][0]['value'] = \
            result_file_content.decode('utf-8')
        output = json.dumps(result_values)

        logging.info('Worker %d has encoded the result: %s',
                     self.worker_id, output)

        return output

    def run(self):
        try:
            self._handle_invocations()

            # Extract DAG
            dag_file_path = self._extract_plan()

            # Run DAG in runner
            output = self._run_runner(dag_file_path)

            # Assemble and send result
            result = {
                'worker_id': self.worker_id,
                'query_id': self.query_id,
                'local_start_time': self.local_start_time,
                'output': output,
            }
            self._return_result(result)

            # Clean up
            self._finalize()

            return result

        except BaseException:
            self._handle_exception()
            return None  # keep pylint happy


def run(event, context):
    handler = Handler(event, context)
    return handler.run()
