import numpy as np
import pandas as pd

from jitq.benchmarks.timer import measure_time
from jitq.jitq_context import JitqContext

MIN_SAL = 1
MAX_SAL = 2**31
N_EMPLOYEES = 2**22
SALARIES = pd.DataFrame()
TEMP = np.arange(0, N_EMPLOYEES)
np.random.shuffle(TEMP)
SALARIES['e_id'] = TEMP
SALARIES['sal'] = np.random.randint(MIN_SAL, MAX_SAL, size=N_EMPLOYEES)


def run_benchmarks():
    for i in range(0, 7):
        n_departments = 2**(13 + i)
        print("number of deps: " + str(n_departments))
        departments = pd.DataFrame()
        temp = np.arange(0, N_EMPLOYEES)
        np.random.shuffle(temp)
        departments['d_id'] = np.random.randint(
            0, n_departments, size=N_EMPLOYEES)
        departments['e_id'] = temp

        dep_emp_sal = pd.merge(departments, SALARIES, on='e_id')

        def bench_pandas_udf(dep_emp_sal=dep_emp_sal):
            def run():
                idx = dep_emp_sal.groupby('d_id').apply(
                    lambda df: df.sal.argmin())
                return dep_emp_sal.ix[idx, :]

            return run

        def bench_pandas(dep_emp_sal=dep_emp_sal):
            def run():
                dep_mins = dep_emp_sal.drop(
                    'e_id', axis=1).groupby('d_id').min().reset_index()
                emp_sal = pd.merge(dep_mins, dep_emp_sal, on=['d_id', 'sal'])
                return emp_sal

            return run

        def bench_jitq(dep_emp_sal=dep_emp_sal):
            context = JitqContext()

            dag = context.collection(dep_emp_sal).reduce_by_key(
                lambda t1, t2: t1 if t1[1] < t2[1] else t2)

            def run():
                dag.collect()

            return run

        print("bench udfs1")
        print("*" * 10)
        print("time pandas " + str(measure_time(bench_pandas_udf(), 1)))

        print("time pandas w/ udf  " + str(measure_time(bench_pandas(), 1)))
        print("time our  " + str(measure_time(bench_jitq(), 1)))


if __name__ == '__main__':
    run_benchmarks()
