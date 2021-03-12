$NOMOD51
;------------------------------------------------------------------------------
;  This file is part of the C51 Compiler package
;  Copyright (c) 1988-2005 Keil Elektronik GmbH and Keil Software, Inc.
;  Version 8.01
;
;  *** <<< Use Configuration Wizard in Context Menu >>> ***
;------------------------------------------------------------------------------
;  POST_STARTUP.A51:  This code is executed after startup reset.


                 NAME    ?C_START


post_startup     SEGMENT   CODE
                 RSEG      post_startup
;?STACK          SEGMENT   IDATA
;                RSEG    ?STACK
;                DS      1

                 EXTRN CODE (__vsf_main_entry)
                 PUBLIC  ?C_START

;                CSEG    AT      0
?C_START:        LJMP    __vsf_main_entry

;                RSEG    ?C_C51START

                 END
