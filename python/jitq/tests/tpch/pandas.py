#!/usr/bin/env python3

import abc
import datetime

from decimal import Decimal

import pandas as pd
import numpy as np

from jitq.utils import get_project_path


def parse_date(string):
    timetuple = datetime.datetime.strptime(string, "%Y-%m-%d").timetuple()
    timestamp = datetime.datetime(
        timetuple.tm_year, timetuple.tm_mon, timetuple.tm_mday,
        tzinfo=datetime.timezone.utc).timestamp()
    return int(timestamp)


def parse_enum(string):
    return int(string.split('#')[1])


class Pandas(abc.ABC):
    def __init__(self, jitq_context):
        self.jitq_context = jitq_context
        self.dataframes = {}

    def _load(self, table_name):
        df = self._do_load(table_name)

        # Store in more narrow integer types
        for column in df.columns.values:
            if df[column].dtype != np.int64:
                continue
            for dtype in [np.uint8, np.int8, np.uint16, np.int16,
                          np.uint32, np.int32]:
                if df[column].min() >= np.iinfo(dtype).min and \
                        df[column].max() <= np.iinfo(dtype).max:
                    df[column] = df[column].astype(dtype)

        # XXX: Work-around for #78
        for column in df.columns.values:
            if df[column].dtype in [np.uint8, np.int8, np.uint16, np.int16,
                                    np.uint32]:
                df[column] = df[column].astype(np.int32)

        return df

    @abc.abstractmethod
    def _do_load(self, table_name):
        pass

    def scan(self, table_name, columns):
        if isinstance(columns, list):
            column_names = columns
        elif isinstance(columns, dict):
            column_names = columns.keys()
        else:
            RuntimeError('Unexpected type of columns: {}'
                         .format(type(columns)))

        if table_name not in self.dataframes:
            self.dataframes[table_name] = self._load(table_name)

        df = self.dataframes[table_name][column_names].copy()
        return self.jitq_context.collection(df)


