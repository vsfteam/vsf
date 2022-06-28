"""vsh: vsf shell feature tests."""

import os
import pytest
import platform
import time

from pytest_bdd import (
    given,
    scenario,
    then,
    when,
)

@pytest.fixture
def vsh_subprocess(pytestconfig):

    "TODO: support windows and usart"
    vsf_serial = pytestconfig.getoption("vsf_serial")
    vsf_serial_cfg = pytestconfig.getoption("vsf_serial_cfg")
    if vsf_serial and vsf_serial_cfg:
        if platform.system() == 'Windows':
            cmd = 'plink -serial \\\\.\\' + vsf_serial + ' -sercfg ' + vsf_serial_cfg
        else:
            # TODO: support screen or other serial tool
            cmd = 'picocom ' + vsf_serial_cfg  + ' ' + vsf_serial
    else:
        cmd = os.path.join(os.environ['VSF_PATH'], pytestconfig.getoption("vsf_template"))
    print("run cmd: %s" % cmd)

    if platform.system() == 'Windows':
        import wexpect
        vsh_subprocess = wexpect.spawn(cmd)
    else:
        import pexpect
        vsh_subprocess = pexpect.spawn(cmd, encoding='utf-8')

    vsf_vsh_started = pytestconfig.getoption("vsf_vsh_started")
    if not vsf_vsh_started:
        vsh_subprocess.expect('>>>', timeout=10)
        print("%s%s%s" % (
            '\n' + '*' * 80 + '\n',
            vsh_subprocess.before,
            vsh_subprocess.after,
        ))

    yield vsh_subprocess

    #if vsf_serial and vsf_serial_cfg and platform.system() != 'Windows':
    #    vsh_subprocess.send('\x01\x11')

    vsh_subprocess.close()


@scenario('../features/vsh.feature', 'ls')
def test_ls():
    pass

@given('connect vsh')
def connect_vsh(vsh_subprocess):
    return vsh_subprocess

@when('type ls /')
def type_ls_(vsh_subprocess):
    vsh_subprocess.sendline("ls /\r")

@then('receive string')
def receive_string(vsh_subprocess, pytestconfig):

    vsf_serial = pytestconfig.getoption("vsf_serial")
    vsf_serial_cfg = pytestconfig.getoption("vsf_serial_cfg")
    if vsf_serial and vsf_serial_cfg:
        time.sleep(1)

    vsh_subprocess.expect('var.*$')
    print("%s%s%s%s" % (
        '\n' + '*' * 80 + '\n',
        vsh_subprocess.before,
        '\n' + '*' * 80 + '\n',
        vsh_subprocess.after,
    ))

    assert 'ls' in vsh_subprocess.before
    assert 'bin' in vsh_subprocess.after
