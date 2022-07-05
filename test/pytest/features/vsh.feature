Feature: vsh: vsf shell
    TODO: Add more infomation

    @base
    Scenario Outline: vsh base
        Given connect vsh
        When type <vsh_input>
        Then <vsh_output> in output

        Examples:
        | vsh_input                                                             | vsh_output            |
        | ls /                                                                  | bin.*                 |
        | ls /bin                                                               | sh.*                  |
        | echo hello                                                            | hello                 |
        | cat /fatfs/FAKEFAT32/readme.txt                                       | readme.*              |
        | time ls /                                                             | take.*                |
        | export                                                                | PATH.*                |
        | export EXPORT_TEST=abcd1234                                           | EXPORT_TEST=abcd1234.*|
        | cd /bin && pwd && cd /                                                | /bin.*                |

    @coremark
    Scenario Outline: coremask
        Given connect vsh
        When type <vsh_input>
        Then <vsh_output> in output

        Examples:
        | vsh_input                                                             | vsh_output            |
        | coremark                                                              | CoreMark Size.*       |

    @memory
    Scenario Outline: memory
        Given connect vsh
        When type <vsh_input>
        Then <vsh_output> in output

        Examples:
        | vsh_input                                                             | vsh_output            |
        | free                                                                   | Mem:.*               |

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
        | mkdir -p /mnt/hostfs && mount -t winfs . /mnt/hostfs && ls /mnt/hostfs | pytest.*             |
        | json                                                                   | Member 1.*           |
        | socket                                                                 | ip for .* is .*      |

