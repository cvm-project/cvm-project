# pylint: disable=redefined-outer-name  # required by pytest

import datetime
import io
import logging
import os
import subprocess
import sys
import time
from unittest import mock

import boto3
import botocore
import botocore.stub
import pytest

from jitq.aws_helpers import make_boto3_client, make_boto3_resource
from jitq.jitq_context import JitqContext
from jitq.serverless import driver


def create_s3_bucket(bucket_name, prefix):
    logger = logging.getLogger(__name__)

    resource = make_boto3_resource('s3')
    bucket = resource.Bucket(bucket_name)

    logger.info('Creating S3 bucket "%s"', bucket.name)
    try:
        bucket.create(
            CreateBucketConfiguration={
                'LocationConstraint': boto3.session.Session().region_name,
            },
        )
    except botocore.exceptions.ClientError as ex:
        if ex.response['Error']['Code'] != 'BucketAlreadyOwnedByYou':
            raise ex
        logger.info('Bucket already existed')

    clean_s3_bucket(bucket_name, prefix)


def clean_s3_bucket(bucket_name, prefix):
    logger = logging.getLogger(__name__)

    resource = make_boto3_resource('s3')
    bucket = resource.Bucket(bucket_name)
    client = make_boto3_client('s3')

    # Clean up bucket from previous runs
    logger.info('Deleting files of previous runs...')

    def delete_objects(page):
        if 'Contents' not in page:
            return
        bucket.delete_objects(
            Delete={'Objects': [{'Key': o['Key']} for o in page['Contents']]},
        )
        logger.info('  Deleted %s files.', len(page['Contents']))
    paginator = client.get_paginator('list_objects')
    delete_paginator = paginator.paginate(
        Bucket=bucket.name,
        Prefix=prefix,
    )
    list(map(delete_objects, delete_paginator))


def create_sqs_queue(queue_name):
    logger = logging.getLogger(__name__)
    sqs_client = make_boto3_client('sqs')
    sqs_resource = make_boto3_resource('sqs')

    logger.info('Creating SQS "%s"', queue_name)
    sqs_client.create_queue(
        QueueName=queue_name,
    )

    queue = sqs_resource.get_queue_by_name(
        QueueName=queue_name,
    )

    logger.info('Draining existing messages from %s...', queue_name)

    num_messages = int(queue.attributes.get('ApproximateNumberOfMessages', 0))
    if num_messages > 300:
        logger.info('  Found %d messages in the queue. '
                    'Calling purge and sleeping...')
        queue.purge()
        time.sleep(num_messages / 1000 + 10)
        return

    while True:
        messages = queue.receive_messages(
            MaxNumberOfMessages=10,
            VisibilityTimeout=1,
            WaitTimeSeconds=1,
        )

        logger.info('  Drained %s messages.', len(messages))

        if not messages:
            break

        handles = [m.receipt_handle for m in messages]

        queue.delete_messages(
            Entries=[{'Id': str(t[0]), 'ReceiptHandle': t[1]}
                     for t in enumerate(handles)],
        )


TEST_RESULT_QUEUE_NAME = 'JitqLambdaRunnerResultQueue'
TEST_DLQ_NAME = 'JitqLambdaRunnerDeadLetterQueue'
TEST_BUCKET_NAME = 'jitqlambdarunnerbucket'
TEST_LAMBDA_FUNCTION_NAME = 'JitqLambdaRunnerFunction'

JITQ_MOCK_STACK = {
    'Stacks': [{
        'StackName': 'jitq-test-stack',
        'CreationTime': datetime.datetime.utcnow(),
        'StackStatus': 'UPDATE_COMPLETE',
        'Outputs': [
            {
                'OutputKey': 'JitqLambdaRunnerResultQueue',
                'OutputValue': TEST_RESULT_QUEUE_NAME,
            },
            {
                'OutputKey': 'JitqLambdaRunnerBucket',
                'OutputValue': TEST_BUCKET_NAME,
            },
            {
                'OutputKey': 'JitqLambdaRunnerFunction',
                'OutputValue': TEST_LAMBDA_FUNCTION_NAME,
            },
            {
                'OutputKey': 'JitqLambdaRunnerDeadLetterQueue',
                'OutputValue': TEST_DLQ_NAME,
            },
        ],
    }],
}

_BOTO3_CLIENT_ORIG = boto3.client


def mock_boto3_client(service_name, *args, **kwargs):
    client = _BOTO3_CLIENT_ORIG(service_name, *args, **kwargs)
    if service_name == 'cloudformation':
        # Return hard-coded stack description
        stubber = botocore.stub.Stubber(client)
        expected_params = {'StackName': botocore.stub.ANY}
        for _ in range(2):
            stubber.add_response('describe_stacks', JITQ_MOCK_STACK,
                                 expected_params)
        stubber.activate()
    return client


# pylint: disable=protected-access
_MAKE_API_CALL_ORIG = botocore.client.BaseClient._make_api_call


