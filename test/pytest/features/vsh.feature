@vsh
Feature: vsh: vsf shell
    TODO: Add more infomation

    @command
    Scenario Outline: vsh
        Given connect vsh
        When type <vsh_input>
        Then <vsh_output> in output

        Examples:
        | vsh_input                         | vsh_output            |
        | ls /                              | bin.*                 |
        | ls /bin                           | sh.*                  |
        | echo hello                        | hello                 |
        | cat /fatfs/FAKEFAT32/readme.txt   | readme.*              |
        | time ls /                         | take.*                |
        | export                            | PATH.*                |
        #| coremark                          | CoreMark Size.*       |

    @linux-only
    Scenario Outline: vsh linux
        Given connect vsh
        When type <vsh_input>
        Then <vsh_output> in output

        Examples:
        | vsh_input                         | vsh_output            |
        | free                              | Mem:.*                |
