"""vsh: vsf shell feature tests."""

import os
import pytest
import platform
import time

from conftest import printh

from pytest_bdd import (
    given,
    scenario,
    then,
    when,
    parsers
)

@pytest.fixture
def vsh_subprocess(pytestconfig):
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
        import wexpect
        vsh_subprocess = wexpect.spawn(cmd, encoding='utf-8')
    else:
        import pexpect
        vsh_subprocess = pexpect.spawn(cmd, encoding='utf-8')

    vsf_vsh_started = pytestconfig.getoption("vsf_vsh_started")
    if not vsf_vsh_started:
        vsh_subprocess.expect('>>>', timeout=pytestconfig.getoption("vsf_timeout"))
        printh("expect >>> before", vsh_subprocess.before)
        printh("expect >>> after ", vsh_subprocess.after)

    yield vsh_subprocess

    vsh_subprocess.close()


@scenario('../features/vsh.feature', 'ls')
def test_ls():
    pass

@given('connect vsh')
def connect_vsh(vsh_subprocess):
    return vsh_subprocess

@when('type ls /')
def type_ls_(vsh_subprocess, pytestconfig):
    vsh_subprocess.sendline("ls /\r")

@then(parsers.parse('ls / output include "{output}"'))
def receive_string(vsh_subprocess, pytestconfig, output):

    vsh_subprocess.expect(output + '.*$', timeout=pytestconfig.getoption("vsf_timeout"))

    printh("expect ls / output before", vsh_subprocess.before)
    printh("expect ls / output after ", vsh_subprocess.after)

    assert 'ls /' in vsh_subprocess.before
    assert output in vsh_subprocess.after
