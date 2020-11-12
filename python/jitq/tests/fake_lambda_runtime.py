#!/usr/bin/env python3

import json
import os
from unittest import mock
import sys

from jitq.tests.aws_helpers import mock_make_api_call_process


def main():
    # Assuming this script is started in the folder where the handler is, we
    # need to add the current directory to where Python searches packages
    sys.path.append(os.getcwd())

    with mock.patch('botocore.client.BaseClient._make_api_call',
                    new=mock_make_api_call_process):
        # pylint: disable=import-error
        #         pylint doesn't know about modified sys.path
        # pylint: disable=import-outside-toplevel
        #         this needs to happen after modifying sys.path
        import handler

        event = json.load(sys.stdin)
        context = None

        result = handler.run(event, context)

        json.dump(result, sys.stdout)


if __name__ == '__main__':
    main()
