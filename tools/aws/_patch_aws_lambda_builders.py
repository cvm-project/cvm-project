try:
    # pylint: disable=import-error
    from aws_lambda_builders.workflows.python_pip.workflow \
        import PythonPipWorkflow
except BaseException:
    pass

if 'PythonPipWorkflow' in locals():
    PythonPipWorkflow.EXCLUDED_FILES = \
        tuple(x for x in PythonPipWorkflow.EXCLUDED_FILES if x != "*.so")
