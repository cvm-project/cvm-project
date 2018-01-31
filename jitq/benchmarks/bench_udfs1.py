import numpy as np
import pandas as pd
from jitq.benchmarks.timer import timer
from jitq.jitq_context import JitqContext

min_sal = 1
max_sal = 2 ** 31
n_emps = 2 ** 22
salaries = pd.DataFrame()
t = np.arange(0, n_emps)
np.random.shuffle(t)
salaries['e_id'] = t
salaries['sal'] = np.random.randint(min_sal, max_sal, size=n_emps)

for i in range(0, 7):
    n_deps = 2 ** (13 + i)
    print("number of deps: " + str(n_deps))
    deps = pd.DataFrame()
    t = np.arange(0, n_emps)
    np.random.shuffle(t)
    deps['d_id'] = np.random.randint(0, n_deps, size=n_emps)
    deps['e_id'] = t

    dep_emp_sal = pd.merge(deps, salaries, on='e_id')


    def bench_pandas_udf():
        def run():
            idx = dep_emp_sal.groupby('d_id').apply(lambda df: df.sal.argmin())
            return dep_emp_sal.ix[idx, :]

        return run


    def bench_pandas():
        def run():
            dep_mins = dep_emp_sal.drop('e_id', axis=1).groupby('d_id').min().reset_index()
            emp_sal = pd.merge(dep_mins, dep_emp_sal, on=['d_id', 'sal'])
            return emp_sal

        return run


    def bench_our():
        bc = JitqContext()

        dag = bc.collection(dep_emp_sal).reduce_by_key(lambda t1, t2: t1 if t1[1] < t2[1] else t2)

        def run():
            dag.collect()

        return run


    print("bench udfs1")
    print("*" * 10)
    t = timer(bench_pandas_udf(), 1)
    print("time pandas " + str(t))

    t = timer(bench_pandas(), 1)
    print("time pandas w/ udf  " + str(t))
    #
    t = timer(bench_our(), 1)
    print("time our  " + str(t))
