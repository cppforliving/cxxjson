from setuptools import setup, Extension
from Cython.Build import cythonize
from glob import glob

sources = glob('cython/cxx.pyx')
sources += glob('src/**/*.cpp')

cxx = Extension(
    name='cxx',
    sources=sources,
    include_dirs=['./', './inc'],
    language='c++',
    extra_compile_args=['-O2', '-std=c++17']
)

setup(
    name='cxx',
    version='0.1',
    url='https://gitlab.com/attu/cxx',
    ext_modules=cythonize(cxx)
)
