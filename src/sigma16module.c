#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "config.h"
#ifdef ENABLE_TRACE
#include "events.h"
#endif
#include "vm.h"

typedef struct {
    PyObject_HEAD PyObject* d;
    PyObject* op;
    PyObject* sb;
    PyObject* sa;
} InstructionRRRObject;

static void InstructionRRR_dealloc(InstructionRRRObject* self) {
    Py_XDECREF(self->d);
    Py_XDECREF(self->op);
    Py_XDECREF(self->sb);
    Py_XDECREF(self->sa);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyMemberDef InstructionRRR_members[] = {
    {"d", T_OBJECT_EX, offsetof(InstructionRRRObject, d), 0,
     "destination register"},
    {"op", T_OBJECT_EX, offsetof(InstructionRRRObject, op), 0, "opcode"},
    {"sb", T_OBJECT_EX, offsetof(InstructionRRRObject, sb), 0,
     "source operand 2"},
    {"sa", T_OBJECT_EX, offsetof(InstructionRRRObject, sa), 0,
     "source operand 1"},
    {NULL}};

static PyTypeObject InstructionRRRType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "sigma16.InstructionRRR",
    .tp_doc = "Representation of RRR instruction",
    .tp_basicsize = sizeof(InstructionRRRObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = PyType_GenericNew,
    .tp_dealloc = (destructor)InstructionRRR_dealloc,
    .tp_members = InstructionRRR_members};

PyObject* Sigma16InstructionRRR_FromBytes(sigma16_inst_rrr_t instruction) {
    InstructionRRRObject* obj;
    if (!(obj = PyObject_New(InstructionRRRObject, &InstructionRRRType))) {
        return NULL;
    }
    Py_INCREF(obj);

    obj->d = PyLong_FromLong((long)instruction.d);
    obj->op = PyLong_FromLong((long)instruction.op);
    obj->sb = PyLong_FromLong((long)instruction.sb);
    obj->sa = PyLong_FromLong((long)instruction.sa);
    return obj;
}

typedef struct {
    PyObject_HEAD PyObject* d;
    PyObject* op;
    PyObject* sb;
    PyObject* sa;
    PyObject* disp;
} InstructionRXObject;

static void InstructionRX_dealloc(InstructionRXObject* self) {
    Py_XDECREF(self->d);
    Py_XDECREF(self->op);
    Py_XDECREF(self->sb);
    Py_XDECREF(self->sa);
    Py_XDECREF(self->disp);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyMemberDef InstructionRXObject_members[] = {
    {"d", T_OBJECT_EX, offsetof(InstructionRXObject, d), 0,
     "destination register"},
    {"op", T_OBJECT_EX, offsetof(InstructionRXObject, op), 0, "opcode"},
    {"sb", T_OBJECT_EX, offsetof(InstructionRXObject, sb), 0,
     "secondary opcode"},
    {"sa", T_OBJECT_EX, offsetof(InstructionRXObject, sa), 0,
     "source register"},
    {"disp", T_OBJECT_EX, offsetof(InstructionRXObject, disp), 0,
     "literal displacement for effective address"},
    {NULL}};

static PyTypeObject InstructionRXType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "sigma16.InstructionRX",
    .tp_doc = "Representation of RX instruction",
    .tp_basicsize = sizeof(InstructionRXObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = PyType_GenericNew,
    .tp_dealloc = (destructor)InstructionRX_dealloc,
    .tp_members = InstructionRXObject_members};

PyObject* Sigma16InstructionRX_FromBytes(sigma16_inst_rx_t instruction) {
    InstructionRXObject* obj;
    if (!(obj = PyObject_New(InstructionRXObject, &InstructionRXType))) {
        return NULL;
    }
    Py_INCREF(obj);

    obj->d = PyLong_FromLong((long)instruction.d);
    obj->op = PyLong_FromLong((long)instruction.op);
    obj->sb = PyLong_FromLong((long)instruction.sb);
    obj->sa = PyLong_FromLong((long)instruction.sa);
    obj->disp = PyLong_FromLong((long)instruction.disp);
    return obj;
}

typedef struct {
    PyObject_HEAD PyObject* d;
    PyObject* op;
    PyObject* sb;
    PyObject* ab;
} InstructionEXP0Object;

static void InstructionEXP0_dealloc(InstructionEXP0Object* self) {
    Py_XDECREF(self->d);
    Py_XDECREF(self->op);
    Py_XDECREF(self->ab);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyMemberDef InstructionEXP0_members[] = {
    {"d", T_OBJECT_EX, offsetof(InstructionEXP0Object, d), 0,
     "destination register"},
    {"op", T_OBJECT_EX, offsetof(InstructionEXP0Object, op), 0, "opcode"},
    {"ab", T_OBJECT_EX, offsetof(InstructionEXP0Object, ab), 0,
     "secondary opcode"},
    {NULL}};

static PyTypeObject InstructionEXP0Type = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "sigma16.InstructionEXP0",
    .tp_doc = "Representation of EXP0 instruction",
    .tp_basicsize = sizeof(InstructionEXP0Object),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = PyType_GenericNew,
    .tp_dealloc = (destructor)InstructionEXP0_dealloc,
    .tp_members = InstructionEXP0_members};

PyObject* Sigma16InstructionEXP0_FromBytes(sigma16_inst_exp0_t instruction) {
    InstructionEXP0Object* obj;
    if (!(obj = PyObject_New(InstructionEXP0Object, &InstructionEXP0Type))) {
        return NULL;
    }
    Py_INCREF(obj);

    obj->d = PyLong_FromLong((long)instruction.d);
    obj->op = PyLong_FromLong((long)instruction.op);
    obj->ab = PyLong_FromLong((long)instruction.ab);
    return obj;
}

typedef struct {
    PyObject_HEAD PyObject* cpu;
    PyObject* memory;
    PyObject* executable;
#ifdef ENABLE_TRACE
    PyObject* trace_handler;
#endif
    sigma16_vm_t* vm;
} EmulatorObject;

static void Emulator_dealloc(EmulatorObject* self) {
    Py_XDECREF(self->cpu);
    Py_XDECREF(self->memory);
    Py_XDECREF(self->executable);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

#ifdef ENABLE_TRACE
void vm_trace_compat(sigma16_vm_t* vm, enum sigma16_trace_event event) {
    PyObject* args;
    PyObject* instruction;

    switch (event) {
        case INST_RRR:
            instruction = Sigma16InstructionRRR_FromBytes(vm->cpu.ir.rrr);
            break;
        case INST_RX:
            instruction = Sigma16InstructionRX_FromBytes(vm->cpu.ir.rx);
            break;
        case INST_EXP0:
            instruction = Sigma16InstructionEXP0_FromBytes(vm->cpu.ir.exp0);
            break;
        default:
            return;
    }

    if (!((EmulatorObject*)vm->py_obj_self)->trace_handler) {
        return;
    }

    args = PyTuple_Pack(1, instruction);
    PyObject_CallObject(((EmulatorObject*)vm->py_obj_self)->trace_handler,
                        args);
}
#endif

static int Emulator_init(EmulatorObject* self, PyObject* args, PyObject* kwds) {
#ifdef ENABLE_TRACE
    static char* kwlist[] = {"executable", "trace_handler", NULL};
    PyObject* trace_handler = NULL;
#else
    static char* kwlist[] = {"executable", NULL};
#endif
    PyObject* executable = NULL;
    PyObject* tmp;

#ifdef ENABLE_TRACE
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OO", kwlist, &executable,
                                     &trace_handler)) {
        return -1;
    }
#else
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &executable)) {
        return -1;
    }
