#define PY_SSIZE_T_CLEAN
#include "linalg.h"
#include "Python.h"

// TODO: Add matrix pyobjects, add matmul on the python side and add printing for matrices

/* 
When exposing a struct from C to the Python API, you need to do the following:

1. Define your struct as a python object.

This involves declaring a new struct specifically with the your struct as a field, and
the macro PyObject_HEAD as the other. The macro is essentially a bunch of fields that get
added to your custom PyObject that allows it to be compatible with Python's object system,
including reference count and type system. 

2. Define deallocation and initialization functions

Python needs to know what to do with these objects when they're created or destroyed.
We tend to take this for granted in Python when it's abstracted away for us.

Frequently you will see a *self argument in functions. This is just the conventional way
to refer to the pointer of a Python object, and it's fairly logical that it borrows
the "self" name in Python for an object to refer to itself.

Let's start by defining the struct as a PyVectorObject

*/

typedef struct {
    PyObject_HEAD
    vector vec;
} PyVectorObject;

typedef struct {
    PyObject_HEAD
    int num_rows;
    PyVectorObject **vectors;
} PyMatrixObject;

/* 
Let's make sure this next part makes sense to the reader, as the function body may 
look a bit daunting.

Py_TYPE is a macro from the Python.h header that returns the type object of self,
in this case the type object of PyVectorObject.

->tp_free accesses the `tp_free` field of the type object. It's a function pointer
that points to the deallocation function for objects of its type. Recall that in C,
(*pointer).method and pointer->method are equivalent. It's just a shorthand for accessing
a method of a pointer to a struct.

However, tp_free only works on a PyObject pointer, not a PyVectorObject pointer, which
self naturally is. (PyObject *)self casts it a PyObject pointer, which is safe because
our PyVectorObject has the PyObject_HEAD, which provides the necessary fields that allow
it to be treated as a PyObject.

tp_free is another trick from Python.h. It's a method that every type object has
and implements its destructor. So essentially, the deallocator says:

"Convert my custom object back in to a PyObject, and then call its deallocation function"
*/

static void PyVector_dealloc(PyVectorObject *self) {
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static void PyMatrix_dealloc(PyMatrixObject *self) {
    Py_TYPE(self)->tp_free((PyObject *)self);
}

/*
Next is an init function. Traditionally, the function naming is <name of obj>_init.

It traditionally expects *self again, as well as a PyObject pointer to any args,
and a PyObject pointer to any extra kwds.
*/

static int PyVector_init(PyVectorObject *self, PyObject *args) {
    // Initialize a PyObject null pointer to eventually be populated
    // by the args PyObject.
    PyObject *input_list = NULL;

    // Send the PyObject from args to the variable pointed to by &input_list
    // 0 if succeeds, else -1.
    if (!PyArg_ParseTuple(args, "O", &input_list)) {
        return -1;
    }

    // If we get here, it's a PyObject. But is it a list?
    if (!PyList_Check(input_list)) {
        PyErr_SetString(PyExc_TypeError, "Parameter must be a list.");
    }

    // Can now use the macro Py_ssize_t to get the size of the object
    // and use it to initialize size for the underlying size member.
    Py_ssize_t size = PyList_Size(input_list);

    // Now can populate the fields.
    // Need to cast size to int, as vector expects type int for size, not Py_ssize_t
    self->vec.size = (int)size;
    self->vec.elements = (float *)malloc(size * sizeof(float));
    
    // Check if the memory allocation was successfully or we'll unknowingly
    // be dealing with a null pointer
    if (self->vec.elements == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate memory for vector elements.");
        return -1;
    }

    // We've allocated memory for the elements, in that we have a float pointer
    // allocated, but it doesn't point to any actual variables yet. We have our
    // list though, so let's copy them over. This is perhaps inefficient, as
    // there's no true need to copy it into this object rather than playing with
    // the list object itself, but just for example.

    // We use Py_ssize_t as the type for the iterator, as we index a Python list
    // in the API with PyList_GetItem, which expects a Py_ssize_t for the indexing 
    // variable.
    for (Py_ssize_t i = 0; i < size; i++) {
        PyObject *item = PyList_GetItem(input_list, i);
        if (!PyFloat_Check(item)) {
            PyErr_SetString(PyExc_TypeError, "Elements must be of type float");
            // Still need to free the dynamically allocated vec.elements to prevent mem leaks
            free(self->vec.elements);
            return -1;
        }
        self->vec.elements[i] = (float)PyFloat_AsDouble(item);
    }

    return 0;
}

/* 
Hang on though. We have PyVectorObject referenced in PyVector_init, 
which initiatlizes it, but nothing to actually construct the object itself.

How would Python know how much memory to allocate for my object? Let's add
that. 

*/

// *args and *kwds are needed whether it's used or not
static PyObject *PyVector_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    // Declare a pointer (memory address) for our object
    PyVectorObject *self;

    // This one-liner allocates memory for the PyVectorObject.
    // It invoked tp_alloc, which is an analogue to malloc for
    // Python's C API, uses our fed type to work out how much
    // memory to allocate, returning a (PyObject *) type, 
    // which then needs to be cast to PyVectorObject so we
    // can access its fields and start initializing its member methods.
    self = (PyVectorObject *)type->tp_alloc(type, 0);

    // If the allocation succeeded let's initialize our stuff.
    if (self != NULL) {
        self->vec.size = 0;
        self->vec.elements = NULL;
    }

    // If PyVector_new is going to be used for tp_new, it expects a 
    // (PyObject *) return type.
    return (PyObject *)self;
}