def mock_make_api_call_sam(self, operation_name, kwargs):
    if operation_name == 'Invoke':
        # Overwrite invocation and log type to the only values supported by
        # sam's local invocation
        kwargs['InvocationType'] = 'RequestResponse'
        kwargs['LogType'] = 'None'
    return _MAKE_API_CALL_ORIG(self, operation_name, kwargs)


def mock_make_api_call_process(self, operation_name, kwargs):
    if operation_name == 'Invoke':
        logger = logging.getLogger(__name__)

        # Call handler function in new Python process
        cwd = os.path.join(os.path.dirname(driver.__file__), 'aws_lambda')

        env = os.environ.copy()
        env['JITQ_LAMBDA_RUNNER_RESULT_QUEUE_URL'] = ''
        env['JITQ_LAMBDA_RUNNER_RESULT_QUEUE_NAME'] = TEST_RESULT_QUEUE_NAME
        env['JITQ_LAMBDA_RUNNER_BUCKET_NAME'] = TEST_BUCKET_NAME
        env['JITQ_LAMBDA_FUNCTION_NAME'] = TEST_LAMBDA_FUNCTION_NAME
        env['OMP_NUM_THREADS'] = '1'
        if 'JITQ_LD_PRELOAD' in env:
            env['LD_PRELOAD'] = env['JITQ_LD_PRELOAD']

        input_ = kwargs['Payload']

        fake_runtime = os.path.join(os.path.dirname(__file__),
                                    'fake_lambda_runtime.py')

        try:
            process = subprocess.run([sys.executable, fake_runtime],
                                     cwd=cwd, env=env, input=input_,
                                     check=True, stdout=subprocess.PIPE,
                                     stderr=subprocess.PIPE)
            output = process.stdout
            errors = process.stderr.decode('utf-8')

        except subprocess.CalledProcessError as ex:
            logger.error(
                'Fake lambda runtime failed. '
                'Output on stderr:\n%s\nOutput on stdout:\n%s',
                ex.stderr.decode('utf-8'),
                ex.stdout.decode('utf-8'))
            raise ex

        if errors:
            logger.warning(
                'Fake lambda runtime produced output on stderr:\n %s', errors)

        return {
            'StatusCode': 200,
            'LogResult': b'',
            'Payload': botocore.response.StreamingBody(io.BytesIO(output),
                                                       len(output)),
            'ExecutedVersion': '',
        }

    return _MAKE_API_CALL_ORIG(self, operation_name, kwargs)


@pytest.fixture
def serverless_stack_type(request):
    stack_type = request.config.getoption('--serverless_stack_type')
    stack_type = stack_type.lower()
    assert stack_type in ['aws', 'sam', 'process']
    return stack_type


@pytest.fixture
def jitq_aws_stack_name(serverless_stack_type):
    # pylint: disable=unused-argument  # fixture produces side-effect

    logger = logging.getLogger(__name__)

    jitq_context = JitqContext()
    stack_name = jitq_context.conf.get('serverless', {}) \
        .get('aws_stack_name', 'test-lambda-stack')

    logger.info('Using stack "%s" of type "%s".',
                stack_name, serverless_stack_type)

    if serverless_stack_type in ['sam', 'process']:
        # Mock boto3 to work with local stack
        with mock.patch('boto3.client', mock_boto3_client):
            new = mock_make_api_call_process \
                if serverless_stack_type == 'process' \
                else mock_make_api_call_sam
            with mock.patch('botocore.client.BaseClient._make_api_call',
                            new=new):
                yield stack_name
    else:
        # Use AWS stack
        yield stack_name


@pytest.fixture
def jitq_aws_stack(serverless_stack_type, jitq_aws_stack_name):
    # Look up meta data of stack components
    cloudformation_client = make_boto3_client('cloudformation')
    res = cloudformation_client.describe_stacks(
        StackName=jitq_aws_stack_name,
    )

    if len(res['Stacks']) != 1:
        raise RuntimeError('Could not find stack: "{}"'.format(
            jitq_aws_stack_name))

    stack = res['Stacks'][0]
    stack_metadata = {o['OutputKey']: o['OutputValue']
                      for o in stack['Outputs']}

    s3_bucket_name = stack_metadata['JitqLambdaRunnerBucket']
    sqs_result_queue_name = stack_metadata['JitqLambdaRunnerResultQueue']
    sqs_dql_name = stack_metadata['JitqLambdaRunnerDeadLetterQueue']

    # Create local components of stack
    s3_prefix = 'jitq/'
    if serverless_stack_type in ['sam', 'process']:
        create_s3_bucket(s3_bucket_name, s3_prefix)
        create_sqs_queue(sqs_result_queue_name)
        create_sqs_queue(sqs_dql_name)
    else:
        clean_s3_bucket(s3_bucket_name, s3_prefix)

    return jitq_aws_stack_name
