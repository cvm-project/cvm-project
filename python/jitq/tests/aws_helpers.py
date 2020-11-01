# pylint: disable=redefined-outer-name  # required by pytest

import logging

import boto3
import botocore

from jitq.aws_helpers import make_boto3_client, make_boto3_resource


def create_s3_bucket(bucket_name, prefix):
    logger = logging.getLogger(__name__)

    resource = make_boto3_resource('s3')
    bucket = resource.Bucket(bucket_name)

    # Create local components of stack
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