static PyObject *PyMatrix_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PyMatrixObject *self;
    self = (PyMatrixObject *)type->tp_alloc(type, 0);

    if (self != NULL) {
        self->num_rows = 0;
        self->vectors = NULL;
    }

    return (PyObject *)self;
}

/* 
When a `PyVectorObject` is created, `tp_new` is called, using our previously
created PyVector_new function, where the PyTypeObject passed is below.

tp_alloc is ommitted here as ommitted fields use defaults. tp_alloc being
ommitted is the same as specifying tp_alloc = PyType_GenericAlloc here.
*/

static PyTypeObject PyVectorType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "linalg.Vector",
    .tp_basicsize = sizeof(PyVectorObject),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)PyVector_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyVector_new,
    .tp_init = (initproc)PyVector_init,  // Custom initialization function
};


static int PyMatrix_init(PyMatrixObject *self, PyObject *args) {

    PyObject *pyvector_list = NULL;


    if (!PyArg_ParseTuple(args, "O", &pyvector_list)) {
        return -1;
    }

    if (!PyList_Check(pyvector_list)) {
        PyErr_SetString(PyExc_TypeError, "Parameters must be a list of Vectors.");
        return -1;
    }

    Py_ssize_t size = PyList_Size(pyvector_list);


    self->num_rows = (int)size;
    self->vectors = (PyVectorObject **)malloc(sizeof(PyVectorObject) * size);

    if (self->vectors == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate memory for vectors.");
        return -1;
    }

    for (Py_ssize_t i = 0; i < size; i++) {
        PyObject *item = PyList_GetItem(pyvector_list, i);
        if (!(PyObject_TypeCheck(item, &PyVectorType))) {
            PyErr_SetString(PyExc_TypeError, "Elements must of type Vector");
            // Still need to free the dynamically allocated vec.elements to prevent mem leaks
            free(self->vectors);
            return -1;
        }
        Py_INCREF(item);
        self->vectors[i] = (PyVectorObject *)item;
    }

    return 0;
}

static PyTypeObject PyMatrixType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "linalg.Matrix",
    .tp_basicsize = sizeof(PyMatrixObject),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)PyMatrix_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyMatrix_new,
    .tp_init = (initproc)PyMatrix_init,  // Custom initialization function
};

/* 
Let's now add some Python wrappers for our functions in math.c 

This is not a particularly painful process. We mostly just need to
PyArgParseTuple to extract the objects so that we can expose
the underlying C structs.
*/


