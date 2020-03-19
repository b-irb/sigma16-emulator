#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "instructions.h"
#include "vm.h"

typedef struct {
    PyObject_HEAD PyObject* cpu;
    PyObject* memory;
    PyObject* executable;
    PyObject* trap_handler;
    sigma16_vm_t* vm;
} EmulatorObject;

static void Emulator_dealloc(EmulatorObject* self) {
    Py_XDECREF(self->cpu);
    Py_XDECREF(self->memory);
    Py_XDECREF(self->executable);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static int Emulator_init(EmulatorObject* self, PyObject* args, PyObject* kwds) {
    static char* kwlist[] = {"executable", "trap_handler"};
    PyObject* executable = NULL;
    PyObject* trap_handler = NULL;
    PyObject* tmp;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OO", kwlist, &executable,
                                     &trap_handler)) {
        return -1;
    }

    if (executable) {
        tmp = self->executable;
        Py_INCREF(executable);
        self->executable = executable;
        Py_XDECREF(tmp);
    }

    if (trap_handler) {
        tmp = self->trap_handler;
        Py_INCREF(trap_handler);
        self->trap_handler = trap_handler;
        Py_XDECREF(tmp);
    }

    const char* executable_c_str = PyUnicode_AsUTF8(executable);
    if (sigma16_vm_init(&self->vm, executable_c_str) < 0) {
        return PyErr_SetFromErrno(PyExc_BaseException);
    }

    Py_XDECREF(tmp);
    return 0;
}

static PyMemberDef Emulator_members[] = {
    {"cpu", T_OBJECT_EX, offsetof(EmulatorObject, cpu), 0, "sigma16 CPU"},
    {"memory", T_OBJECT_EX, offsetof(EmulatorObject, memory), 0,
     "sigma16 emulator memory view"},
    {"executable", T_OBJECT_EX, offsetof(EmulatorObject, executable), 0,
     "executable filename"},
    {"trap_handler", T_OBJECT_EX, offsetof(EmulatorObject, trap_handler), 0,
     "function handler for tracing"},
    {NULL}};

static PyObject* Emulator_execute(EmulatorObject* self,
                                  PyObject* Py_UNUSED(ignored)) {
    if (sigma16_vm_exec(self->vm) < 0) {
        return PyErr_SetFromErrno(PyExc_BaseException);
    }
    Py_RETURN_NONE;
}

void sigma16_trace(sigma16_vm_t* vm, enum sigma16_instruction_fmt fmt) {
    switch (fmt) {
        case RRR:
            break;
        case RX:
            break;
        case EXP:
            break;
    };
}

static PyMethodDef Emulator_methods[] = {
    {"execute", (PyCFunction)Emulator_execute, METH_NOARGS,
     "Read and execute filename held in executable instance attribute"},
    {NULL}};

static PyTypeObject EmulatorType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "sigma16.Emulator",
    .tp_doc = "Sigma16 emulator",
    .tp_basicsize = sizeof(EmulatorObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)Emulator_init,
    .tp_dealloc = (destructor)Emulator_dealloc,
    .tp_members = Emulator_members,
    .tp_methods = Emulator_methods};

static PyModuleDef sigma16_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "sigma16",
    .m_doc = "Sigma16 interfacing layer",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_sigma16(void) {
    PyObject* m;
    if (PyType_Ready(&EmulatorType) < 0) {
        return NULL;
    }

    m = PyModule_Create(&sigma16_module);
    if (m == NULL) {
        return NULL;
    }

    Py_INCREF(&EmulatorType);
    if (PyModule_AddObject(m, "Emulator", (PyObject*)&EmulatorType) < 0) {
        Py_DECREF(&EmulatorType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
