from setuptools import setup, Extension

module = Extension('vectormodule',
                   sources=['src/vectormodule.c', 'src/math.c'],
                   include_dirs=['.'])  # Assuming vector.h is in the same directory

setup(name='VectorModule',
      version='1.0',
      description='A module that provides a vector type implemented in C',
      ext_modules=[module])
