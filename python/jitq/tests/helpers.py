import argparse
import json


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
