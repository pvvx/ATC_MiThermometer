#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#############################################################################
# atc_mi_interface module setup [name: atc-mi-interface]
#############################################################################

from setuptools import setup

DESCRIPTION = (
    'Python tools and API for the "atc1441" and "pvvx" Xiaomi Mijia'
    ' Thermometer custom firmware (ATC_MiThermometer)'
)

PACKAGE_NAME = "atc-mi-interface"

VERSIONFILE = "atc_mi_interface/__version__.py"

LONG_DESCRIPTION = '''
# atc-mi-interface

__Python tools and API for the "atc1441" and "pvvx" Xiaomi Mijia Thermometer custom firmware__

Python components (API and command-line tools, including some GUI) to represent
the data model of the BLE advertisements of the Xiaomi Mijia Thermometer custom
firmware (ATC_MiThermometer) developed by
[atc1441](https://github.com/atc1441/ATC_MiThermometer)
and [pvvx](https://github.com/pvvx/ATC_MiThermometer). When using the latest
releases of the "pvvx" firmware, it also allows to read and write the
firmware configuration parameters.

A documented interface and a testing tool are included to receive, decode,
show, build and edit the BLE advertisements.

Installation without GUI:

```
pip install [ -i https://test.pypi.org/simple/ ] atc-mi-interface
```

Installation including wxPython prerequisites for the GUI:

```
pip install [ -i https://test.pypi.org/simple/ ] atc-mi-interface[gui]
```

Full information, installation notes, API reference and usage details at the
[pvvx/ATC_MiThermometer/python-interface repository](https://github.com/pvvx/ATC_MiThermometer/tree/master/python-interface#python-interfacing-methods-and-data-representation-model).
'''

###########################################################################

verstrline = open(VERSIONFILE, "rt").read()
VSRE = r"^__version__ = ['\"]([^'\"]*)['\"]"
mo = re.search(VSRE, verstrline, re.M)
if mo:
    verstr = mo.group(1)
else:
    raise RuntimeError("Unable to find version string in %s." % (VERSIONFILE,))

setup(
    name=PACKAGE_NAME,
    version=verstr,
    description=DESCRIPTION,
    long_description=LONG_DESCRIPTION,
    long_description_content_type="text/markdown",
    classifiers=[
        "License :: OSI Approved :: The Unlicense (Unlicense)",
        "Topic :: Software Development :: Libraries :: Python Modules",
        'Programming Language :: Python :: 3 :: Only',
        "Programming Language :: Python :: Implementation :: CPython",
        "Development Status :: 5 - Production/Stable",
        "Typing :: Typed",
        "Intended Audience :: Developers",
    ],
    author="Ircama",
    url="https://github.com/pvvx/ATC_MiThermometer/tree/master/python-interface",
    license='https://unlicense.org',
    packages=["atc_mi_interface"],
    entry_points={
        "console_scripts": [
            "atc_mi_config=atc_mi_interface.atc_mi_config:main",
            "atc_mi_advertising=atc_mi_interface.atc_mi_advertising:main",
            "atc_mi_format_test=atc_mi_interface.atc_mi_format_test:main"
        ]
    },
    include_package_data=True,
    install_requires=[
        'construct',
        'bleak',
        'pycryptodome',
        'arrow'
    ],
    extras_require={
        'gui': [
            "wxPython",
            "construct-gallery>=1.1.0",
            "construct-editor"
        ]
    },
    keywords=[
        PACKAGE_NAME,
        "Xiaomi",
        "Mijia",
        "Thermometer",
        "firmware",
        "wxpython",
        "editor",
        "construct",
        "bleak",
        "BLE",
        "bluetooth",
        "LYWSD03MMC",
    ],
    python_requires=">=3.8",
)
