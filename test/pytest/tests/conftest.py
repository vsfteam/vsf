#!/usr/bin/env python

import logging
from hexdump import hexdump

def pytest_addoption(parser):
    parser.addoption('--vsf_timeout',     action='store',      default=10,    type=int, help='default expect timeout')
    parser.addoption('--vsf_serial',      action='store',      default=None,            help='run vsh test by serial')
    parser.addoption('--vsf_serial_cfg',  action='store',      default=None,            help='serial configuration')
    parser.addoption('--vsf_vsh_started', action='store_true', default=False,           help='skip capturing the output of vsh startup')
    parser.addoption('--vsf_template',    action='store',                               help='executable programs path')

def printh(prefix_str, dump_str):
    logging.info ("%s : \n%s" % (prefix_str, dump_str))
    logging.debug("\n%s"      % hexdump(bytes(dump_str, encoding='utf-8')))

