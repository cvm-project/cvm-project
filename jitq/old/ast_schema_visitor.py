import ast


class SchemaVisitor(ast.NodeVisitor):
    def generic_visit(self, node):
        # print(node)
        ast.NodeVisitor.generic_visit(self, node)

    def go(self, schema, node):
        """
        computes the schema for this function
        :param schema: input schema
        :param node: function definition
        :return: output schema
        """
        self.visit(node)
        return 'c1'

    def visit_FunctionDef(self, node):
        print (node)