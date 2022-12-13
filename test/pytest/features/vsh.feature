Feature: vsh: vsf shell
    TODO: Add more information

    @base
    Scenario Outline: vsh base
        Given connect vsh
        When type <vsh_input>
        Then <vsh_output> in output

        Examples:
        | vsh_input                                                              | vsh_output            |
        | ls /                                                                   | bin.*                 |
        | ls /bin                                                                | sh.*                  |
        | time ls /                                                              | take.*                |
        | export                                                                 | PATH.*                |
        | export EXPORT_TEST=abcd1234                                            | EXPORT_TEST=abcd1234.*|
        | echo "$PATH"                                                           | /bin.*                |
        | cd /bin && pwd && cd /                                                 | /bin/.*               |
        #| love                                                                  | .*                    |
        #| cpp_test                                                              | 12345678              |
        #| lua\r\nprint(2 ^ 32);\r\n os.exit()\r\n                               | 4294967296            |

    @coremark
    Scenario Outline: coremask
        Given connect vsh
        When type <vsh_input>
        Then <vsh_output> in output

        Examples:
        | vsh_input                                                              | vsh_output            |
        | coremark                                                               | CoreMark Size.*       |

    @memory
    Scenario Outline: memory
        Given connect vsh
        When type <vsh_input>
        Then <vsh_output> in output

        Examples:
        | vsh_input                                                              | vsh_output            |
        | free                                                                   | Mem:.*                |

    @linux-only
    Scenario Outline: vsh linux
        Given connect vsh
        When type <vsh_input>
        Then <vsh_output> in output

        Examples:
        | vsh_input                                                              | vsh_output           |
        | mkdir -p /mnt/hostfs && mount -t linfs . /mnt/hostfs && ls /mnt/hostfs | pytest.*             |

    @windows-only
    Scenario Outline: vsh windows
        Given connect vsh
        When type <vsh_input>
        Then <vsh_output> in output

        Examples:
        | vsh_input                                                              | vsh_output           |
        | cat /memfs/webroot/index.html                                          | Hello World.*        |
        | mkdir -p /mnt/hostfs && mount -t winfs . /mnt/hostfs && ls /mnt/hostfs | pytest.*             |
        | json                                                                   | Member 1.*           |
        | socket                                                                 | ip for .* is .*      |
        | vsfvm /memfs/test.dart                                                 | thread1 loop: 4.*    |

