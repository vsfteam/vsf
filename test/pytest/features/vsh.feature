#@vsh
Feature: vsh: vsf shell
    TODO: Add more infomation

    #Scenario: vsh
    #    Given connect vsh
    #    When type vsh
    #    Then receive string

    Scenario: ls
        Given connect vsh
        When type ls /
        Then ls / output include "bin"