#endif
    if (executable) {
        tmp = self->executable;
        Py_INCREF(executable);
        self->executable = executable;
        Py_XDECREF(tmp);
    }
#ifdef ENABLE_TRACE
    if (trace_handler) {
        tmp = self->trace_handler;
        Py_INCREF(trace_handler);
        self->trace_handler = trace_handler;
        Py_XDECREF(tmp);
    }
#endif
    const char* executable_c_str = PyUnicode_AsUTF8(executable);
    if (sigma16_vm_init(&self->vm, executable_c_str) < 0) {
        return PyErr_SetFromErrno(PyExc_BaseException);
    }
#ifdef ENABLE_TRACE
    self->vm->py_obj_self = self;
    self->vm->trace_handler = vm_trace_compat;
#endif
    return 0;
}

static PyMemberDef Emulator_members[] = {
    {"cpu", T_OBJECT_EX, offsetof(EmulatorObject, cpu), 0, "sigma16 CPU"},
    {"memory", T_OBJECT_EX, offsetof(EmulatorObject, memory), 0,
     "sigma16 emulator memory view"},
    {"executable", T_OBJECT_EX, offsetof(EmulatorObject, executable), 0,
     "executable filename"},
#ifdef ENABLE_TRACE
    {"trace_handler", T_OBJECT_EX, offsetof(EmulatorObject, trace_handler), 0,
#endif
     "function handler for tracing"},
    {NULL}};

static PyObject* Emulator_execute(EmulatorObject* self,
                                  PyObject* Py_UNUSED(ignored)) {
    if (sigma16_vm_exec(self->vm) < 0) {
        return PyErr_SetFromErrno(PyExc_BaseException);
    }
    Py_RETURN_NONE;
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
    if (PyType_Ready(&InstructionRRRType) < 0) {
        return NULL;
    }
    if (PyType_Ready(&InstructionRXType) < 0) {
        return NULL;
    }
    if (PyType_Ready(&InstructionEXP0Type) < 0) {
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
    Py_INCREF(&InstructionRRRType);
    if (PyModule_AddObject(m, "InstructionRRR",
                           (PyObject*)&InstructionRRRType) < 0) {
        Py_DECREF(&InstructionRRRType);
        Py_DECREF(&EmulatorType);
        Py_DECREF(m);
        return NULL;
    }
    Py_INCREF(&InstructionRXType);
    if (PyModule_AddObject(m, "InstructionRX", (PyObject*)&InstructionRXType) <
        0) {
        Py_DECREF(&InstructionRXType);
        Py_DECREF(&EmulatorType);
        Py_DECREF(m);
        return NULL;
    }
    Py_INCREF(&InstructionEXP0Type);
    if (PyModule_AddObject(m, "InstructionEXP0",
                           (PyObject*)&InstructionEXP0Type) < 0) {
        Py_DECREF(&InstructionEXP0Type);
        Py_DECREF(&EmulatorType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