class PandasCsv(Pandas):
    SCHEMA = {
        'customer': [
            {'col': 'c_custkey', 'type': int},
            {'col': 'c_name', 'type': str},         # char
            {'col': 'c_address', 'type': str},      # varchar
            {'col': 'c_nationkey', 'type': int},    # (nunique: 25)
            {'col': 'c_phone', 'type': str},        # char
            {'col': 'c_acctbal', 'type': Decimal},  # decimal
            {'col': 'c_mktsegment', 'type': str},   # char (nunique: 25)
            {'col': 'c_comment', 'type': str},      # varchar
        ],
        'lineitem': [
            {'col': 'l_orderkey', 'type': int},
            {'col': 'l_partkey', 'type': int},
            {'col': 'l_suppkey', 'type': int},
            {'col': 'l_linenumber', 'type': int},
            {'col': 'l_quantity', 'type': int},
            {'col': 'l_extendedprice', 'type': Decimal},   # decimal
            {'col': 'l_discount', 'type': Decimal},        # decimal
            {'col': 'l_tax', 'type': Decimal},             # decimal
            {'col': 'l_returnflag', 'type': str},          # char(1)
            {'col': 'l_linestatus', 'type': str},          # char(1)
            {'col': 'l_shipdate', 'type': parse_date},     # date
            {'col': 'l_commitdate', 'type': parse_date},   # date
            {'col': 'l_receiptdate', 'type': parse_date},  # date
            {'col': 'l_shipinstruct', 'type': str},
            {'col': 'l_shipmode', 'type': str},
            {'col': 'l_comment', 'type': str},
        ],
        'nation': [
            {'col': 'n_nationkey', 'type': int},    # (nunique: 25)
            {'col': 'n_name', 'type': str},         # char (nunique: 25)
            {'col': 'n_regionkey', 'type': int},    # (nunique: 5)
            {'col': 'n_comment', 'type': str},      # char (nunique: 25)
        ],
        'orders': [
            {'col': 'o_orderkey', 'type': int},
            {'col': 'o_custkey', 'type': int},
            {'col': 'o_orderstatus', 'type': str},       # char(1)
            {'col': 'o_totalprice', 'type': Decimal},    # decimal
            {'col': 'o_orderdate', 'type': parse_date},  # date
            {'col': 'o_orderpriority', 'type': str},     # char(15)
            {'col': 'o_clerk', 'type': str},             # char(15)
            {'col': 'o_shippriority', 'type': int},
            {'col': 'o_comment', 'type': str},
        ],
        'part': [
            {'col': 'p_partkey', 'type': int},
            {'col': 'p_name', 'type': str},          # varchar
            {'col': 'p_mfgr', 'type': parse_enum},   # char (nunique: 5)
            {'col': 'p_brand', 'type': parse_enum},  # char (nunique: 25)
            {'col': 'p_type', 'type': str},          # varchar (nunique: 150)
            {'col': 'p_size', 'type': int},
            {'col': 'p_container', 'type': str},        # char (nunique: 40)
            {'col': 'p_retailprice', 'type': Decimal},  # decimal
            {'col': 'p_comment', 'type': str},          # varchar
        ],
        'partsupp': [
            {'col': 'ps_partkey', 'type': int},
            {'col': 'ps_suppkey', 'type': int},
            {'col': 'ps_availqty', 'type': int},
            {'col': 'ps_supplycost', 'type': Decimal},  # decimal
            {'col': 'ps_comment', 'type': str},         # varchar
        ],
        'region': [
            {'col': 'r_regionkey', 'type': int},    # (nunique: 5)
            {'col': 'r_name', 'type': str},         # char (nunique: 5)
            {'col': 'r_comment', 'type': str},      # char (nunique: 5)
        ],
        'supplier': [
            {'col': 's_suppkey', 'type': int},      #
            {'col': 's_name', 'type': str},         # char
            {'col': 's_address', 'type': str},      # varchar
            {'col': 's_nationkey', 'type': int},    # (nunique: 25)
            {'col': 's_phone', 'type': str},        # char
            {'col': 's_acctbal', 'type': Decimal},  # decimal
            {'col': 's_comment', 'type': str},      # varchar
        ],
    }
    DICTS = {
        'c_mktsegment': ['AUTOMOBILE', 'BUILDING', 'FURNITURE',
                         'HOUSEHOLD', 'MACHINERY'],
        'l_returnflag': ['A', 'N', 'R'],
        'l_linestatus': ['F', 'O'],
        'l_shipinstruct': ['COLLECT COD', 'DELIVER IN PERSON', 'NONE',
                           'TAKE BACK RETURN'],
        'l_shipmode': ['AIR', 'FOB', 'MAIL', 'RAIL', 'REG AIR', 'SHIP',
                       'TRUCK'],
        'n_name': ['ALGERIA', 'ARGENTINA', 'BRAZIL', 'CANADA', 'EGYPT',
                   'ETHIOPIA', 'FRANCE', 'GERMANY', 'INDIA', 'INDONESIA',
                   'IRAN', 'IRAQ', 'JAPAN', 'JORDAN', 'KENYA', 'MOROCCO',
                   'MOZAMBIQUE', 'PERU', 'CHINA', 'ROMANIA',
                   'SAUDI ARABIA', 'VIETNAM', 'RUSSIA', 'UNITED KINGDOM',
                   'UNITED STATES'],
        'o_orderstatus': ['F', 'O', 'P'],
        'o_orderpriority': ['0-DUMMY', '1-URGENT', '2-HIGH', '3-MEDIUM',
                            '4-NOT SPECIFIED', '5-LOW'],
        'p_container': [s1 + ' ' + s2
                        for s1 in ['JUMBO', 'LG', 'MED', 'SM', 'WRAP']
                        for s2 in ['BAG', 'BOX', 'CAN', 'CASE',
                                   'DRUM', 'JAR', 'PACK', 'PKG']
                        ],
        'p_type': [s1 + ' ' + s2 + ' ' + s3
                   for s1 in ['PROMO', 'LARGE', 'STANDARD',
                              'SMALL', 'MEDIUM', 'ECONOMY']
                   for s2 in ['BURNISHED', 'BRUSHED', 'POLISHED',
                              'PLATED', 'ANODIZED']
                   for s3 in ['COPPER', 'BRASS', 'TIN', 'STEEL', 'NICKEL']
                   ],
        'r_name': ['AFRICA', 'AMERICA', 'ASIA', 'EUROPE', 'MIDDLE EAST'],
    }

    def __init__(self, input_path, scale_factor, jitq_context):
        super(PandasCsv, self).__init__(jitq_context)

        self.input_path = input_path
        self.scale_factor = scale_factor

    def _do_load(self, table_name):
        # Load from CSV -------------------------------------------------------
        column_names = [c['col'] for c in self.SCHEMA[table_name]]
        column_types = {c['col']: c['type'] for c in self.SCHEMA[table_name]}

        filename = self.input_path.format(
            jitqpath=get_project_path(),
            sf=self.scale_factor,
            format='csv',
            table=table_name)
        df = pd.read_csv(filename, sep='|', index_col=False,
                         names=column_names, converters=column_types)

        # Dictionary encoding -------------------------------------------------
        for column in df.columns.values:
            if not column in self.DICTS:
                continue
            dict_ = self.DICTS[column]
            dict_ = {v: i for i, v in enumerate(sorted(dict_))}
            df[column] = df[column] \
                .apply(lambda v, d=dict_: d[v]) \
                .astype(np.uint8)

        # Fixed-point numbers -------------------------------------------------
        for col in ['c_acctbal', 'l_discount', 'l_tax', 'l_extendedprice',
                    's_acctbal', 'o_totalprice', 'p_retailprice']:
            if not col in df.columns:
                continue
            df[col] = (df[col] * 100).astype(int)

        return df


class PandasParquet(Pandas):
    TABLES = ['customer', 'lineitem', 'nation', 'orders', 'part', 'partsupp',
              'region', 'supplier']

    def __init__(self, input_path, scale_factor, jitq_context):
        super(PandasParquet, self).__init__(jitq_context)

        self.input_path = input_path
        self.scale_factor = scale_factor

    def _do_load(self, table_name):
        filename = self.input_path.format(
            jitqpath=get_project_path(),
            sf=self.scale_factor,
            format='parquet',
            table=table_name)
        return pd.read_parquet(filename)
