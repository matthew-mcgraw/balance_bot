from setuptools import find_packages
from setuptools import setup

setup(
    name='balance_hw_interfaces',
    version='0.0.0',
    packages=find_packages(
        include=('balance_hw_interfaces', 'balance_hw_interfaces.*')),
)
