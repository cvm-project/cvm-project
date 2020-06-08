from jitq.utils import get_project_path


class Parquet:
    def __init__(self, tpch_input_path, tpch_scale, jitq_context):
        self.input_path = tpch_input_path
        self.scale_factor = tpch_scale
        self.jitq_context = jitq_context

    def scan(self, table_name, columns):
        column_names = columns.keys()
        select_project = columns.values()

        input_path = self.input_path.format(
            jitqpath=get_project_path(),
            sf=self.scale_factor,
            format='parquet',
            table=table_name)

        return self.jitq_context \
            .read_parquet(input_path, select_project) \
            .alias(column_names)
