import unittest
from blaze.ast_schema_visitor import SchemaVisitor

class TestIdentity(unittest.TestCase):
    def test_identity(self):
        func = lambda t: t
        input_schema = 'c1'
        res = SchemaVisitor().go(input_schema, func)
        self.assertEqual(input_schema, res)