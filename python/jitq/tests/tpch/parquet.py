from jitq.utils import get_project_path


class Parquet:
    def __init__(self, input_path, num_partitions, scale_factor, jitq_context):
        self.input_path = input_path
        self.num_partitions = num_partitions
        self.scale_factor = scale_factor
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
            .read_parquet(input_path, select_project,
                          (1, self.num_partitions + 1)) \
            .alias(column_names)
