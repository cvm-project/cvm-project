#include <Python.h>

static char module_docstring[] =
        "This module provides an interface to relational algebra operators";


static PyObject* operators_join(PyObject *self, PyObject *args) {
    PyObject *left_obj, *right_obj;
    if (!PyArg_ParseTuple(args, "00", &left_obj, &right_obj))
        return NULL;
    return Py_BuildValue("i", 10*100);
}



static PyMethodDef OperationsMethods[] = {
    {"join",  operators_join, METH_VARARGS, "Natural join"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC PyInit_rel_operators(void)
{

    PyObject *module;
    static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "operators",
        module_docstring,
        -1,
        OperationsMethods,
        NULL,
        NULL,
        NULL,
        NULL
    };
    module = PyModule_Create(&moduledef);
    if (!module) return NULL;

    return module;
}
