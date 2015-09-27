
import os
from distutils.core import setup, Extension

setup(
    name='sdds',
    version='2.11',
    py_modules=['sdds'],
    ext_modules=[
        Extension('sddsdata', ['sddsdatamodule.c'],
                  include_dirs=['../../../include'],
                  libraries=['SDDS1','mdblib'],
                  extra_link_args=['-L../../../lib/'+os.environ['EPICS_HOST_ARCH']])
    ],
)
