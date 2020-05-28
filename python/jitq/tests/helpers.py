import argparse
import functools
import json

import pytest


def str2bool(value):
    if isinstance(value, bool):
        return value
    if value.lower() in ('yes', 'true', 't', 'y', '1', 'on'):
        return True
    if value.lower() in ('no', 'false', 'f', 'n', '0', 'off'):
        return False
    raise argparse.ArgumentTypeError('Boolean value expected.')


def format_json(json_string):
    return json.dumps(json.loads(json_string), sort_keys=True, indent=4)


def only_targets(targets=None, skip=None):
    targets = targets or []
    skip_targets = skip or []

    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            jitq_context = kwargs['jitq_context']
            target = jitq_context.conf \
                .get('optimizer', {}) \
                .get('target', None)
            if (target not in targets) or \
                    (not targets and target in skip_targets):
                pytest.skip('Target "{}" currently not supported'
                            .format(target))
            func(*args, **kwargs)
        return wrapper
    return decorator
