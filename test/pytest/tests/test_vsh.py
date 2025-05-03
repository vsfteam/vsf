"""vsh: vsf shell feature tests."""

import os
import pytest
import platform
import time
import re

from conftest import printh

from pytest_bdd import (
    given,
    scenarios,
    then,
    when,
    parsers
)

scenarios("../features")

@pytest.fixture(scope='session')
def vsh(pytestconfig):
    vsf_serial = pytestconfig.getoption("vsf_serial")
    vsf_serial_cfg = pytestconfig.getoption("vsf_serial_cfg")
    if vsf_serial and vsf_serial_cfg:
        if platform.system() == 'Windows':
            cmd = 'plink -serial \\\\.\\' + vsf_serial + ' -sercfg ' + vsf_serial_cfg
        else:
            # TODO: support screen or other serial tool
            cmd = 'picocom ' + vsf_serial_cfg  + ' /dev/' + vsf_serial
    else:
        cmd = vsf_template = pytestconfig.getoption("vsf_template")
    print("run cmd: %s" % cmd)

    if platform.system() == 'Windows':
        # https://github.com/XnneHangLab/wexpect-uv
        import wexpect as pexpect
    else:
        import pexpect
    vsh_subprocess = pexpect.spawn(cmd, encoding='utf-8', timeout=pytestconfig.getoption("vsf_timeout"))

    vsf_vsh_started = pytestconfig.getoption("vsf_vsh_started")
    if not vsf_vsh_started:
        vsh_subprocess.expect('/.* # ', timeout=pytestconfig.getoption("vsf_timeout"))
        printh("expect prompt before", vsh_subprocess.before)
        printh("expect prompt after ", vsh_subprocess.after)

    yield  { "subprocess": vsh_subprocess, "input": None, "output": None, "config": pytestconfig }
    #yield vsh_subprocess

    vsh_subprocess.close()

@given('connect vsh')
def connect_vsh(vsh):
    pass

@when(parsers.parse('type {vsh_input}'))
def type_command(vsh, vsh_input):
    vsh['input'] = vsh_input.lstrip() + '\r'
    printh("sendline" , vsh['input'])
    vsh['subprocess'].sendline(vsh['input'])

@then(parsers.parse('{vsh_output} in output'))
def compare_output(vsh, vsh_output):
    vsh_output= vsh_output.lstrip()

    vsh['subprocess'].expect(vsh_output, timeout=vsh['config'].getoption("vsf_timeout"))

    printh("expect %s output before" % vsh['input'], vsh['subprocess'].before)
    printh("expect %s output after " % vsh['input'], vsh['subprocess'].after)

    assert re.search(vsh_output, vsh['subprocess'].after)
