from setuptools import setup, Extension

module = Extension('linalg',
                   sources=['src/linalgmodule.c', 'src/linalg.c'],
                   include_dirs=['.'])

setup(name='LinalgModule',
      version='1.0',
      description='A module that provides linear algebra operations implemented in C',
      ext_modules=[module])
