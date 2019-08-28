import base64
from concurrent.futures import ThreadPoolExecutor
import io
import json
import logging
import math
from pathlib import Path
import os
import tarfile
import tempfile
import time

import botocore

from jitq.aws_helpers import make_boto3_client, make_boto3_resource
from jitq.utils import get_project_path


class Driver:
    def __init__(self, stack_name):

        self.logger = logging.getLogger(__name__)
        self.enable_log_upload = True

        max_workers = int(os.environ.get('JITQ_LAMBDA_DRIVER_NUM_THREADS', 20))
        self.logger.info('Using %s threads to spawn lambdas.', max_workers)
        self.executor = ThreadPoolExecutor(max_workers=max_workers)

        # Retrieve metadata from stack
        cloudformation_client = make_boto3_client('cloudformation')
        res = cloudformation_client.describe_stacks(
            StackName=stack_name,
        )

        if len(res['Stacks']) != 1:
            raise RuntimeError('Could not find stack: "{}"'.format(stack_name))

        stack = res['Stacks'][0]
        stack_metadata = {o['OutputKey']: o['OutputValue']
                          for o in stack['Outputs']}

        # S3
        s3_bucket_name = stack_metadata['JitqLambdaRunnerBucket']
        self.s3_client = make_boto3_client('s3')
        self.s3_resource = make_boto3_resource('s3')
        self.s3_bucket = self.s3_resource.Bucket(s3_bucket_name)

        # Lambda
        self.lambda_client = make_boto3_client(
            'lambda',
            config=botocore.client.Config(
                read_timeout=70,
                retries={'max_attempts': 0},
            ),
        )
        self.lambda_function_name = stack_metadata['JitqLambdaRunnerFunction']

        # SQS
        self.sqs_resource = make_boto3_resource('sqs')
        self.result_queue_name = stack_metadata['JitqLambdaRunnerResultQueue']
        self.dlq_name = stack_metadata['JitqLambdaRunnerDeadLetterQueue']
        self.result_queue = self.sqs_resource.get_queue_by_name(
            QueueName=self.result_queue_name,
        )
        self.dlq = self.sqs_resource.get_queue_by_name(
            QueueName=self.dlq_name,
        )

    def __enter__(self):
        return self

    def __exit__(self, type_, value, traceback):
        self.executor.shutdown(wait=True)

    @staticmethod
    def package_plan(compiled_dag_str):
        compile_dag_bytes = compiled_dag_str.encode('utf-8')

        tar_file_path = os.path.join(get_project_path(), 'plan.tar.xz')

        with tarfile.open(tar_file_path, 'w:xz') as tar_file:
            compiled_dag_info = tarfile.TarInfo('dag.json')
            compiled_dag_info.size = len(compile_dag_bytes)
            tar_file.addfile(compiled_dag_info, io.BytesIO(compile_dag_bytes))

            compiled_dag = json.loads(compiled_dag_str)
            operators = compiled_dag['operators']
            assert len(operators) >= 2
            compiled_pipeline_ops = [o for o in operators
                                     if o['op'] == 'compiled_pipeline']
            assert len(compiled_pipeline_ops) == 1
            library_name = compiled_pipeline_ops[0]['library_name']
            library_path = os.path.join(get_project_path(),
                                        'backend', 'gen', library_name)
            tar_file.add(library_path, library_name)

            tar_file.close()

        return tar_file_path

    def start_workers(self, num_workers, query_id, inputs, tar_file_path):
        # pylint: disable=too-many-locals # XXX: fixme

        self.logger.info('Invoking lambdas for query %s...', query_id)

        assert len(inputs) == num_workers

        with open(tar_file_path, 'rb') as tar_file:
            plan = tar_file.read()

        def run_lambda(worker_range):
            worker_ids = list(range(worker_range[0], worker_range[1]))
            worker_id = worker_ids.pop(0)
            invocation_inputs = [inputs[i] for i in worker_ids]
            event = {
                'worker_id': worker_id,
                'num_workers': num_workers,
                'query_id': query_id,
                'log_config': {
                    'level': logging.INFO,
                    'enable_log_upload': self.enable_log_upload,
                },
                'plan': base64.b64encode(plan).decode('ascii'),
                'inputs': inputs[worker_id],
                'invocations': {
                    'worker_ids': worker_ids,
                    'inputs': invocation_inputs,
                },
            }

            self.logger.info('  Starting worker %d, who will start workers '
                             '[%d,%s).', worker_id, worker_range[0] + 1,
                             worker_range[1])
            res = self.lambda_client.invoke(
                FunctionName=self.lambda_function_name,
                Payload=json.dumps(event).encode('utf-8'),
                InvocationType='Event',
                LogType='Tail',
            )
            self.logger.info('  Done starting worker %d.', worker_id)
            return res

        worker_range_size = 1
        if num_workers > 500:
            worker_range_size = math.ceil(math.sqrt(num_workers))
        num_ranges = math.ceil(num_workers / worker_range_size)
        worker_ranges = [((i + 0) * num_workers // num_ranges,
                          (i + 1) * num_workers // num_ranges)
                         for i in range(num_ranges)]
        self.logger.info("Ranges: %s", worker_ranges)
        results = list(self.executor.map(run_lambda, worker_ranges))

        self.logger.info('Done invoking lambdas for query %s...', query_id)

        for res in results:
            if not 'Payload' in res:
                self.logger.debug('Lambda returned: %s', res)
                continue
            payload = res['Payload'].read()
            if not payload:
                self.logger.debug('Lambda returned empty payload.')
                continue
            payload = json.loads(payload.decode('utf-8'))
            if payload is not None and 'errorMessage' in payload:
                log_result = base64.b64decode(res.get('LogResult', '')) \
                    .decode('utf-8')
                raise RuntimeError(
                    'Error during lambda invokation: {}\nLast log output:\n{}'
                    .format(payload['errorMessage'], log_result))

    def fetch_message_batch(self, queue):
        messages = queue.receive_messages(
            MaxNumberOfMessages=10,
            VisibilityTimeout=1,
            WaitTimeSeconds=1,
        )

        if not messages:
            return []

        # Delete received messages from queue
        handles = [m.receipt_handle for m in messages]
        queue.delete_messages(
            Entries=[{'Id': str(t[0]), 'ReceiptHandle': t[1]}
                     for t in enumerate(handles)],
        )

        for message in messages:
            self.logger.debug('Received message: %s', message)

        return messages

    def fetch_results(self, query_id, num_workers):
        start_fetch_time = time.time()
        messages = {}
        num_discarded_messages = 0
        num_exceptions = 0

        while len(messages) < num_workers:
            if time.time() > start_fetch_time + 120:
                self.logger.error('  Timeout, giving up...', exc_info=True)
                break

            message_batches = self.executor.map(
                lambda i: self.fetch_message_batch(self.result_queue),
                range(10))

            for batch in message_batches:
                for message in batch:
                    body = json.loads(message.body)

                    if 'query_id' in body and \
                            body['query_id'] == query_id and \
                            'worker_id' in body and \
                            isinstance(body['worker_id'], int):
                        worker_id = body['worker_id']
                        if worker_id in messages:
                            self.logger.warning(
                                'Worker %d returned two messages.', worker_id)
                        messages[worker_id] = body
                    else:
                        # Wrong or no query_id
                        num_discarded_messages += 1
                        continue

                    if 'exception' in body:
                        num_exceptions += 1

        self.logger.info('Received %d messages, %d of them exceptions, '
                         'plus %d invalid ones.',
                         len(messages), num_exceptions, num_discarded_messages)

        return messages.values()

    def fetch_deadletters(self):
        self.logger.info('Checking for deadletters...')
        num_dlq_messages = 0
        while True:
            messages = self.fetch_message_batch(self.dlq)

            if not messages:
                break

            num_dlq_messages += len(messages)
        self.logger.info('Received %d deadletters.', num_dlq_messages)

    def download_log_files(self, s3_prefix, num_workers):
        self.logger.info('  Downloading log files from %s...',
                         self.s3_bucket.name)

        log_dir = os.path.join(get_project_path(), 'log')
        Path(log_dir).mkdir(parents=True, exist_ok=True)

        downloaded_files = set()

        def download_log_file(obj):
            key = obj['Key']
            filename = os.path.basename(key)
            filename = s3_prefix.replace('/', '-') + filename
            if filename in downloaded_files:
                return None
            self.s3_bucket.download_file(
                Key=key,
                Filename=os.path.join(log_dir, filename),
            )
            return filename

        def download_logs(page):
            if 'Contents' not in page:
                self.logger.warning('  No log files found.')
                return
            filenames = \
                list(self.executor.map(download_log_file, page['Contents']))
            filenames = [n for n in filenames if n is not None]
            downloaded_files.update(filenames)
            self.logger.info('  Downloaded %s log files.',
                             len(filenames))

        start_download_time = time.time()

        while len(downloaded_files) < num_workers:
            if time.time() > start_download_time + 120:
                self.logger.error('  Timeout, giving up...', exc_info=True)
                break

            paginator = self.s3_client.get_paginator('list_objects')
            logs_paginator = paginator.paginate(
                Bucket=self.s3_bucket.name,
                Prefix=s3_prefix + 'log/',
            )
            list(map(download_logs, logs_paginator))

    @staticmethod
    def extract_results(results):
        outputs = []
        result_dir = tempfile.mkdtemp(prefix='jitq-lambda-result-')
        for i, result in enumerate(results):
            if 'exception' in result:
                raise RuntimeError(
                    'Exception in worker {}:\n {}\n'
                    'You may find more details in log {}.'.format(
                        result['worker_id'], result['exception'],
                        result['log_stream_name']))

            result_values = json.loads(result['output'])
            result_file_content = result_values[0]['fields'][0]['value']
            result_file_path = os.path.join(result_dir,
                                            'result-{0}.parquet'.format(i))
            with open(result_file_path, 'wb') as result_file:
                result_file.write(base64.b64decode(result_file_content))
            result_values[0]['fields'][0]['value'] = result_file_path
            outputs.append(json.dumps(result_values))

        return outputs

    def run_query(self, compiled_dag_str, num_workers, inputs):
        global_start_time = time.time()
        query_id = int(global_start_time * 1000)

        s3_prefix = 'jitq/query-{}/'.format(query_id)

        # Package plan
        tar_file_path = self.package_plan(compiled_dag_str)

        # Start lambdas
        self.start_workers(num_workers, query_id, inputs, tar_file_path)

        # Fetch results
        results = self.fetch_results(query_id, num_workers)

        # Fetch deadletters and logs (asynchronously)
        self.executor.submit(self.fetch_deadletters)
        self.download_log_files(s3_prefix, num_workers)

        # Extract result values
        outputs = self.extract_results(results)
        assert len(outputs) == num_workers

        self.executor.shutdown()

        return outputs
