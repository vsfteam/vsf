#!/usr/bin/env python

def pytest_addoption(parser):
    # TODO: support cross-build test
    parser.addoption('--vsf_template', action='store', default='example/template/project/cmake/linux/build/vsf_template')
    parser.addoption('--vsf_serial', action='store', default=None)
    parser.addoption('--vsf_serial_cfg', action='store', default=None)
    parser.addoption('--vsf_vsh_started', action='store_true', default=False)
