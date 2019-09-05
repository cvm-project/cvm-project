import os

import boto3
import botocore
import pytest


class Filesystem:
    def __init__(self, localdir):
        self.localdir = localdir
        self.remotedir = localdir

    def copy_from_remote(self, filename):
        pass

    def copy_to_remote(self, filename):
        pass

    def remove_from_remote(self, filename):
        pass

    def to_local(self, filename):
        return os.path.join(self.localdir, filename)

    def to_remote(self, filename):
        return os.path.join(self.remotedir, filename)


class S3Filesystem(Filesystem):
    def __init__(self, *args, **kwargs):
        super(S3Filesystem, self).__init__(*args, **kwargs)

        self._s3_bucket = 'mybucket'
        self.remotedir = 's3://mybucket/'

        s3_config = {
            'use_ssl': False,
            'verify': False,
        }
        if 'AWS_S3_ENDPOINT' in os.environ:
            s3_config['endpoint_url'] = os.environ['AWS_S3_ENDPOINT']

        self._s3_client = boto3.client('s3', **s3_config)

        try:
            self._s3_client.create_bucket(
                Bucket=self._s3_bucket,
                CreateBucketConfiguration={
                    'LocationConstraint': boto3.session.Session().region_name,
                },
            )
        except botocore.exceptions.ClientError as ex:
            if ex.response['Error']['Code'] != 'BucketAlreadyOwnedByYou':
                raise ex

    def copy_from_remote(self, filename):
        self._s3_client.download_file(
            Filename=self.to_local(filename),
            Bucket=self._s3_bucket,
            Key=filename,
        )

    def copy_to_remote(self, filename):
        self._s3_client.upload_file(
            Filename=self.to_local(filename),
            Bucket=self._s3_bucket,
            Key=filename,
        )

    def remove_from_remote(self, filename):
        self._s3_client.delete_objects(
            Bucket=self._s3_bucket,
            Delete={'Objects': [{'Key': filename}]},
        )


FILESYSTEMS = {
    'file': Filesystem,
    's3': S3Filesystem,
}


@pytest.fixture
def filesystem_instance(filesystem, tmpdir):
    return FILESYSTEMS[filesystem](str(tmpdir))
