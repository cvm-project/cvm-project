import os

import pytest

from jitq.tests.aws_helpers import create_s3_bucket
from jitq.aws_helpers import make_boto3_client


class Filesystem:
    def __init__(self, localdir, *args, **kwargs):
        # pylint: disable=unused-argument
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
    def __init__(self, *args, s3_bucket_name, **kwargs):
        super(S3Filesystem, self).__init__(*args, **kwargs)

        self._s3_client = make_boto3_client('s3')
        self._s3_bucket = s3_bucket_name
        self.remotedir = 's3://' + s3_bucket_name + '/'

        create_s3_bucket(s3_bucket_name, 'jitq-filesystem-test/')

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
def filesystem_instance(filesystem, tmpdir, request, target):
    if target == 'lambda':
        if filesystem != 's3':
            pytest.skip("unsupported configuration")
    bucket = request.config.getoption('--s3_bucket_name')
    return FILESYSTEMS[filesystem](str(tmpdir), s3_bucket_name=bucket)
