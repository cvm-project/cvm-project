#!/usr/bin/env python3

from abc import ABC, abstractmethod
from decimal import Decimal

import numpy as np
import pandas as pd

from jitq.tests.tpch.pandas import parse_date


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
        lineitem_scan = database.scan('lineitem', [
            'l_shipdate', 'l_discount', 'l_extendedprice', 'l_tax',
            'l_quantity', 'l_returnflag', 'l_linestatus',
        ])

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
        lineitem_scan = database.scan('lineitem', [
            'l_orderkey', 'l_commitdate', 'l_receiptdate',
        ])
        orders_scan = database.scan('orders', [
            'o_orderkey', 'o_orderpriority', 'o_orderdate',
        ])

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
            .map(lambda r: (r.l_orderkey, np.int32(1)),
                 names=['l_orderkey', 'dummy_for_distinct']) \
            .reduce_by_key(lambda i1, i2: np.int32(i1 + i2)) \
            .map(lambda t: t.l_orderkey)

        return orders_scan \
            .join(lineitem_scan) \
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
        lineitem_scan = database.scan('lineitem', [
            'l_shipdate', 'l_discount', 'l_quantity', 'l_extendedprice',
        ])

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


class Q12(TpchJitqQuery):
        # select
        #         l_shipmode,
        #         sum(case
        #                 when o_orderpriority = '1-URGENT'
        #                         or o_orderpriority = '2-HIGH'
        #                         then 1
        #                 else 0
        #         end) as high_line_count,
        #         sum(case
        #                 when o_orderpriority <> '1-URGENT'
        #                         and o_orderpriority <> '2-HIGH'
        #                         then 1
        #                 else 0
        #         end) as low_line_count
        # from
        #         orders,
        #         lineitem
        # where
        #         o_orderkey = l_orderkey
        #         and l_shipmode in ('MAIL', 'SHIP')
        #         and l_commitdate < l_receiptdate
        #         and l_shipdate < l_commitdate
        #         and l_receiptdate >= date '1994-01-01'
        #         and l_receiptdate < date '1995-01-01'
        # group by
        #         l_shipmode
        # order by
        #         l_shipmode

    def load(self, database):
        lineitem_scan = database.scan('lineitem', [
            'l_commitdate', 'l_shipdate', 'l_receiptdate', 'l_shipmode',
            'l_orderkey',
        ])
        orders_scan = database.scan('orders', [
            'o_orderkey', 'o_orderpriority',
        ])

        return {'lineitem': lineitem_scan, 'orders': orders_scan}

    def run(self, scans):
        lineitem_scan = scans['lineitem']
        orders_scan = scans['orders']

        lb_receiptdate = parse_date('1994-01-01')
        hb_receiptdate = parse_date('1995-01-01')

        join = lineitem_scan \
            .filter(lambda r:
                    r.l_shipmode in [2, 5] and
                    r.l_shipdate < r.l_commitdate < r.l_receiptdate and
                    lb_receiptdate < r.l_receiptdate < hb_receiptdate) \
            .map(lambda r: (r.l_orderkey, r.l_shipmode)) \
            .join(orders_scan
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
    #         100.00 * sum(case
    #                 when p_type like 'PROMO%'
    #                         then l_extendedprice * (1 - l_discount)
    #                 else 0
    #         end) / sum(l_extendedprice * (1 - l_discount)) as promo_revenue
    # from
    #         lineitem,
    #         part
    # where
    #         l_partkey = p_partkey
    #         and l_shipdate >= date '1995-09-01'
    #         and l_shipdate < date '1995-10-01'

    def load(self, database):
        lineitem_scan = database.scan('lineitem', [
            'l_partkey', 'l_shipdate', 'l_discount', 'l_extendedprice',
        ])
        part_scan = database.scan('part', [
            'p_partkey', 'p_type',
        ])

        return {'lineitem': lineitem_scan, 'part': part_scan}

    def run(self, scans):
        lineitem_scan = scans['lineitem']
        part_scan = scans['part']

        lb_shipdate = parse_date('1995-09-01')
        hb_shipdate = parse_date('1995-10-01')

        # like 'PROMO%' matches dictionary codes [75, 99]

        join = lineitem_scan \
            .filter(lambda r: lb_shipdate <= r.l_shipdate < hb_shipdate) \
            .map(lambda r: (r.l_partkey,
                            r.l_extendedprice * (100 - r.l_discount))) \
            .join(part_scan.map(lambda r: (r.p_partkey, r.p_type))) \
            .alias(['l_partkey', 'discounted_price', 'p_type'])

        return join \
            .map(lambda r:
                 (r.discounted_price,
                  r.discounted_price if 75 <= r.p_type <= 99 else 0),
                 ['discounted_price', 'promo_revenue']) \
            .reduce(lambda r1, r2: (r1.discounted_price + r2.discounted_price,
                                    r1.promo_revenue + r2.promo_revenue))

    def postprocess(self, res):
        res = Decimal(res[1]) / Decimal(res[0]) * 100
        return pd.DataFrame([res], columns=['promo_revenue'])


class Q17(TpchJitqQuery):
    # select
    #         sum(l_extendedprice) / 7.0 as avg_yearly
    # from
    #         lineitem,
    #         part
    # where
    #         p_partkey = l_partkey
    #         and p_brand = 'Brand#23'
    #         and p_container = 'MED BOX'
    #         and l_quantity < (
    #                 select
    #                         0.2 * avg(l_quantity)
    #                 from
    #                         lineitem
    #                 where
    #                         l_partkey = p_partkey
    #         );

    def load(self, database):
        lineitem_scan = database.scan('lineitem', [
            'l_partkey', 'l_quantity', 'l_extendedprice',
        ])
        part_scan = database.scan('part', [
            'p_partkey', 'p_container', 'p_brand',
        ])

        return {'lineitem': lineitem_scan, 'part': part_scan}

    def run(self, scans):
        lineitem_scan = scans['lineitem']
        part_scan = scans['part']

        # MED BOX has dictionary value 25

        part_scan = part_scan \
            .filter(lambda r:
                    r.p_brand == 23 and
                    r.p_container == 17) \
            .map(lambda r: r.p_partkey)

        interesting_lineitems = part_scan \
            .join(lineitem_scan
                  .map(lambda r:
                       (r.l_partkey, r.l_extendedprice, r.l_quantity))) \
            .map(lambda t: (t[0], t[1], t[2])) \
            .alias(['p_partkey', 'l_extendedprice', 'l_quantity'])

        return interesting_lineitems \
            .map(lambda t: (t.p_partkey, np.int64(t.l_quantity), 1),
                 ['p_partkey', 'l_quantity', 'count']) \
            .reduce_by_key(lambda t1, t2: (t1.l_quantity + t2.l_quantity,
                                           t1.count + t2.count)) \
            .map(lambda t: (t.p_partkey, t.l_quantity / t.count)) \
            .join(interesting_lineitems
                  .map(lambda r:
                       (r.p_partkey, r.l_extendedprice, r.l_quantity))) \
            .alias(['p_partkey', 'avg_qty',
                    'l_extendedprice', 'l_quantity']) \
            .filter(lambda r: r.l_quantity < 0.2 * r.avg_qty) \
            .map(lambda r: np.int64(r.l_extendedprice)) \
            .reduce(lambda i1, i2: i1 + i2)

    def postprocess(self, res):
        res = Decimal(res) / 7 / 100
        return pd.DataFrame([res], columns=['avg_yearly'])


class Q19(TpchJitqQuery):
    # select
    #     sum(l_extendedprice* (1 - l_discount)) as revenue
    # from
    #     lineitem,
    #     part
    # where
    #     (
    #             p_partkey = l_partkey
    #             and p_brand = 'Brand#12'
    #             and p_container in ('SM CASE', 'SM BOX', 'SM PACK', 'SM PKG')
    #             and p_size between 1 and 5
    #             and l_quantity >= 1 and l_quantity <= 1 + 10
    #             and l_shipmode in ('AIR', 'REG AIR')
    #             and l_shipinstruct = 'DELIVER IN PERSON'
    #     )
    #     or
    #     (
    #             p_partkey = l_partkey
    #             and p_brand = 'Brand#23'
    #             and p_container in ('MED BAG','MED BOX','MED PKG','MED PACK')
    #             and p_size between 1 and 10
    #             and l_quantity >= 10 and l_quantity <= 10 + 10
    #             and l_shipmode in ('AIR', 'REG AIR')
    #             and l_shipinstruct = 'DELIVER IN PERSON'
    #     )
    #     or
    #     (
    #             p_partkey = l_partkey
    #             and p_brand = 'Brand#34'
    #             and p_container in ('LG CASE', 'LG BOX', 'LG PACK', 'LG PKG')
    #             and p_size between 1 and 15
    #             and l_quantity >= 20 and l_quantity <= 20 + 10
    #             and l_shipmode in ('AIR', 'REG AIR')
    #             and l_shipinstruct = 'DELIVER IN PERSON'
    #     )

    def load(self, database):
        lineitem_scan = database.scan('lineitem', [
            'l_orderkey', 'l_linenumber',
            'l_partkey', 'l_quantity', 'l_extendedprice',
            'l_discount', 'l_shipinstruct', 'l_shipmode'
        ])
        part_scan = database.scan('part', [
            'p_partkey', 'p_brand', 'p_size', 'p_container'
        ])

        return {'lineitem': lineitem_scan, 'part': part_scan}

    def run(self, scans):
        lineitem_scan = scans['lineitem']
        part_scan = scans['part']

        lineitem_scan = lineitem_scan \
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
                 ['l_partkey', 'discounted_price', 'group']) \
            .map(lambda r: (r.l_partkey, (r.discounted_price << 3) + r.group),
                 ['l_partkey', 'discounted_price_group'])

        part_scan = part_scan \
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

        # Result of join: (partkey, discounted_price | part_group, part_group)
        return lineitem_scan \
            .map(lambda r: (r.l_partkey, r.discounted_price_group)) \
            .join(part_scan.map(lambda r: (r.p_partkey, r.group))) \
            .alias(['l_partkey', 'discounted_price_lgroup', 'pgroup']) \
            .filter(lambda r: r.discounted_price_lgroup & r.pgroup != 0) \
            .map(lambda r: r.discounted_price_lgroup >> 3) \
            .reduce(lambda i1, i2: i1 + i2)

    def postprocess(self, res):
        res = Decimal(res) / 100 / 100
        return pd.DataFrame([res], columns=['revenue'])
