#!/usr/bin/env python3

# pylint: disable=too-many-lines  # Nothing we can do...

from abc import ABC, abstractmethod
from decimal import Decimal

import datetime as dt
import numba as nb
import numpy as np
import pandas as pd

from jitq.tests.tpch.pandas import parse_date, PandasCsv


@nb.njit
def extract_year(date):
    secsperday = 24 * 60 * 60
    secsperyear = secsperday * 365
    leapyears = ((date // secsperyear + 1) // 4)
    return 1970 + (date - leapyears * secsperday) // secsperyear


class TpchJitqQuery(ABC):
    def __init__(self, context):
        self.context = context

    @abstractmethod
    def load(self, database):
        pass

    @abstractmethod
    def run(self, scans):
        pass

    @abstractmethod
    def postprocess(self, res):
        pass


class Q01(TpchJitqQuery):
    # select
    #     l_returnflag,
    #     l_linestatus,
    #     sum(l_quantity) as sum_qty,
    #     sum(l_extendedprice) as sum_base_price,
    #     sum(l_extendedprice * (1 - l_discount)) as sum_disc_price,
    #     sum(l_extendedprice * (1 - l_discount) * (1 + l_tax))
    #             as sum_charge,
    #     avg(l_quantity) as avg_qty,
    #     avg(l_extendedprice) as avg_price,
    #     avg(l_discount) as avg_disc,
    #     count(*) as count_order
    # from
    #     lineitem
    # where
    #     l_shipdate <= date '1998-12-01' - interval '90' day
    # group by
    #     l_returnflag,
    #     l_linestatus
    # order by
    #     l_returnflag,
    #     l_linestatus

    def load(self, database):
        lineitem_scan = database.scan('lineitem', {
            'l_quantity': (4, nb.int64),
            'l_extendedprice': (5, nb.int64),
            'l_discount': (6, nb.int64),
            'l_tax': (7, nb.int64),
            'l_returnflag': (8, nb.int64),
            'l_linestatus': (9, nb.int64),
            'l_shipdate': (10, nb.int64),
        })

        return {
            'lineitem': lineitem_scan,
        }

    def run(self, scans):
        b_shipdate = parse_date('1998-09-02')

        return scans['lineitem'] \
            .filter(lambda r: r.l_shipdate <= b_shipdate) \
            .map(lambda r:
                 (r.l_returnflag * 2 + r.l_linestatus,
                  np.int64(r.l_quantity),
                  np.int64(r.l_extendedprice),
                  r.l_extendedprice * (100 - r.l_discount),
                  r.l_extendedprice * (100 - r.l_discount) *
                  (100 + r.l_tax),
                  np.int64(r.l_discount),
                  1),
                 names=['returnflag_linestatus', 'l_quantity',
                        'l_extendedprice', 'discounted', 'disc_taxed',
                        'l_discount', 'count']) \
            .reduce_by_key(lambda r1, r2:
                           (r1.l_quantity + r2.l_quantity,
                            r1.l_extendedprice + r2.l_extendedprice,
                            r1.discounted + r2.discounted,
                            r1.disc_taxed + r2.disc_taxed,
                            r1.l_discount + r2.l_discount,
                            r1.count + r2.count)) \
            .sort() \
            .map(lambda r:
                 (r.returnflag_linestatus >> 1,
                  r.returnflag_linestatus & 1,
                  r.l_quantity,
                  r.l_extendedprice,
                  r.discounted,
                  r.disc_taxed,
                  r.l_quantity / r.count,
                  r.l_extendedprice / r.count,
                  r.l_discount / r.count,
                  r.count),
                 names=['l_returnflag', 'l_linestatus', 'sum_qty',
                        'sum_base_price', 'sum_disc_price', 'sum_charge',
                        'avg_qty', 'avg_price', 'avg_disc', 'count_order']) \
            .collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        df.l_returnflag = df.l_returnflag.apply(
            lambda i: ['A', 'N', 'R'][i])
        df.l_linestatus = df.l_linestatus.apply(
            lambda i: ['F', 'O'][i])
        df.sum_base_price = df.sum_base_price.apply(Decimal) / 100
        df.sum_disc_price = df.sum_disc_price.apply(Decimal) / 100 / 100
        df.sum_charge = df.sum_charge.apply(Decimal) / 100 / 100 / 100

        return df


class Q02(TpchJitqQuery):
    # select
    #     s_acctbal,
    #     s_name,
    #     n_name,
    #     p_partkey,
    #     p_mfgr,
    #     s_address,
    #     s_phone,
    #     s_comment
    # from
    #     part,
    #     supplier,
    #     partsupp,
    #     nation,
    #     region
    # where
    #     p_partkey = ps_partkey
    #     and s_suppkey = ps_suppkey
    #     and p_size = 15
    #     and p_type like '%BRASS'
    #     and s_nationkey = n_nationkey
    #     and n_regionkey = r_regionkey
    #     and r_name = 'EUROPE'
    #     and ps_supplycost = (
    #             select
    #                     min(ps_supplycost)
    #             from
    #                     partsupp,
    #                     supplier,
    #                     nation,
    #                     region
    #             where
    #                     p_partkey = ps_partkey
    #                     and s_suppkey = ps_suppkey
    #                     and s_nationkey = n_nationkey
    #                     and n_regionkey = r_regionkey
    #                     and r_name = 'EUROPE'
    #     )
    # order by
    #     s_acctbal desc,
    #     n_name,
    #     s_name,
    #     p_partkey
    # limit 100

    def load(self, database):
        nation_scan = database.scan('nation', {
            'n_nationkey': (0, nb.int64),
            'n_name': (1, nb.int64),
            'n_regionkey': (2, nb.int64),
        })
        part_scan = database.scan('part', {
            'p_partkey': (0, nb.int64),
            'p_mfgr': (2, nb.int64),
            'p_type': (4, nb.int64),
            'p_size': (5, nb.int64),
        })
        partsupp_scan = database.scan('partsupp', {
            'ps_partkey': (0, nb.int64),
            'ps_suppkey': (1, nb.int64),
            'ps_supplycost': (3, nb.int64),
        })
        region_scan = database.scan('region', {
            'r_regionkey': (0, nb.int64),
            'r_name': (1, nb.int64),
        })
        supplier_scan = database.scan('supplier', {
            's_suppkey': (0, nb.int64),
            's_name': (1, nb.int64),
            's_nationkey': (3, nb.int64),
            's_phone': (4, nb.int64),
            's_acctbal': (5, nb.int64),
        })

        return {
            'nation': nation_scan,
            'part': part_scan,
            'partsupp': partsupp_scan,
            'region': region_scan,
            'supplier': supplier_scan,
        }

    def run(self, scans):
        partsupp_scan = scans['partsupp'] \
            .map(lambda r:
                 (r.ps_suppkey, r.ps_partkey, r.ps_supplycost))

        nation_scan = scans['nation'] \
            .map(lambda r: (r.n_regionkey, r.n_nationkey, r.n_name))

        supplier_scan = scans['supplier'] \
            .map(lambda r:
                 (r.s_nationkey, r.s_suppkey, r.s_acctbal, r.s_phone,
                  r.s_name))

        region_scan = scans['region'] \
            .filter(lambda r: r.r_name == 3) \
            .map(lambda r: r.r_regionkey) \
            .join(nation_scan) \
            .alias(['n_regionkey', 'n_nationkey', 'n_name']) \
            .map(lambda r: (r.n_nationkey, r.n_name)) \
            .join(supplier_scan) \
            .alias(['n_nationkey', 'n_name', 's_suppkey', 's_acctbal',
                    's_phone', 's_name']) \
            .map(lambda r:
                 (r.s_suppkey, r.s_acctbal, r.s_phone, r.s_name, r.n_name)) \
            .join(partsupp_scan) \
            .alias(['s_suppkey', 's_acctbal', 's_phone', 's_name', 'n_name',
                    'ps_partkey', 'ps_supplycost']) \
            .map(lambda r:
                 (r.ps_partkey, r.s_suppkey, r.ps_supplycost, r.s_acctbal,
                  r.s_phone, r.s_name, r.n_name))

        part_min_price = region_scan \
            .map(lambda t: (t[0], t[2])) \
            .reduce_by_key(lambda i1, i2: (i1 if i1 < i2 else i2))

        # XXX: replace join on part_min_price + filter with new
        #      multi-attribute join
        part = scans['part'] \
            .filter(lambda r: r.p_size == 15 and (r.p_type % 5) == 0) \
            .map(lambda r: (r.p_partkey, r.p_mfgr)) \
            .join(region_scan) \
            .alias(['partkey', 'p_mfgr', 'ps_suppkey', 'ps_supplycost',
                    's_acctbal', 's_phone', 's_name', 'n_name']) \
            .map(lambda r:
                 (r.partkey, r.p_mfgr, r.ps_suppkey, r.ps_supplycost,
                  r.s_acctbal, r.s_phone, r.s_name, r.n_name)) \
            .join(part_min_price) \
            .alias(['p_partkey', 'p_mfgr', 'ps_suppkey', 'ps_supplycost',
                    's_acctbal', 's_phone', 's_name', 'n_name',
                    'min_supplycost']) \
            .filter(lambda r: r.ps_supplycost == r.min_supplycost) \
            .map(lambda r:
                 ((r.s_acctbal << 42) |
                  ((~r.n_name & ((1 << 7) - 1)) << 35) |
                  (~r.p_partkey) & ((1 << 18) - 1),
                  r.p_mfgr,
                  r.s_phone,
                  r.s_name),
                 names=['acctbal_name_suppkey_partkey', 'p_mfgr', 's_phone',
                        's_name']) \
            .topk(100) \
            .map(lambda t:
                 (t.acctbal_name_suppkey_partkey >> 42,
                  t.s_name,
                  (~t.acctbal_name_suppkey_partkey >> 35) & ((1 << 7) - 1),
                  ~t.acctbal_name_suppkey_partkey & ((1 << 18) - 1),
                  t.p_mfgr,
                  t.s_phone),
                 names=['s_acctbal', 's_name', 'n_name', 'p_partkey',
                        'p_mfgr', 's_phone'])

        return part.collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        df.s_acctbal = df.s_acctbal.apply(Decimal) / 100
        nationdict = sorted(PandasCsv.DICTS['n_name'])
        df.n_name = df.n_name.apply(lambda i: (nationdict[i]))
        df.s_name = df.s_name.apply('Supplier#{:09}'.format)
        df.p_mfgr = df.p_mfgr.apply('Manufacturer#{}'.format)

        return df


class Q03(TpchJitqQuery):
    # select
    #		l_orderkey,
    # 		sum(l_extendedprice*(1-l_discount)) as revenue,
    # 		o_orderdate,
    #		o_shippriority
    # from
    # 		customer,
    # 		orders,
    # 		lineitem
    # where
    # 		c_mktsegment = 'BUILDING'
    # 		and c_custkey = o_custkey
    #		and l_orderkey = o_orderkey
    #		and o_orderdate < date '1995-03-15'
    # 		and l_shipdate > date '1995-03-15'
    # group by
    # 		l_orderkey,
    # 		o_orderdate,
    # 		o_shippriority
    # order by
    # 		revenue desc,
    # 		o_orderdate;

    def load(self, database):
        customer_scan = database.scan('customer', {
            'c_custkey': (0, nb.int64),
            'c_mktsegment': (6, nb.int64),
        })
        lineitem_scan = database.scan('lineitem', {
            'l_orderkey': (0, nb.int64),
            'l_extendedprice': (5, nb.int64),
            'l_discount': (6, nb.int64),
            'l_shipdate': (10, nb.int64),
        })
        orders_scan = database.scan('orders', {
            'o_orderkey': (0, nb.int64),
            'o_custkey': (1, nb.int64),
            'o_orderdate': (4, nb.int64),
            'o_shippriority': (7, nb.int64),
        })

        return {
            'customer': customer_scan,
            'lineitem': lineitem_scan,
            'orders': orders_scan,
        }

    def run(self, scans):
        hb_orderdate = parse_date('1995-03-15')
        lb_shipdate = parse_date('1995-03-15')

        orders_scan = scans['orders'] \
            .filter(lambda r: r.o_orderdate < hb_orderdate) \
            .map(lambda r: (r.o_custkey, r.o_orderkey,
                            r.o_orderdate, r.o_shippriority))

        lineitem_scan = scans['lineitem'] \
            .filter(lambda r: r.l_shipdate > lb_shipdate) \
            .map(lambda r:
                 (np.int64(r.l_orderkey),
                  r.l_extendedprice * (100 - r.l_discount)))

        customer_scan = scans['customer'] \
            .filter(lambda r: r.c_mktsegment == 1) \
            .map(lambda r: r.c_custkey) \
            .join(orders_scan) \
            .alias(['o_custkey', 'o_orderkey',
                    'o_orderdate', 'o_shippriority']) \
            .map(lambda r: (np.int64(r.o_orderkey),
                            r.o_orderdate, r.o_shippriority)) \
            .join(lineitem_scan) \
            .alias(['l_orderkey', 'o_orderdate',
                    'o_shippriority', 'l_extendedprice']) \
            .map(lambda r:
                 (((r.o_orderdate // (24 * 60 * 60)) << 35) +
                  (r.o_shippriority << 32) +
                  r.l_orderkey,
                  r.l_extendedprice),
                 names=['combinedkey', 'revenue']) \
            .reduce_by_key(lambda i1, i2: (i1 + i2)) \
            .map(lambda r:
                 (((1 << 32) - 1) & r.combinedkey,
                  ((1 << 3) - 1) & (r.combinedkey >> 32),
                  r.combinedkey >> 35,
                  r.revenue),
                 names=['l_orderkey', 'o_shippriority',
                        'o_orderdate', 'revenue']) \
            .map(lambda r:
                 ((r.revenue << 16) + r.o_orderdate,
                  r.l_orderkey,
                  r.o_shippriority),
                 names=['revenue_orderdate', 'l_orderkey', 'o_shippriority']) \
            .topk(10) \
            .map(lambda r:
                 (r.l_orderkey,
                  r.revenue_orderdate >> 16,
                  (((1 << 16) - 1) & r.revenue_orderdate) * 24 * 60 * 60,
                  r.o_shippriority),
                 names=['l_orderkey', 'revenue',
                        'o_orderdate', 'o_shippriority'])

        return customer_scan.collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        df.revenue = df.revenue.apply(Decimal) / 100 / 100
        df.o_orderdate = df.o_orderdate.apply(dt.date.fromtimestamp)

        return df


class Q04(TpchJitqQuery):
    # select
    #     o_orderpriority,
    #     count(*) as order_count
    # from
    #     orders
    # where
    #     o_orderdate >= date '1993-07-01'
    #     and o_orderdate < date '1993-07-01' + interval '3' month
    #     and exists (
    #         select
    #             *
    #         from
    #             lineitem
    #         where
    #             l_orderkey = o_orderkey
    #             and l_commitdate < l_receiptdate
    #     )
    # group by
    #         o_orderpriority
    # order by
    #         o_orderpriority;

    def load(self, database):
        lineitem_scan = database.scan('lineitem', {
            'l_orderkey': (0, nb.int64),
            'l_commitdate': (11, nb.int64),
            'l_receiptdate': (12, nb.int64),
        })
        orders_scan = database.scan('orders', {
            'o_orderkey': (0, nb.int64),
            'o_orderdate': (4, nb.int64),
            'o_orderpriority': (5, nb.int64),
        })

        return {
            'lineitem': lineitem_scan,
            'orders': orders_scan,
        }

    def run(self, scans):
        lb_orderdate = parse_date('1993-07-01')
        hb_orderdate = parse_date('1993-10-01')

        orders_scan = scans['orders'] \
            .filter(lambda r: lb_orderdate <= r.o_orderdate < hb_orderdate) \
            .map(lambda r: (r.o_orderkey, r.o_orderpriority))

        lineitem_scan = scans['lineitem'] \
            .filter(lambda r: r.l_commitdate < r.l_receiptdate) \
            .map(lambda t: t.l_orderkey)

        return orders_scan \
            .semijoin(lineitem_scan) \
            .map(lambda t: (t[1], np.int32(1)),
                 ['o_orderpriority', 'order_count']) \
            .reduce_by_key(lambda i1, i2: np.int32(i1 + i2)) \
            .sort() \
            .collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        orderpriorities = ['1-URGENT', '2-HIGH', '3-MEDIUM',
                           '4-NOT SPECIFIED', '5-LOW']
        df.o_orderpriority = df.o_orderpriority \
            .apply(lambda i: orderpriorities[i - 1])

        return df


class Q05(TpchJitqQuery):
    # select
    #     n_name,
    #     sum(l_extendedprice * (1 - l_discount)) as revenue
    # from
    #     customer,
    #     orders,
    #     lineitem,
    #     supplier,
    #     nation,
    #     region
    # where
    #     c_custkey = o_custkey
    #     and l_orderkey = o_orderkey
    #     and l_suppkey = s_suppkey
    #     and c_nationkey = s_nationkey
    #     and s_nationkey = n_nationkey
    #     and n_regionkey = r_regionkey
    #     and r_name = 'ASIA'
    #     and o_orderdate >= date '1994-01-01'
    #     and o_orderdate < date '1995-01-01'
    # group by
    #     n_name
    # order by
    #     revenue desc

    def load(self, database):
        customer_scan = database.scan('customer', {
            'c_custkey': (0, nb.int64),
            'c_nationkey': (3, nb.int64),
        })
        lineitem_scan = database.scan('lineitem', {
            'l_orderkey': (0, nb.int64),
            'l_suppkey': (2, nb.int64),
            'l_extendedprice': (5, nb.int64),
            'l_discount': (6, nb.int64),
        })
        nation_scan = database.scan('nation', {
            'n_nationkey': (0, nb.int64),
            'n_name': (1, nb.int64),
            'n_regionkey': (2, nb.int64),
        })
        orders_scan = database.scan('orders', {
            'o_orderkey': (0, nb.int64),
            'o_custkey': (1, nb.int64),
            'o_orderdate': (4, nb.int64),
        })
        region_scan = database.scan('region', {
            'r_regionkey': (0, nb.int64),
            'r_name': (1, nb.int64),
        })
        supplier_scan = database.scan('supplier', {
            's_suppkey': (0, nb.int64),
            's_nationkey': (3, nb.int64),
        })

        return {
            'customer': customer_scan,
            'lineitem': lineitem_scan,
            'nation': nation_scan,
            'orders': orders_scan,
            'region': region_scan,
            'supplier': supplier_scan,
        }

    def run(self, scans):
        lb_orderdate = parse_date('1994-01-01')
        hb_orderdate = parse_date('1995-01-01')

        nation_scan = scans['nation'] \
            .map(lambda r: (r.n_regionkey, r.n_nationkey, r.n_name))

        supplier_scan = scans['supplier'] \
            .map(lambda r: (r.s_nationkey, r.s_suppkey))

        lineitem_scan = scans['lineitem'] \
            .map(lambda r:
                 (r.l_orderkey,
                  r.l_suppkey,
                  r.l_extendedprice * (100 - r.l_discount)))

        orders_scan = scans['orders'] \
            .filter(lambda r: (lb_orderdate <= r.o_orderdate < hb_orderdate)) \
            .map(lambda r: (r.o_orderkey, r.o_custkey)) \
            .join(lineitem_scan) \
            .alias(['l_orderkey', 'o_custkey', 'l_suppkey', 'revenue']) \
            .map(lambda r: (r.o_custkey, r.l_suppkey, r.revenue)) \

        customer_scan = scans['customer'] \
            .map(lambda r: (r.c_custkey, r.c_nationkey)) \
            .join(orders_scan) \
            .alias(['o_custkey', 'c_nationkey', 'l_suppkey', 'revenue']) \
            .map(lambda r: (r.c_nationkey, r.l_suppkey, r.revenue)) \

        return scans['region'] \
            .filter(lambda r: (r.r_name == 2)) \
            .map(lambda r: r.r_regionkey) \
            .join(nation_scan) \
            .alias(['n_regionkey', 'n_nationkey', 'n_name']) \
            .map(lambda r: (r.n_nationkey, r.n_name)) \
            .join(supplier_scan) \
            .alias(['s_nationkey', 'n_name', 's_suppkey']) \
            .map(lambda r: (r.s_nationkey, r.s_suppkey, r.n_name)) \
            .join(customer_scan, 2) \
            .alias(['c_nationkey', 'l_suppkey', 'n_name', 'revenue']) \
            .map(lambda r: (r.n_name, r.revenue),
                 names=['n_name', 'revenue']) \
            .reduce_by_key(lambda i1, i2: i1 + i2) \
            .map(lambda r: (-r.revenue, r.n_name),
                 names=['revenue', 'n_name']) \
            .sort() \
            .map(lambda r: (r.n_name, -r.revenue),
                 names=['n_name', 'revenue']) \
            .collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        df.revenue = df.revenue.apply(Decimal) / 100 / 100
        nationdict = sorted(PandasCsv.DICTS['n_name'])
        df.n_name = df.n_name.apply(lambda i: (nationdict[i]))

        return df


class Q06(TpchJitqQuery):
    # select
    #     sum(l_extendedprice * l_discount) as revenue
    # from
    #     lineitem
    # where
    #     l_shipdate >= date '1994-01-01'
    #     and l_shipdate < date '1995-01-01'
    #     and l_discount between 0.06 - 0.01 and 0.06 + 0.01
    #     and l_quantity < 24

    def load(self, database):
        lineitem_scan = database.scan('lineitem', {
            'l_quantity': (4, nb.int64),
            'l_extendedprice': (5, nb.int64),
            'l_discount': (6, nb.int64),
            'l_shipdate': (10, nb.int64),
        })

        return {
            'lineitem': lineitem_scan,
        }

    def run(self, scans):
        lb_shipdate = parse_date('1994-01-01')
        hb_shipdate = parse_date('1995-01-01')

        return scans['lineitem'] \
            .filter(lambda r:
                    r.l_shipdate >= lb_shipdate and
                    r.l_shipdate < hb_shipdate and
                    r.l_discount >= 5 and
                    r.l_discount <= 7 and
                    r.l_quantity < 24
                    ) \
            .map(lambda r: r.l_discount * r.l_extendedprice) \
            .reduce(lambda i1, i2: i1 + i2)

    def postprocess(self, res):
        res = Decimal(res) / 100 / 100
        return pd.DataFrame([res], columns=['revenue'])


class Q07(TpchJitqQuery):
    XFAIL = {'target': ['process']}

    # select
    #     supp_nation,
    #     cust_nation,
    #     l_year,
    #     sum(volume) as revenue
    # from (
    #     select
    #         n1.n_name as supp_nation,
    #         n2.n_name as cust_nation,
    #         extract(year from l_shipdate) as l_year,
    #         l_extendedprice * (1 - l_discount) as volume
    #     from
    #         supplier,
    #         lineitem,
    #         orders,
    #         customer,
    #         nation n1,
    #         nation n2
    #     where
    #         s_suppkey = l_suppkey
    #         and o_orderkey = l_orderkey
    #         and c_custkey = o_custkey
    #         and s_nationkey = n1.n_nationkey
    #         and c_nationkey = n2.n_nationkey
    #         and (
    #             (n1.n_name = 'FRANCE' and n2.n_name = 'GERMANY') or
    #             (n1.n_name = 'GERMANY' and n2.n_name = 'FRANCE')
    #         )
    #         and l_shipdate between date '1995-01-01' and date '1996-12-31'
    #     ) as shipping
    # group by
    #     supp_nation,
    #     cust_nation,
    #     l_year
    # order by
    #     supp_nation,
    #     cust_nation,
    #     l_year

    def load(self, database):
        customer_scan = database.scan('customer', {
            'c_custkey': (0, nb.int64),
            'c_nationkey': (3, nb.int64),
        })
        lineitem_scan = database.scan('lineitem', {
            'l_orderkey': (0, nb.int64),
            'l_suppkey': (2, nb.int64),
            'l_extendedprice': (5, nb.int64),
            'l_discount': (6, nb.int64),
            'l_shipdate': (10, nb.int64),
        })
        nation_scan = database.scan('nation', {
            'n_nationkey': (0, nb.int64),
            'n_name': (1, nb.int64),
        })
        orders_scan = database.scan('orders', {
            'o_orderkey': (0, nb.int64),
            'o_custkey': (1, nb.int64),
        })
        supplier_scan = database.scan('supplier', {
            's_suppkey': (0, nb.int64),
            's_nationkey': (3, nb.int64),
        })

        return {
            'customer': customer_scan,
            'nation': nation_scan,
            'lineitem': lineitem_scan,
            'orders': orders_scan,
            'supplier': supplier_scan,
        }

    def run(self, scans):
        lb_shipdate = parse_date('1995-01-01')
        hb_shiptdate = parse_date('1996-12-31')

        customer_scan = scans['customer'] \
            .map(lambda r: (r.c_nationkey, r.c_custkey))

        orders_scan = scans['orders'] \
            .map(lambda r: (r.o_custkey, r.o_orderkey))

        lineitem_scan = scans['lineitem'] \
            .filter(lambda r: (lb_shipdate <= r.l_shipdate <= hb_shiptdate)) \
            .map(lambda r:
                 (r.l_orderkey, r.l_suppkey, r.l_extendedprice, r.l_discount,
                  r.l_shipdate))

        filtered_nations = scans['nation'] \
            .filter(lambda r: r.n_name in [7, 8]) \
            .map(lambda r: (r.n_nationkey, r.n_name))

        nation2 = filtered_nations \
            .cartesian(filtered_nations) \
            .alias(['n2_nationkey', 'n2_name', 'n1_nationkey', 'n1_name']) \
            .filter(lambda r: (r.n1_nationkey != r.n2_nationkey)) \
            .map(lambda r:
                 (r.n2_nationkey, r.n2_name, r.n1_nationkey, r.n1_name)) \
            .join(customer_scan) \
            .alias(['n2_nationkey', 'n2_name', 'n1_nationkey', 'n1_name',
                    'c_custkey']) \
            .map(lambda r:
                 (r.c_custkey, r.n1_nationkey, r.n1_name, r.n2_name)) \
            .join(orders_scan) \
            .alias(['o_custkey', 'n1_nationkey', 'n1_name', 'n2_name',
                    'o_orderkey']) \
            .map(lambda r:
                 (r.o_orderkey, r.n1_nationkey, r.n1_name, r.n2_name)) \
            .join(lineitem_scan) \
            .alias(['l_orderkey', 'n1_nationkey', 'n1_name', 'n2_name',
                    'l_suppkey', 'l_extendedprice', 'l_discount',
                    'l_shipdate']) \
            .map(lambda r:
                 (r.l_suppkey, r.n1_nationkey, r.n1_name, r.n2_name,
                  r.l_extendedprice, r.l_discount, r.l_shipdate))

        supplier_scan = scans['supplier'] \
            .map(lambda r: (r.s_suppkey, r.s_nationkey)) \
            .join(nation2, 2) \
            .alias(['l_suppkey', 'n1_nationkey', 'n1_name', 'n2_name',
                    'l_extendedprice', 'l_discount', 'l_shipdate']) \
            .map(lambda r:
                 (r.n1_name,
                  r.n2_name,
                  extract_year(r.l_shipdate),
                  r.l_extendedprice * (100 - r.l_discount)),
                 names=['n1_name', 'n2_name', 'l_year', 'revenue']) \
            .map(lambda r:
                 ((r.n1_name << 38) +
                  (r.n2_name << 32) +
                  r.l_year,
                  r.revenue),
                 names=['n1_n2_name_year', 'revenue']) \
            .reduce_by_key(lambda i1, i2: (i1 + i2)) \
            .sort() \
            .map(lambda r:
                 (r.n1_n2_name_year >> 38,
                  (r.n1_n2_name_year >> 32) & ((1 << 6) - 1),
                  r.revenue,
                  r.n1_n2_name_year & ((1 << 32) - 1)),
                 names=['suppnation', 'custnation', 'revenue', 'l_year'])

        return supplier_scan.collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        df.revenue = df.revenue.apply(Decimal) / 100 / 100
        nationdict = sorted(PandasCsv.DICTS['n_name'])
        df.suppnation = df.suppnation.apply(lambda i: (nationdict[i]))
        df.custnation = df.custnation.apply(lambda i: (nationdict[i]))

        return df


class Q08(TpchJitqQuery):
    # select
    #     o_year,
    #     sum(case
    #         when nation = 'BRAZIL'
    #         then volume
    #         else 0
    #     end) / sum(volume) as mkt_share
    # from (
    #     select
    #         extract(year from o_orderdate) as o_year,
    #         l_extendedprice * (1-l_discount) as volume,
    #         n2.n_name as nation
    #     from
    #         part,
    #         supplier,
    #         lineitem,
    #         orders,
    #         customer,
    #         nation n1,
    #         nation n2,
    #         region
    #     where
    #        p_partkey = l_partkey
    #         and s_suppkey = l_suppkey
    #         and l_orderkey = o_orderkey
    #         and o_custkey = c_custkey
    #         and c_nationkey = n1.n_nationkey
    #         and n1.n_regionkey = r_regionkey
    #         and r_name = 'AMERICA'
    #         and s_nationkey = n2.n_nationkey
    #         and o_orderdate between date '1995-01-01' and date '1996-12-31'
    #         and p_type = 'ECONOMY ANODIZED STEEL'
    #     ) as all_nations
    # group by
    #     o_year
    # order by
    #     o_year;

    def load(self, database):
        customer_scan = database.scan('customer', {
            'c_custkey': (0, nb.int64),
            'c_nationkey': (3, nb.int64),
        })
        lineitem_scan = database.scan('lineitem', {
            'l_orderkey': (0, nb.int64),
            'l_partkey': (1, nb.int64),
            'l_suppkey': (2, nb.int64),
            'l_extendedprice': (5, nb.int64),
            'l_discount': (6, nb.int64),
        })
        nation_scan = database.scan('nation', {
            'n_nationkey': (0, nb.int64),
            'n_name': (1, nb.int64),
            'n_regionkey': (2, nb.int64),
        })
        orders_scan = database.scan('orders', {
            'o_orderkey': (0, nb.int64),
            'o_custkey': (1, nb.int64),
            'o_orderdate': (4, nb.int64),
        })
        part_scan = database.scan('part', {
            'p_partkey': (0, nb.int64),
            'p_type': (4, nb.int64),
        })
        region_scan = database.scan('region', {
            'r_regionkey': (0, nb.int64),
            'r_name': (1, nb.int64),
        })
        supplier_scan = database.scan('supplier', {
            's_suppkey': (0, nb.int64),
            's_nationkey': (3, nb.int64),
        })

        return {
            'customer': customer_scan,
            'lineitem': lineitem_scan,
            'nation': nation_scan,
            'orders': orders_scan,
            'part': part_scan,
            'supplier': supplier_scan,
            'region': region_scan,
        }

    def run(self, scans):
        lb_orderdate = parse_date('1995-01-01')
        hb_orderdate = parse_date('1996-12-31')

        lineitem_scan = scans['lineitem'] \
            .map(lambda r:
                 (r.l_partkey, r.l_orderkey, r.l_suppkey, r.l_extendedprice,
                  r.l_discount))

        orders_scan = scans['orders'] \
            .filter(lambda r: lb_orderdate <= r.o_orderdate <= hb_orderdate) \
            .map(lambda r: (r.o_orderkey, r.o_custkey, r.o_orderdate))

        customer_scan = scans['customer'] \
            .map(lambda r: (r.c_custkey, r.c_nationkey))

        part_scan = scans['part'] \
            .filter(lambda r: (r.p_type == 3)) \
            .map(lambda r: r.p_partkey) \
            .join(lineitem_scan) \
            .alias(['l_partkey', 'l_orderkey', 'l_suppkey', 'l_extendedprice',
                    'l_discount']) \
            .map(lambda r:
                 (r.l_orderkey, r.l_suppkey, r.l_extendedprice,
                  r.l_discount)) \
            .join(orders_scan) \
            .alias(['l_orderkey', 'l_suppkey', 'l_extendedprice', 'l_discount',
                    'o_custkey', 'o_orderdate']) \
            .map(lambda r:
                 (r.o_custkey, r.l_suppkey, r.l_extendedprice, r.l_discount,
                  r.o_orderdate)) \
            .join(customer_scan) \
            .alias(['o_custkey', 'l_suppkey', 'l_extendedprice', 'l_discount',
                    'o_orderdate', 'c_nationkey']) \
            .map(lambda r:
                 (r.c_nationkey, r.l_suppkey, r.l_extendedprice, r.l_discount,
                  r.o_orderdate))

        supplier_scan = scans['supplier'] \
            .map(lambda r: (r.s_suppkey, r.s_nationkey))

        region_scan = scans['region'] \
            .filter(lambda r: (r.r_name == 1)) \
            .map(lambda r: (r.r_regionkey))

        nation1 = scans['nation'] \
            .map(lambda r: (r.n_regionkey, r.n_nationkey)) \
            .join(region_scan) \
            .alias(['regionkey', 'n1_nationkey']) \
            .map(lambda r: (r.n1_nationkey)) \
            .join(part_scan) \
            .alias(['n1_nationkey', 'l_suppkey', 'l_extendedprice',
                    'l_discount', 'o_orderdate']) \
            .map(lambda r:
                 (r.l_suppkey, r.l_extendedprice, r.l_discount,
                  r.o_orderdate)) \
            .join(supplier_scan) \
            .alias(['l_suppkey', 'l_extendedprice', 'l_discount',
                    'o_orderdate', 's_nationkey']) \
            .map(lambda r:
                 (r.s_nationkey, r.l_extendedprice, r.l_discount,
                  r.o_orderdate)) \

        nation2 = scans['nation'] \
            .map(lambda r: (r.n_nationkey, r.n_name)) \
            .join(nation1) \
            .alias(['n_nationkey', 'n_name', 'l_extendedprice', 'l_discount',
                    'o_orderdate']) \
            .map(lambda r:
                 (extract_year(r.o_orderdate),
                  r.l_extendedprice * (100 - r.l_discount),
                  r.n_name),
                 names=['o_year', 'revenue', 'n_name']) \
            .map(lambda r:
                 (r.o_year, r.revenue, r.revenue if r.n_name == 2 else 0),
                 names=['o_year', 'total_revenue', 'revenue']) \
            .reduce_by_key(lambda r1, r2:
                           (r1.total_revenue + r2.total_revenue,
                            r1.revenue + r2.revenue)) \
            .map(lambda r:
                 (r.o_year, r.revenue / r.total_revenue),
                 names=['o_year', 'mkt_share']) \
            .sort()

        return nation2.collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)
        return df


class Q10(TpchJitqQuery):
    # select
    #     c_custkey,
    #     c_name,
    #     sum(l_extendedprice * (1 - l_discount)) as revenue,
    #     c_acctbal,
    #     n_name,
    #     c_address,
    #     c_phone,
    #     c_comment
    # from
    #     customer,
    #     orders,
    #     lineitem,
    #     nation
    # where
    #    c_custkey = o_custkey
    #     and l_orderkey = o_orderkey
    #     and o_orderdate >= date '1993-10-01'
    #     and o_orderdate < date '1994-01-01'
    #     and l_returnflag = 'R'
    #     and c_nationkey = n_nationkey
    # group by
    #     c_custkey,
    #     c_name,
    #     c_acctbal,
    #     c_phone,
    #     n_name,
    #     c_address,
    #     c_comment
    # order by
    #     revenue desc
    # limit 20

    def load(self, database):
        customer_scan = database.scan('customer', {
            'c_custkey': (0, nb.int64),
            'c_name': (1, nb.int64),
            'c_nationkey': (3, nb.int64),
            'c_phone': (4, nb.int64),
            'c_acctbal': (5, nb.int64),
        })
        nation_scan = database.scan('nation', {
            'n_nationkey': (0, nb.int64),
            'n_name': (1, nb.int64),
        })
        lineitem_scan = database.scan('lineitem', {
            'l_orderkey': (0, nb.int64),
            'l_extendedprice': (5, nb.int64),
            'l_discount': (6, nb.int64),
            'l_returnflag': (8, nb.int64),
        })
        orders_scan = database.scan('orders', {
            'o_orderkey': (0, nb.int64),
            'o_custkey': (1, nb.int64),
            'o_orderdate': (4, nb.int64),
        })

        return {
            'customer': customer_scan,
            'nation': nation_scan,
            'lineitem': lineitem_scan,
            'orders': orders_scan,
        }

    def run(self, scans):
        lb_orderdate = parse_date('1993-10-01')
        hb_orderdate = parse_date('1994-01-01')

        lineitem_scan = scans['lineitem'] \
            .filter(lambda x: x.l_returnflag == 2) \
            .map(lambda x: (x.l_orderkey, x.l_extendedprice, x.l_discount))

        customer_scan = scans['customer'] \
            .map(lambda x:
                 (x.c_custkey, x.c_acctbal, x.c_nationkey, x.c_phone,
                  x.c_name))

        orders_scan = scans['orders'] \
            .filter(lambda r: lb_orderdate <= r.o_orderdate < hb_orderdate) \
            .map(lambda r: (r.o_custkey, r.o_orderkey)) \
            .join(customer_scan) \
            .alias(['c_custkey', 'o_orderkey', 'c_acctbal', 'c_nationkey',
                    'c_phone', 'c_name']) \
            .map(lambda r:
                 (r.c_nationkey, r.o_orderkey, r.c_custkey, r.c_acctbal,
                  r.c_phone, r.c_name))

        nation_scan = scans['nation'] \
            .map(lambda r: (r.n_nationkey, r.n_name)) \
            .join(orders_scan) \
            .alias(['o_nationkey', 'n_name', 'o_orderkey', 'c_custkey',
                    'c_acctbal', 'c_phone', 'c_name']) \
            .map(lambda r:
                 (r.o_orderkey, r.c_custkey, r.c_acctbal, r.c_phone, r.c_name,
                  r.n_name)) \
            .join(lineitem_scan) \
            .alias(['o_orderkey', 'c_custkey', 'c_acctbal', 'c_phone',
                    'c_name', 'n_name', 'l_extendedprice', 'l_discount']) \
            .map(lambda r:
                 (r.c_custkey,
                  r.l_extendedprice * (100 - r.l_discount),
                  r.c_acctbal,
                  r.c_phone,
                  r.c_name,
                  r.n_name),
                 names=['c_custkey', 'revenue', 'c_acctbal', 'c_phone',
                        'c_name', 'n_name']) \
            .reduce_by_key(lambda r1, r2:
                           (r1.revenue + r2.revenue,
                            r2.c_acctbal,
                            r1.c_phone,
                            r1.c_name,
                            r1.n_name)) \
            .map(lambda r:
                 (r.revenue, r.c_custkey, r.c_acctbal, r.c_phone, r.c_name,
                  r.n_name)) \
            .topk(20) \
            .map(lambda t:
                 (t[1], t[0], t[2], t[3], t[4], t[5]),
                 names=['c_custkey', 'revenue', 'c_acctbal', 'c_phone',
                        'c_name', 'n_name'])

        return nation_scan.collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        df.revenue = df.revenue.apply(Decimal) / 100 / 100
        df.c_acctbal = df.c_acctbal.apply(Decimal) / 100
        nationdict = sorted(PandasCsv.DICTS['n_name'])
        df.n_name = df.n_name.apply(lambda i: (nationdict[i]))
        df.c_name = df.c_name.apply('Customer#{:09}'.format)

        return df


class Q11(TpchJitqQuery):
    # select
    #     ps_partkey,
    #     sum(ps_supplycost * ps_availqty) as "value"
    # from
    #     partsupp,
    #     supplier,
    #     nation
    # where
    #    ps_suppkey = s_suppkey
    #     and s_nationkey = n_nationkey
    #     and n_name = 'GERMANY'
    # group by
    #     ps_partkey having
    #         sum(ps_supplycost * ps_availqty) > (
    #             select
    #                 sum(ps_supplycost * ps_availqty) * 0.0001
    #             from
    #                 partsupp,
    #                 supplier,
    #                 nation
    #             where
    #                 ps_suppkey = s_suppkey
    #                 and s_nationkey = n_nationkey
    #                 and n_name = 'GERMANY'
    #         )
    # order by
    #     "value" desc

    def load(self, database):
        nation_scan = database.scan('nation', {
            'n_nationkey': (0, nb.int64),
            'n_name': (1, nb.int64),
        })
        partsupp_scan = database.scan('partsupp', {
            'ps_partkey': (0, nb.int64),
            'ps_suppkey': (1, nb.int64),
            'ps_availqty': (2, nb.int64),
            'ps_supplycost': (3, nb.int64),
        })
        supplier_scan = database.scan('supplier', {
            's_suppkey': (0, nb.int64),
            's_nationkey': (3, nb.int64),
        })

        return {
            'nation': nation_scan,
            'partsupp': partsupp_scan,
            'supplier': supplier_scan,
        }

    def run(self, scans):
        supplier_scan = scans['supplier'] \
            .map(lambda x: (x.s_nationkey, x.s_suppkey))

        partsupp_scan = scans['partsupp'] \
            .map(lambda x:
                 (x.ps_suppkey, x.ps_partkey, x.ps_supplycost * x.ps_availqty))

        nation_scan = scans['nation'] \
            .filter(lambda r: r.n_name == 8) \
            .map(lambda r: r.n_nationkey) \
            .join(supplier_scan) \
            .alias(['nationkey', 's_suppkey']) \
            .map(lambda r: r.s_suppkey) \
            .join(partsupp_scan) \
            .alias(['ps_suppkey', 'ps_partkey', 'value'])

        total_value = nation_scan \
            .map(lambda r: r.value) \
            .reduce(lambda i1, i2: i1 + i2)

        # Total value is None if nation_scan empty. Setting it to 0 allows
        # the correct execution of the query.
        if total_value is None:
            total_value = 0

        result = nation_scan \
            .map(lambda r: (r.ps_partkey, r.value)) \
            .reduce_by_key(lambda i1, i2: i1 + i2) \
            .alias(['ps_partkey', 'value']) \
            .filter(lambda r: (r.value * 10000) > total_value) \
            .map(lambda r: (-r.value, r.ps_partkey)) \
            .sort() \
            .map(lambda r: (r[1], -r[0]),
                 names=['ps_partkey', 'value'])

        return result.collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        df.value = df.value.apply(Decimal) / 100

        return df


class Q12(TpchJitqQuery):
        # select
        #     l_shipmode,
        #     sum(case
        #         when o_orderpriority = '1-URGENT'
        #             or o_orderpriority = '2-HIGH'
        #             then 1
        #         else 0
        #     end) as high_line_count,
        #     sum(case
        #         when o_orderpriority <> '1-URGENT'
        #             and o_orderpriority <> '2-HIGH'
        #             then 1
        #         else 0
        #     end) as low_line_count
        # from
        #     orders,
        #     lineitem
        # where
        #     o_orderkey = l_orderkey
        #     and l_shipmode in ('MAIL', 'SHIP')
        #     and l_commitdate < l_receiptdate
        #     and l_shipdate < l_commitdate
        #     and l_receiptdate >= date '1994-01-01'
        #     and l_receiptdate < date '1995-01-01'
        # group by
        #     l_shipmode
        # order by
        #     l_shipmode

    def load(self, database):
        lineitem_scan = database.scan('lineitem', {
            'l_orderkey': (0, nb.int64),
            'l_shipdate': (10, nb.int64),
            'l_commitdate': (11, nb.int64),
            'l_receiptdate': (12, nb.int64),
            'l_shipmode': (14, nb.int64),
        })
        orders_scan = database.scan('orders', {
            'o_orderkey': (0, nb.int64),
            'o_orderpriority': (5, nb.int64),
        })

        return {
            'lineitem': lineitem_scan,
            'orders': orders_scan,
        }

    def run(self, scans):
        lb_receiptdate = parse_date('1994-01-01')
        hb_receiptdate = parse_date('1995-01-01')

        join = scans['lineitem'] \
            .filter(lambda r:
                    r.l_shipmode in [2, 5] and
                    r.l_shipdate < r.l_commitdate < r.l_receiptdate and
                    lb_receiptdate <= r.l_receiptdate < hb_receiptdate) \
            .map(lambda r: (r.l_orderkey, r.l_shipmode)) \
            .join(scans['orders']
                  .map(lambda r: (r.o_orderkey, r.o_orderpriority))) \
            .alias(['l_orderkey', 'l_shipmode', 'o_orderpriority'])

        return join \
            .map(lambda t:
                 (t.l_shipmode,
                  1 if t.o_orderpriority in [1, 2] else 0,
                  0 if t.o_orderpriority in [1, 2] else 1),
                 ['l_shipmode', 'high_line_count', 'low_line_count']) \
            .reduce_by_key(lambda r1, r2:
                           (r1.high_line_count + r2.high_line_count,
                            r1.low_line_count + r2.low_line_count)) \
            .collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        shipmodes = {2: 'MAIL', 5: 'SHIP'}
        df.l_shipmode = df.l_shipmode.apply(lambda i: shipmodes[i])

        return df.sort_values(by=['l_shipmode'])


class Q14(TpchJitqQuery):
    # select
    #     100.00 * sum(case
    #         when p_type like 'PROMO%'
    #         then l_extendedprice * (1 - l_discount)
    #         else 0
    #     end) / sum(l_extendedprice * (1 - l_discount)) as promo_revenue
    # from
    #     lineitem,
    #     part
    # where
    #     l_partkey = p_partkey
    #     and l_shipdate >= date '1995-09-01'
    #     and l_shipdate < date '1995-10-01'

    def load(self, database):
        lineitem_scan = database.scan('lineitem', {
            'l_partkey': (1, nb.int64),
            'l_extendedprice': (5, nb.int64),
            'l_discount': (6, nb.int64),
            'l_shipdate': (10, nb.int64),
        })
        part_scan = database.scan('part', {
            'p_partkey': (0, nb.int64),
            'p_type': (4, nb.int64),
        })

        return {
            'lineitem': lineitem_scan,
            'part': part_scan,
        }

    def run(self, scans):
        lb_shipdate = parse_date('1995-09-01')
        hb_shipdate = parse_date('1995-10-01')

        # like 'PROMO%' matches dictionary codes [75, 99]

        join = scans['lineitem'] \
            .filter(lambda r: lb_shipdate <= r.l_shipdate < hb_shipdate) \
            .map(lambda r: (r.l_partkey,
                            r.l_extendedprice * (100 - r.l_discount))) \
            .join(scans['part']
                  .map(lambda r: (r.p_partkey, r.p_type))) \
            .alias(['l_partkey', 'revenue', 'p_type'])

        return join \
            .map(lambda r:
                 (r.revenue,
                  r.revenue if 75 <= r.p_type <= 99 else 0),
                 ['revenue', 'promo_revenue']) \
            .reduce(lambda r1, r2: (r1.revenue + r2.revenue,
                                    r1.promo_revenue + r2.promo_revenue))

    def postprocess(self, res):
        res = Decimal(res[1]) / Decimal(res[0]) * 100
        return pd.DataFrame([res], columns=['promo_revenue'])


class Q15(TpchJitqQuery):
    # with revenue as (
    #     select
    #         l_suppkey as supplier_no,
    #         sum(l_extendedprice * (1 - l_discount)) as total_revenue
    #     from
    #         lineitem
    #     where
    #         l_shipdate >= date '1996-01-01'
    #         and l_shipdate < date '1996-04-01'
    #     group by
    #         l_suppkey)
    # select
    #     s_suppkey,
    #     s_name,
    #     s_address,
    #     s_phone,
    #     total_revenue
    # from
    #     supplier,
    #     revenue
    # where
    #     s_suppkey = supplier_no and
    #     total_revenue = (
    #         select
    #             max(total_revenue)
    #         from
    #             revenue
    #     )
    # order by
    #     s_suppkey

    def load(self, database):
        lineitem_scan = database.scan('lineitem', {
            'l_suppkey': (2, nb.int64),
            'l_extendedprice': (5, nb.int64),
            'l_discount': (6, nb.int64),
            'l_shipdate': (10, nb.int64),
        })
        supplier_scan = database.scan('supplier', {
            's_suppkey': (0, nb.int64),
            's_name': (1, nb.int64),
            's_phone': (4, nb.int64),
        })

        return {
            'lineitem': lineitem_scan,
            'supplier': supplier_scan,
        }

    def run(self, scans):
        lb_shipdate = parse_date('1996-01-01')
        hb_shipdate = parse_date('1996-04-01')

        revenue = scans['lineitem'] \
            .filter(lambda r: (lb_shipdate <= r.l_shipdate < hb_shipdate)) \
            .map(lambda r:
                 (r.l_suppkey, r.l_extendedprice * (100 - r.l_discount)),
                 names=['l_suppkey', 'total_revenue']) \
            .reduce_by_key(lambda i1, i2: i1 + i2)

        max_rev = revenue \
            .map(lambda r: r.total_revenue) \
            .reduce(lambda i1, i2: i1 if i1 > i2 else i2)

        supplier_scan = scans['supplier'] \
            .map(lambda r: (r.s_suppkey, r.s_name, r.s_phone))

        max_rev_info = revenue \
            .filter(lambda r: r.total_revenue == max_rev) \
            .map(lambda r: (r.l_suppkey, r.total_revenue)) \
            .join(supplier_scan) \
            .alias(['l_suppkey', 'total_revenue', 's_name', 's_phone']) \
            .sort() \
            .map(lambda r:
                 (r.l_suppkey, r.s_name, r.s_phone, r.total_revenue),
                 names=['supplier_no', 's_name', 's_phone', 'total_revenue'])

        return max_rev_info.collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        df.total_revenue = df.total_revenue.apply(Decimal) / 100 / 100
        df.s_name = df.s_name.apply('Supplier#{:09}'.format)

        return df.reset_index(drop=True)


class Q17(TpchJitqQuery):
    # select
    #     sum(l_extendedprice) / 7.0 as avg_yearly
    # from
    #     lineitem,
    #     part
    # where
    #     p_partkey = l_partkey
    #     and p_brand = 'Brand#23'
    #     and p_container = 'MED BOX'
    #     and l_quantity < (
    #         select
    #             0.2 * avg(l_quantity)
    #         from
    #             lineitem
    #         where
    #             l_partkey = p_partkey
    #     );

    def load(self, database):
        lineitem_scan = database.scan('lineitem', {
            'l_partkey': (1, nb.int64),
            'l_quantity': (4, nb.int64),
            'l_extendedprice': (5, nb.int64),
        })
        part_scan = database.scan('part', {
            'p_partkey': (0, nb.int64),
            'p_brand': (3, nb.int64),
            'p_container': (6, nb.int64),
        })

        return {
            'lineitem': lineitem_scan,
            'part': part_scan,
        }

    def run(self, scans):
        # MED BOX has dictionary value 25

        part_scan = scans['part'] \
            .filter(lambda r:
                    r.p_brand == 23 and
                    r.p_container == 17) \
            .map(lambda r: (r.p_partkey,))

        lineitem_scan = scans['lineitem'] \
            .map(lambda r:
                 (r.l_partkey, r.l_extendedprice, r.l_quantity))

        # Result fields: p_partkey (l_extendedprice[31-6]|l_quantity[5-0])
        interesting_lineitems = part_scan \
            .join(lineitem_scan) \
            .alias(['l_partkey', 'l_extendedprice', 'l_quantity']) \
            .map(lambda r:
                 (r.l_partkey,
                  (r.l_extendedprice << 6) +
                  r.l_quantity),
                 names=['l_partkey', 'extendedprice_quantity'])

        return interesting_lineitems \
            .map(lambda r:
                 (r.l_partkey, r.extendedprice_quantity & 63, 1),
                 names=['l_partkey', 'l_quantity', 'count']) \
            .map(lambda r:
                 (r.l_partkey, (r.l_quantity << 32) + 1),
                 names=['l_partkey', 'quantity_count']) \
            .reduce_by_key(lambda i1, i2: i1 + i2) \
            .map(lambda r:
                 (r.l_partkey,
                  r.quantity_count >> 32,
                  r.quantity_count & ((1 << 32) - 1)),
                 names=['l_partkey', 'l_quantity', 'count']) \
            .map(lambda r: (r.l_partkey, r.l_quantity / r.count)) \
            .join(interesting_lineitems
                  .map(lambda r: (r.l_partkey, r.extendedprice_quantity))) \
            .alias(['l_partkey', 'avg_qty', 'extendedprice_quantity']) \
            .map(lambda r:
                 (r.avg_qty,
                  r.extendedprice_quantity & 63,
                  r.extendedprice_quantity >> 6),
                 names=['avg_qty', 'l_quantity', 'l_extendedprice']) \
            .filter(lambda r: r.l_quantity < 0.2 * r.avg_qty) \
            .map(lambda r: r.l_extendedprice) \
            .reduce(lambda i1, i2: i1 + i2)

    def postprocess(self, res):
        res = Decimal(res) / 7 / 100
        return pd.DataFrame([res], columns=['avg_yearly'])


class Q18(TpchJitqQuery):
    # select
    #     c_name,
    #     c_custkey,
    #     o_orderkey,
    #     o_orderdate,
    #     o_totalprice,
    #     sum(l_quantity)
    # from
    #     customer,
    #     orders,
    #     lineitem
    # where
    #     o_orderkey in (
    #         select
    #             l_orderkey
    #         from
    #             lineitem
    #         group by
    #             l_orderkey
    #         having
    #             sum(l_quantity) > 300
    #     )
    #     and c_custkey = o_custkey
    #     and o_orderkey = l_orderkey
    # group by
    #     c_name,
    #     c_custkey,
    #     o_orderkey,
    #     o_orderdate,
    #     o_totalprice
    # order by
    #     o_totalprice desc,
    #     o_orderdate
    # limit 100

    def load(self, database):
        customer_scan = database.scan('customer', {
            'c_custkey': (0, nb.int64),
            'c_name': (1, nb.int64),
        })
        lineitem_scan = database.scan('lineitem', {
            'l_orderkey': (0, nb.int64),
            'l_quantity': (4, nb.int64),
        })
        orders_scan = database.scan('orders', {
            'o_orderkey': (0, nb.int64),
            'o_custkey': (1, nb.int64),
            'o_totalprice': (3, nb.int64),
            'o_orderdate': (4, nb.int64),
        })

        return {
            'customer': customer_scan,
            'lineitem': lineitem_scan,
            'orders': orders_scan,
        }

    def run(self, scans):
        customer = scans['customer']
        lineitem = scans['lineitem']
        orders = scans['orders']

        orders_scan = orders \
            .map(lambda r:
                 (r.o_orderkey, r.o_orderdate, r.o_totalprice, r.o_custkey))

        lineitem_scan = lineitem \
            .map(lambda r: (r.l_orderkey, np.int32(r.l_quantity)),
                 names=['l_orderkey', 'l_quantity']) \
            .reduce_by_key(lambda i1, i2: (np.int32(i1 + i2))) \
            .filter(lambda r: r.l_quantity > 300) \
            .map(lambda r: (r.l_orderkey, r.l_quantity)) \
            .join(orders_scan) \
            .alias(['l_orderkey', 'l_quantity', 'o_orderdate', 'o_totalprice',
                    'o_custkey']) \
            .map(lambda r:
                 (r.o_custkey, r.l_orderkey, r.o_orderdate, r.o_totalprice,
                  r.l_quantity))

        customer_scan = customer \
            .map(lambda r: (r.c_custkey, r.c_name)) \
            .join(lineitem_scan) \
            .alias(['c_custkey', 'c_name', 'o_orderkey', 'o_orderdate',
                    'o_totalprice', 'l_quantity']) \
            .map(lambda r:
                 ((r.c_custkey << 32) + r.o_orderkey,
                  r.o_orderdate,
                  r.o_totalprice,
                  np.int64(r.l_quantity),
                  r.c_name),
                 names=['custkey_orderkey', 'o_orderdate', 'o_totalprice',
                        'l_quantity', 'c_name']) \
            .reduce_by_key(lambda r1, r2:
                           (r1.o_orderdate,
                            r1.o_totalprice,
                            r1.l_quantity + r2.l_quantity,
                            r1.c_name)) \
            .map(lambda r:
                 (r.custkey_orderkey >> 32,
                  r.custkey_orderkey & ((1 << 32) - 1),
                  r.o_orderdate,
                  r.o_totalprice,
                  r.l_quantity,
                  r.c_name),
                 names=['c_custkey', 'o_orderkey', 'o_orderdate',
                        'o_totalprice', 'total_quantity', 'c_name']) \
            .map(lambda r:
                 ((r.o_totalprice << 32) + r.o_orderdate,
                  r.o_orderkey,
                  r.c_custkey,
                  r.total_quantity,
                  r.c_name),
                 names=['totalprice_orderdate', 'o_orderkey', 'c_custkey',
                        'total_quantity', 'c_name']) \
            .topk(100) \
            .map(lambda r:
                 (r.c_name,
                  r.c_custkey,
                  r.o_orderkey,
                  r.totalprice_orderdate & ((1 << 32) - 1),
                  r.totalprice_orderdate >> 32,
                  r.total_quantity),
                 names=['c_name', 'c_custkey', 'o_orderkey', 'o_orderdate',
                        'o_totalprice', 'total_quantity'])

        return customer_scan.collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        df.o_orderdate = df.o_orderdate.apply(dt.date.fromtimestamp)
        df.o_totalprice = df.o_totalprice.apply(Decimal) / 100

        return df


class Q19(TpchJitqQuery):
    # select
    #     sum(l_extendedprice* (1 - l_discount)) as revenue
    # from
    #     lineitem,
    #     part
    # where (
    #         p_partkey = l_partkey
    #         and p_brand = 'Brand#12'
    #         and p_container in ('SM CASE', 'SM BOX', 'SM PACK', 'SM PKG')
    #         and p_size between 1 and 5
    #         and l_quantity >= 1 and l_quantity <= 1 + 10
    #         and l_shipmode in ('AIR', 'REG AIR')
    #         and l_shipinstruct = 'DELIVER IN PERSON'
    #     )
    #     or (
    #         p_partkey = l_partkey
    #         and p_brand = 'Brand#23'
    #         and p_container in ('MED BAG','MED BOX','MED PKG','MED PACK')
    #         and p_size between 1 and 10
    #         and l_quantity >= 10 and l_quantity <= 10 + 10
    #         and l_shipmode in ('AIR', 'REG AIR')
    #         and l_shipinstruct = 'DELIVER IN PERSON'
    #     )
    #     or (
    #         p_partkey = l_partkey
    #         and p_brand = 'Brand#34'
    #         and p_container in ('LG CASE', 'LG BOX', 'LG PACK', 'LG PKG')
    #         and p_size between 1 and 15
    #         and l_quantity >= 20 and l_quantity <= 20 + 10
    #         and l_shipmode in ('AIR', 'REG AIR')
    #         and l_shipinstruct = 'DELIVER IN PERSON'
    #     )

    def load(self, database):
        lineitem_scan = database.scan('lineitem', {
            'l_orderkey': (0, nb.int64),
            'l_partkey': (1, nb.int64),
            'l_linenumber': (3, nb.int64),
            'l_quantity': (4, nb.int64),
            'l_extendedprice': (5, nb.int64),
            'l_discount': (6, nb.int64),
            'l_shipinstruct': (13, nb.int64),
            'l_shipmode': (14, nb.int64),
        })
        part_scan = database.scan('part', {
            'p_partkey': (0, nb.int64),
            'p_brand': (3, nb.int64),
            'p_size': (5, nb.int64),
            'p_container': (6, nb.int64),
        })

        return {
            'lineitem': lineitem_scan,
            'part': part_scan,
        }

    def run(self, scans):
        lineitem_scan = scans['lineitem'] \
            .filter(lambda r:
                    (r.l_shipmode == 0 or r.l_shipmode == 4) and
                    r.l_shipinstruct == 1 and
                    1 <= r.l_quantity <= 30) \
            .map(lambda r:
                 (r.l_partkey,
                  np.int64(r.l_extendedprice * (100 - r.l_discount)),
                  (4 if (1 <= r.l_quantity <= 11) else 0) +
                  (2 if (10 <= r.l_quantity <= 20) else 0) +
                  (1 if (20 <= r.l_quantity <= 30) else 0)),
                 ['l_partkey', 'revenue', 'group']) \
            .map(lambda r: (r.l_partkey, (r.revenue << 3) + r.group),
                 ['l_partkey', 'revenue_group'])

        part_scan = scans['part'] \
            .filter(lambda r:
                    r.p_brand in [12, 23, 34] and
                    1 <= r.p_size <= 15 and
                    9 <= r.p_container <= 31) \
            .map(lambda r:
                 (r.p_partkey,
                  (4 if r.p_brand == 12 and (1 <= r.p_size <= 5) and
                      np.int64(r.p_container) in [25, 27, 30, 31] else 0) +
                  #   ^^^^^^^^ XXX: this cast is a work-around for #76
                  (2 if r.p_brand == 23 and (1 <= r.p_size <= 10) and
                      np.int64(r.p_container) in [16, 17, 22, 23] else 0) +
                  (1 if r.p_brand == 34 and (1 <= r.p_size <= 15) and
                      np.int64(r.p_container) in [9, 11, 14, 15] else 0)),
                 ['p_partkey', 'group']) \
            .filter(lambda r: r.group != 0)

        # Result of join: (partkey, revenue | part_group, part_group)
        return lineitem_scan \
            .map(lambda r: (r.l_partkey, r.revenue_group)) \
            .join(part_scan.map(lambda r: (r.p_partkey, r.group))) \
            .alias(['l_partkey', 'revenue_lgroup', 'pgroup']) \
            .filter(lambda r: r.revenue_lgroup & r.pgroup != 0) \
            .map(lambda r: r.revenue_lgroup >> 3) \
            .reduce(lambda i1, i2: i1 + i2)

    def postprocess(self, res):
        res = Decimal(res) / 100 / 100
        return pd.DataFrame([res], columns=['revenue'])


class Q21(TpchJitqQuery):
    # select
    #     s_name,
    #     count(*) as numwait
    # from
    #     supplier,
    #     lineitem l1,
    #     orders,
    #     nation
    # where
    #     s_suppkey = l1.l_suppkey
    #     and o_orderkey = l1.l_orderkey
    #     and o_orderstatus = 'F'
    #     and l1.l_receiptdate > l1.l_commitdate
    #     and exists (
    #         select
    #             *
    #         from
    #             lineitem l2
    #         where
    #             l2.l_orderkey = l1.l_orderkey
    #             and l2.l_suppkey <> l1.l_suppkey
    #     )
    #     and not exists (
    #         select
    #             *
    #         from
    #             lineitem l3
    #         where
    #             l3.l_orderkey = l1.l_orderkey
    #             and l3.l_suppkey <> l1.l_suppkey
    #             and l3.l_receiptdate > l3.l_commitdate
    #     )
    #     and s_nationkey = n_nationkey
    #     and n_name = 'SAUDI ARABIA'
    # group by
    #     s_name
    # order by
    #     numwait desc,
    #     s_name
    # limit 100

    def load(self, database):
        lineitem_scan = database.scan('lineitem', {
            'l_orderkey': (0, nb.int64),
            'l_suppkey': (2, nb.int64),
            'l_commitdate': (11, nb.int64),
            'l_receiptdate': (12, nb.int64),
        })
        nation_scan = database.scan('nation', {
            'n_nationkey': (0, nb.int64),
            'n_name': (1, nb.int64),
        })
        orders_scan = database.scan('orders', {
            'o_orderkey': (0, nb.int64),
            'o_orderstatus': (2, nb.int64),
        })
        supplier_scan = database.scan('supplier', {
            's_suppkey': (0, nb.int64),
            's_name': (1, nb.int64),
            's_nationkey': (3, nb.int64),
        })

        return {
            'lineitem': lineitem_scan,
            'nation': nation_scan,
            'orders': orders_scan,
            'supplier': supplier_scan,
        }

    def run(self, scans):
        supplier_scan = scans['supplier'] \
            .map(lambda r: (r.s_nationkey, r.s_suppkey, r.s_name))

        lineitem1_scan = scans['lineitem'] \
            .filter(lambda r: r.l_receiptdate > r.l_commitdate) \
            .map(lambda r: (r.l_suppkey, r.l_orderkey))

        orders_scan = scans['orders'] \
            .filter(lambda r: r.o_orderstatus == 0) \
            .map(lambda r: (r.o_orderkey))

        lineitem2_scan = scans['lineitem'] \
            .map(lambda r: (r.l_orderkey, r.l_suppkey))

        nation_scan = scans['nation'] \
            .filter(lambda r: r.n_name == 21) \
            .map(lambda r: r.n_nationkey) \
            .join(supplier_scan) \
            .alias(['s_nationkey', 's_suppkey', 's_name']) \
            .map(lambda r: (r.s_suppkey, r.s_name)) \
            .join(lineitem1_scan) \
            .alias(['l1_suppkey', 's_name', 'l1_orderkey']) \
            .map(lambda r: (r.l1_orderkey, r.l1_suppkey, r.s_name)) \
            .join(orders_scan) \
            .alias(['o_orderkey', 'l1_suppkey', 's_name']) \
            .map(lambda r: (r.o_orderkey, r.l1_suppkey, r.s_name))

        scan = nation_scan \
            .antijoin(
                lineitem1_scan
                .alias(['l_suppkey', 'l_orderkey'])
                .map(lambda r: (r.l_orderkey, r.l_suppkey)),
                lambda r1, r2: r1[1] != r2[1]) \
            .alias(['l_orderkey', 'l1_suppkey', 's_name']) \
            .map(lambda r: (r.l_orderkey, r.l1_suppkey, r.s_name))

        semi = scan \
            .join(lineitem2_scan) \
            .alias(['l_orderkey', 'l1_suppkey', 's_name', 'l2_suppkey']) \
            .filter(lambda r: (r.l1_suppkey != r.l2_suppkey)) \
            .map(lambda r: r.l_orderkey)

        scan2 = scan \
            .semijoin(semi) \
            .alias(['l_orderkey', 'l1_suppkey', 's_name']) \
            .map(lambda r: (r.s_name, 1), names=['s_name', 'numwait']) \
            .reduce_by_key(lambda i1, i2: i1 + i2) \
            .map(lambda r:
                 (r.numwait << 32) |
                 (~r.s_name & ((1 << 32) - 1)),
                 names=['numwait_name']) \
            .topk(100) \
            .map(lambda r:
                 (~r.numwait_name & ((1 << 32) - 1),
                  r.numwait_name >> 32),
                 names=['s_name', 'numwait'])

        return scan2.collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        df.s_name = df.s_name.apply('Supplier#{:09}'.format)

        return df


class Q22(TpchJitqQuery):

    # select
    #     cntrycode,
    #     count(*) as numcust,
    #     sum(c_acctbal) as totacctbal
    # from (
    #     select
    #         substring(c_phone from 1 for 2) as cntrycode,
    #         c_acctbal
    #     from
    #         customer
    #     where
    #         substring(c_phone from 1 for 2) in
    #             ('13', '31', '23', '29', '30', '18', '17')
    #         and c_acctbal > (
    #             select
    #                 avg(c_acctbal)
    #             from
    #                 customer
    #             where
    #                 c_acctbal > 0.00
    #                 and substring(c_phone from 1 for 2) in
    #                     ('13', '31', '23', '29', '30', '18', '17')
    #         )
    #         and not exists (
    #             select
    #                 *
    #             from
    #                 orders
    #             where
    #                 o_custkey = c_custkey
    #         )
    #     ) as custsale
    # group by
    #     cntrycode
    # order by
    #     cntrycode

    def load(self, database):
        customer_scan = database.scan('customer', {
            'c_custkey': (0, nb.int64),
            'c_phone': (4, nb.int64),
            'c_acctbal': (5, nb.int64),
        })
        orders_scan = database.scan('orders', {
            'o_custkey': (1, nb.int64),
        })

        return {
            'customer': customer_scan,
            'orders': orders_scan,
        }

    def run(self, scans):
        orders_scan = scans['orders'] \
            .map(lambda x: x.o_custkey)

        customer_scan = scans['customer'] \
            .map(lambda x:
                 (x.c_phone // pow(10, 10), x.c_acctbal, x.c_custkey),
                 names=['cntrycode', 'c_acctbal', 'c_custkey']) \
            .filter(lambda x: x.cntrycode in [13, 17, 18, 23, 29, 30, 31])

        acctbal_sum, acctbal_count = customer_scan \
            .filter(lambda x: x.c_acctbal > 0) \
            .map(lambda x: (np.int64(x.c_acctbal), 1),
                 names=['c_acctbal', 'count']) \
            .reduce(lambda x1, x2:
                    (x1.c_acctbal + x2.c_acctbal, x1.count + x2.count))

        customer_scan2 = customer_scan \
            .filter(lambda x: x.c_acctbal * acctbal_count > acctbal_sum) \
            .map(lambda x: (x.c_custkey, x.cntrycode, x.c_acctbal)) \
            .antijoin(orders_scan) \
            .alias(['custkey', 'cntrycode', 'c_acctbal']) \
            .map(lambda x: (x.cntrycode, 1, np.int64(x.c_acctbal)),
                 names=['cntrycode', 'numcust', 'totacctbal']) \
            .reduce_by_key(lambda x1, x2:
                           (x1.numcust + x2.numcust,
                            x1.totacctbal + x2.totacctbal)) \
            .sort()

        return customer_scan2.collect()

    def postprocess(self, res):
        df = pd.DataFrame(res)

        df.totacctbal = df.totacctbal.apply(Decimal) / 100

        return df
