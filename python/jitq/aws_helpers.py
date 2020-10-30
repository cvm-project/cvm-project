import logging
import os
import urllib

import boto3


def compute_config_from_endpoint(endpoint):
    if not endpoint:
        return {}
    res = {'endpoint_url': endpoint}
    url = urllib.parse.urlparse(endpoint)
    if url.scheme == 'http':
        res['use_ssl'] = False
        res['verify'] = False
    return res


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
