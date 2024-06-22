import asyncio
import serial
import serial_asyncio_fast
import pprint

data = {
    'index': 0,
    'status': 0,
    'testcase': [],
}

def test_report(data):
    results = [
        'PASS         ', # 0
        'SKIP         ', # 1
        'Watchdog Pass', # 2
        'Assert Fail  ', # 3
        'Watchdog Fail', # 4
        'Assist Fail  ', # 5
        'Fault Fail   ', # 6
    ]
    print('TestCase | Result        | Error_Info')
    for t in data['testcase']:
        print("%-8u | %s | %s" % (t['index'], results[t['result']],  t['err_str']))

def test_line_parser(line):
    response = None
    tokens = line.split()

    vsf_test_cmds = [
        'external_watch_init',
        'external_watch_feed',
        'testcase_index',
        'testcase_status',
        'testcase_result',
        'testcase_request',
        'testcase_all_done',
    ]
    if tokens[0] not in vsf_test_cmds:
        print('unknown line: %s' % line)
    #else:
        #print('match line: %s' % line)

    if tokens[0] == 'external_watch_init':
        timeout = int(tokens[1])
        #print('watchdog init timeout %d ms' % timeout)
    if tokens[0] == 'external_watch_feed':
        pass
    elif tokens[0] == 'testcase_index':
        if tokens[1] == 'read':
            if len(tokens) == 2:
                response = '%u\n' % data['index']
                print('testcase_index response: %s' % response, end='')
            else:
                print('testcase_index read error: %s' % line)
        elif tokens[1] == 'write':
            if len(tokens) == 3:
                new_index = int(tokens[2])
                if new_index != data['index']:
                    data['index'] = new_index
                    print('Test Case %u Start!' % new_index)
                else:
                    print('Test Case %u Powerup or Reboot!' % data['index'])
            else:
                print('testcase_index write error: %s' % line)
        else:
            print('parser testcase_index fail: %s' % line)
    elif tokens[0] == 'testcase_status':
        if tokens[1] == 'read':
            if len(tokens) == 2:
                response = '%u\n' % data['status']
                #print('testcase_status response: %s' % response, end='')
            else:
                print('testcase_index read error: %s' % line)
        elif tokens[1] == 'write':
            if len(tokens) == 3:
                data['status'] = int(tokens[2])
                #print('status to %d' % data['status'])
            else:
                print('testcase_index read error: %s' % line)
    elif tokens[0] == 'testcase_result':
        if len(tokens) >= 4 and tokens[1] == 'write':
            testcase = dict()
            testcase['index'] = int(tokens[2])
            testcase['result'] = int(tokens[3])
            if len(tokens) > 4:
                testcase['err_str'] = ' '.join(tokens[4:])
            else:
                testcase['err_str'] = ''
            data['testcase'].append(testcase)
            print('Test Case %u Finish!' % testcase['index'])
    elif tokens[0] == 'testcase_all_done':
        pprint.pp(data)
        test_report(data)
        data['index'] = 0
        data['status'] = 0
        data['testcase'] = []
    elif tokens[0] == 'testcase_request':
        request_name = tokens[1]
        request_param = ' '.join(tokens[2:])
        response = '1\n' # TODO
        #print('request: <name:%s> <param:%s> response: %s' % (request_name, request_param, response), end='')
    return response

async def reader():
    reader, writer = await serial_asyncio_fast.open_serial_connection(url='COM3', loop=loop, baudrate=115200, bytesize=8, stopbits=serial.STOPBITS_ONE)

    while True:
        line = await reader.readline()
        #print(line)
        try:
            line = line.decode('utf-8')
        except UnicodeDecodeError:
            continue
        line = line.rstrip('\n')
        response = test_line_parser(line)
        if response:
            writer.write(response.encode())


loop = asyncio.get_event_loop()
loop.run_until_complete(reader())
loop.close()