static PyObject* PyVector_dot_product(PyObject* self, PyObject* args) {
    PyVectorObject* vec1;
    PyVectorObject* vec2;
    if (!PyArg_ParseTuple(args, "O!O!", &PyVectorType, (PyObject**)&vec1, &PyVectorType, (PyObject**)&vec2)) {
        return NULL;
    }
    float result = dot_product(&vec1->vec, &vec2->vec);
    return PyFloat_FromDouble(result);
}

/*
We then just need to declare methods using a PyMethodDef object.
*/

static PyVectorObject* PyVector_FromVector(vector* v) {
    PyVectorObject* pyvec = NULL;
    pyvec->vec = *v;

    return pyvec;

}

static PyMatrixObject* PyMatrixObject_FromMatrix(matrix* mat) {
    PyMatrixObject* pymat;

    pymat->num_rows = mat->num_rows;

    for (int i = 0; i < mat->num_rows; ++i) {
        pymat->vectors[i] = PyVector_FromVector(&mat->vectors[i]);
    }

    free_matrix(mat);
    return pymat;
}

static matrix* PyMatrixObject_ToMatrix(PyMatrixObject* pymat) {
    matrix *c = malloc(sizeof(matrix));
    if (!c) return NULL; 

    c->num_rows = (int)pymat->num_rows;
    c->vectors = malloc(c->num_rows * sizeof(vector*));
    if (!c->vectors) {
        free(c);
        return NULL;
    }

    for (int i = 0; i < c->num_rows; ++i) {
        c->vectors[i] = pymat->vectors[i]->vec;
    }

    return c;
}

static PyObject* PyMatrix_matmul(PyObject* self, PyObject* args) {
    PyMatrixObject* a;
    PyMatrixObject* b;

    if (!PyArg_ParseTuple(args, "O!O!", &PyMatrixType, &a, &PyMatrixType, &b)) {
        PyErr_SetString(PyExc_ValueError, "Failed to unpack matrix arguments");
        return NULL;
    }

    matrix *mat_a = PyMatrixObject_ToMatrix(a);
    matrix *mat_b = PyMatrixObject_ToMatrix(b);

    matrix *mat_c = matmul(mat_a, mat_b);

    PyMatrixObject* result = PyMatrixObject_FromMatrix(mat_c);

    return (PyObject *)result;

}

static PyMethodDef linalg_methods[] = {
    {"dot_product", PyVector_dot_product, METH_VARARGS, "Compute the dot product of two linalg"},
    {"matmul", PyMatrix_matmul, METH_VARARGS, "Multiply two matrices"},
    {NULL, NULL, 0, NULL}  // Sentinel
};

/*
Now we have our custom type and some functions implemented. 
We just need to now create and initialized our module.

To do that, we are meant to provide a PyModuleDef struct with the module name,
a PyModuleDef_HEAD_INIT macro field, 
*/

// Module definition and initialization
static PyModuleDef linalg = {
    PyModuleDef_HEAD_INIT,
    .m_name = "linalg",

    // Setting this to -1 means the module keeps all its state in
    // global variables.
    .m_size = -1,
    .m_methods = linalg_methods,
};

// This macro declares that this function is the initialization function
// for the python module.
PyMODINIT_FUNC PyInit_linalg(void) {
    PyObject *m;

    // PyType_Ready takes the pointer for my PyVectorType 
    // struct earlier and ensures it's ready to be used.
    // It does a bunch of fancy stuff under the hood.
    if (PyType_Ready(&PyVectorType) < 0)
        return NULL;

    if (PyType_Ready(&PyMatrixType) < 0)
        return NULL;

    // Creates a Python module as per the linalg struct's
    // specifications defined above. Returns NULL if it fails.
    m = PyModule_Create(&linalg);
    if (m == NULL)
        return NULL;

    // Increments the reference count for the PyVectorType object.
    // When the module is imported, all type objects are subject
    // to the garbage collector, which deletes anything with a 0
    // reference count, so let's incremenet to ensure it doesn't.
    Py_INCREF(&PyVectorType);
    PyModule_AddObject(m, "Vector", (PyObject *)&PyVectorType);

    Py_INCREF(&PyMatrixType);
    PyModule_AddObject(m, "Matrix", (PyObject *)&PyMatrixType);
    return m;
}

// With this all set, it's ready to be compiled with setup.py