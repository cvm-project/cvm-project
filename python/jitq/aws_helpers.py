import logging
import os

import boto3

from jitq.serverless.aws_lambda.utils import compute_config_from_endpoint


def make_boto3_client(service_name, **kwargs):
    env_var_name = 'AWS_{}_ENDPOINT'.format(service_name.upper())
    endpoint_url = os.environ.get(env_var_name, None)
    if endpoint_url is not None:
        logging.info('Using %s endpoint: %s', service_name, endpoint_url)
    config = compute_config_from_endpoint(endpoint_url)
    kwargs.update(config)
    return boto3.client(service_name, **kwargs)


def make_boto3_resource(service_name, **kwargs):
    env_var_name = 'AWS_{}_ENDPOINT'.format(service_name.upper())
    endpoint_url = os.environ.get(env_var_name, None)
    if endpoint_url is not None:
        logging.info('Using %s endpoint: %s', service_name, endpoint_url)
    config = compute_config_from_endpoint(endpoint_url)
    kwargs.update(config)
    return boto3.resource(service_name, **kwargs)
