import json
import os

import jsonmerge

from pandas import DataFrame

from jitq.rdd import RowScan, GeneratorSource, Range, \
    ColumnScan
from jitq.utils import get_project_path


class JitqContext:
    default_conf = {}

    def __init__(self, **kwargs):
        local_conf = kwargs.get('conf', {})

        env_conf = {}
        env_conf_path = get_project_path() + "/jitq_config.json"
        if os.path.isfile(env_conf_path):
            with open(env_conf_path) as env_conf_file:
                env_conf = json.load(env_conf_file)

        conf = JitqContext.default_conf
        conf = jsonmerge.merge(conf, env_conf)
        conf = jsonmerge.merge(conf, local_conf)
        self.conf = conf

        self.serialization_cache = {}
        self.executor_cache = {}

    def read_csv(self, path, dtype=None):
        raise NotImplementedError

    def collection(self, values, add_index=False):
        if isinstance(values, DataFrame):
            parent, field_names = \
                ColumnScan.make_parent_from_values(self, values)
            return ColumnScan(self, parent, add_index, field_names)
        parent = RowScan.make_parent_from_values(self, values)
        return RowScan(self, parent, add_index)

    def range_(self, from_, to, step=1):
        return Range(self, from_, to, step)

    def generator(self, func):
        return GeneratorSource(self, func)
