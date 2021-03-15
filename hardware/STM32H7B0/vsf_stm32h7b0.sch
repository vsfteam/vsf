EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 8268 11693 portrait
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L SimonQian:STM32H7B0_64 U1
U 1 1 5EB53BB4
P 4350 3300
F 0 "U1" H 2950 4750 60  0000 L CNN
F 1 "STM32H7B0_64" H 4000 3300 60  0000 L CNN
F 2 "Package_QFP:TQFP-64_10x10mm_P0.5mm" V 4350 3300 60  0001 C CNN
F 3 "" V 4350 3300 60  0001 C CNN
	1    4350 3300
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:AP1117-33 U0
U 1 1 5EB69EE8
P 5900 6300
F 0 "U0" V 5650 6100 50  0000 L CNN
F 1 "AP1117-33" H 5700 6450 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-89-3" H 5900 6500 50  0001 C CNN
F 3 "http://www.diodes.com/datasheets/AP1117.pdf" H 6000 6050 50  0001 C CNN
	1    5900 6300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Micro_SD_Card J4
U 1 1 5EB752B8
P 6650 9050
F 0 "J4" H 6600 9767 50  0000 C CNN
F 1 "Micro_SD_Card" H 6600 9676 50  0000 C CNN
F 2 "Connector_Card:microSD_HC_Wuerth_693072010801" H 7800 9350 50  0001 C CNN
F 3 "http://katalog.we-online.de/em/datasheet/693072010801.pdf" H 6650 9050 50  0001 C CNN
	1    6650 9050
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x15_Odd_Even J2
U 1 1 5EB8F5EE
P 1400 6850
F 0 "J2" H 1450 7767 50  0000 C CNN
F 1 "Conn_02x15_Odd_Even" H 1450 7676 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x15_P2.54mm_Vertical" H 1400 6850 50  0001 C CNN
F 3 "~" H 1400 6850 50  0001 C CNN
	1    1400 6850
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x15_Odd_Even J3
U 1 1 5EB91F58
P 1400 8750
F 0 "J3" H 1450 9667 50  0000 C CNN
F 1 "Conn_02x15_Odd_Even" H 1450 9576 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x15_P2.54mm_Vertical" H 1400 8750 50  0001 C CNN
F 3 "~" H 1400 8750 50  0001 C CNN
	1    1400 8750
	1    0    0    -1  
$EndComp
$Comp
L Jumper:Jumper_3_Bridged12 JP2
U 1 1 5EBA9E50
P 4050 7750
F 0 "JP2" H 4050 7954 50  0000 C CNN
F 1 "Jumper_3_Bridged12" H 4050 7863 50  0000 C CNN
F 2 "SimonQian:SolderJumper-3_P0.8mm_Open_Pad0.6x0.8mm" H 4050 7750 50  0001 C CNN
F 3 "~" H 4050 7750 50  0001 C CNN
	1    4050 7750
	0    -1   -1   0   
$EndComp
$Comp
L Jumper:Jumper_3_Bridged12 JP1
U 1 1 5EBAABC1
P 2850 7750
F 0 "JP1" H 2850 7954 50  0000 C CNN
F 1 "Jumper_3_Bridged12" H 2850 7863 50  0000 C CNN
F 2 "SimonQian:SolderJumper-3_P0.8mm_Open_Pad0.6x0.8mm" H 2850 7750 50  0001 C CNN
F 3 "~" H 2850 7750 50  0001 C CNN
	1    2850 7750
	0    -1   -1   0   
$EndComp
$Comp
L Jumper:Jumper_3_Bridged12 JP3
U 1 1 5EBAB0D8
P 2850 8600
F 0 "JP3" H 2850 8804 50  0000 C CNN
F 1 "Jumper_3_Bridged12" H 2850 8713 50  0000 C CNN
F 2 "SimonQian:SolderJumper-3_P0.8mm_Open_Pad0.6x0.8mm" H 2850 8600 50  0001 C CNN
F 3 "~" H 2850 8600 50  0001 C CNN
	1    2850 8600
	0    -1   -1   0   
$EndComp
$Comp
L Jumper:Jumper_3_Bridged12 JP4
U 1 1 5EBAB6CA
P 4050 8600
F 0 "JP4" H 4050 8804 50  0000 C CNN
F 1 "Jumper_3_Bridged12" H 4050 8713 50  0000 C CNN
F 2 "SimonQian:SolderJumper-3_P0.8mm_Open_Pad0.6x0.8mm" H 4050 8600 50  0001 C CNN
F 3 "~" H 4050 8600 50  0001 C CNN
	1    4050 8600
	0    -1   -1   0   
$EndComp
$Comp
L Jumper:Jumper_3_Bridged12 JP6
U 1 1 5EBABBF3
P 4050 9450
F 0 "JP6" H 4050 9654 50  0000 C CNN
F 1 "Jumper_3_Bridged12" H 4050 9563 50  0000 C CNN
F 2 "SimonQian:SolderJumper-3_P0.8mm_Open_Pad0.6x0.8mm" H 4050 9450 50  0001 C CNN
F 3 "~" H 4050 9450 50  0001 C CNN
	1    4050 9450
	0    -1   -1   0   
$EndComp
$Comp
L Jumper:Jumper_3_Bridged12 JP5
U 1 1 5EBAC204
P 2850 9450
F 0 "JP5" H 2850 9654 50  0000 C CNN
F 1 "Jumper_3_Bridged12" H 2850 9563 50  0000 C CNN
F 2 "SimonQian:SolderJumper-3_P0.8mm_Open_Pad0.6x0.8mm" H 2850 9450 50  0001 C CNN
F 3 "~" H 2850 9450 50  0001 C CNN
	1    2850 9450
	0    -1   -1   0   
$EndComp
$Comp
L Jumper:Jumper_2_Open JP0
U 1 1 5EBAFB49
P 4200 950
F 0 "JP0" H 4150 900 50  0000 L CNN
F 1 "Jumper_2_Open" H 3950 1100 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 4200 950 50  0001 C CNN
F 3 "~" H 4200 950 50  0001 C CNN
	1    4200 950 
	1    0    0    -1  
$EndComp
$Comp
L Device:R R0
U 1 1 5EBB41D2
P 3850 950
F 0 "R0" V 3850 900 50  0000 L CNN
F 1 "10K" V 3750 900 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 3780 950 50  0001 C CNN
F 3 "~" H 3850 950 50  0001 C CNN
	1    3850 950 
	0    -1   -1   0   
$EndComp
$Comp
L Device:CP C12
U 1 1 5EB62DBA
P 6300 6450
F 0 "C12" V 6450 6450 50  0000 C CNN
F 1 "100uF" H 6150 6350 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 6338 6300 50  0001 C CNN
F 3 "~" H 6300 6450 50  0001 C CNN
	1    6300 6450
	1    0    0    -1  
$EndComp
$Comp
L Device:Crystal_GND24 Y0
U 1 1 5EB6C0C4
P 1900 3000
F 0 "Y0" V 1900 2950 50  0000 L CNN
F 1 "Crystal_GND24" H 1500 2650 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_2520-4Pin_2.5x2.0mm" H 1900 3000 50  0001 C CNN
F 3 "~" H 1900 3000 50  0001 C CNN
	1    1900 3000
	0    1    1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 5EB6EC50
P 6150 7100
F 0 "R1" V 6150 7050 50  0000 L CNN
F 1 "10K" V 6100 7200 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6080 7100 50  0001 C CNN
F 3 "~" H 6150 7100 50  0001 C CNN
	1    6150 7100
	0    1    1    0   
$EndComp
$Comp
L Device:C C11
U 1 1 5EB6F506
P 6000 7200
F 0 "C11" V 6050 7000 50  0000 L CNN
F 1 "100nF" V 6050 7250 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 6038 7050 50  0001 C CNN
F 3 "~" H 6000 7200 50  0001 C CNN
	1    6000 7200
	0    1    1    0   
$EndComp
$Comp
L Device:C C10
U 1 1 5EB70029
P 3600 1300
F 0 "C10" V 3550 1400 50  0000 L CNN
F 1 "100nF" V 3450 1200 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 3638 1150 50  0001 C CNN
F 3 "~" H 3600 1300 50  0001 C CNN
	1    3600 1300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C9
U 1 1 5EB706CE
P 6500 2550
F 0 "C9" V 6450 2600 50  0000 L CNN
F 1 "100nF" V 6350 2450 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 6538 2400 50  0001 C CNN
F 3 "~" H 6500 2550 50  0001 C CNN
	1    6500 2550
	0    1    1    0   
$EndComp
$Comp
L Device:C C8
U 1 1 5EB71392
P 6500 2750
F 0 "C8" V 6450 2800 50  0000 L CNN
F 1 "10uF" V 6450 2500 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 6538 2600 50  0001 C CNN
F 3 "~" H 6500 2750 50  0001 C CNN
	1    6500 2750
	0    1    1    0   
$EndComp
$Comp
L Device:C C7
U 1 1 5EB7291D
P 5100 5300
F 0 "C7" H 5100 5400 50  0000 L CNN
F 1 "100nF" H 5100 5200 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 5138 5150 50  0001 C CNN
F 3 "~" H 5100 5300 50  0001 C CNN
	1    5100 5300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 5EB732ED
P 4900 5300
F 0 "C6" H 4700 5300 50  0000 L CNN
F 1 "10uF" V 4850 5350 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 4938 5150 50  0001 C CNN
F 3 "~" H 4900 5300 50  0001 C CNN
	1    4900 5300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 5EB734D4
P 3800 5300
F 0 "C5" H 3800 5400 50  0000 L CNN
F 1 "100nF" H 3800 5200 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 3838 5150 50  0001 C CNN
F 3 "~" H 3800 5300 50  0001 C CNN
	1    3800 5300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 5EB73C0C
P 2100 3750
F 0 "C4" V 2050 3600 50  0000 L CNN
F 1 "100nF" V 2050 3800 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2138 3600 50  0001 C CNN
F 3 "~" H 2100 3750 50  0001 C CNN
	1    2100 3750
	0    1    1    0   
$EndComp
$Comp
L Connector:USB_B_Micro J1
U 1 1 5EB57899
P 3400 6350
F 0 "J1" H 3457 6817 50  0000 C CNN
F 1 "USB_B_Micro" H 3457 6726 50  0000 C CNN
F 2 "Connector_USB:USB_Micro-B_Wuerth_629105150521" H 3550 6300 50  0001 C CNN
F 3 "~" H 3550 6300 50  0001 C CNN
	1    3400 6350
	1    0    0    -1  
$EndComp
Text Label 3700 6150 0    50   ~ 0
VCC_5V
Text Label 3700 6350 0    50   ~ 0
PA12_OTG_DP
Text Label 3700 6450 0    50   ~ 0
PA11_OTG_DM
Text Label 3700 6550 0    50   ~ 0
PA10_OTG_ID
Wire Wire Line
	3700 6350 4200 6350
Wire Wire Line
	3700 6150 4200 6150
Wire Wire Line
	3700 6450 4200 6450
Wire Wire Line
	3700 6550 4200 6550
$Comp
L power:GND #PWR0101
U 1 1 5EBAF79D
P 5900 6600
F 0 "#PWR0101" H 5900 6350 50  0001 C CNN
F 1 "GND" H 5905 6427 50  0000 C CNN
F 2 "" H 5900 6600 50  0001 C CNN
F 3 "" H 5900 6600 50  0001 C CNN
	1    5900 6600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 6600 5900 6600
Connection ~ 5900 6600
Wire Wire Line
	6300 6300 6200 6300
Wire Wire Line
	5350 6300 5400 6300
Text Label 5350 6300 0    50   ~ 0
VCC_5V
Text Label 6200 6300 0    50   ~ 0
VCC_3V3
Wire Wire Line
	6500 6300 6300 6300
Connection ~ 6300 6300
Text Label 5400 8750 0    50   ~ 0
SDIO_D2
Text Label 5400 8850 0    50   ~ 0
SDIO_D3
Text Label 5400 8950 0    50   ~ 0
SDIO_CMD
Wire Wire Line
	5400 8950 5750 8950
Wire Wire Line
	5400 8750 5750 8750
Text Label 5400 9050 0    50   ~ 0
VCC_3V3
Text Label 5400 9150 0    50   ~ 0
SDIO_CK
Wire Wire Line
	5400 9150 5750 9150
$Comp
L power:GND #PWR0102
U 1 1 5EBCCEA7
P 5750 9250
F 0 "#PWR0102" H 5750 9000 50  0001 C CNN
F 1 "GND" V 5755 9122 50  0000 R CNN
F 2 "" H 5750 9250 50  0001 C CNN
F 3 "" H 5750 9250 50  0001 C CNN
	1    5750 9250
	0    1    1    0   
$EndComp
Wire Wire Line
	5400 9350 5750 9350
Text Label 3000 8600 0    50   ~ 0
SDIO_D0
Text Label 5400 9450 0    50   ~ 0
SDIO_D1
Wire Wire Line
	5400 9450 5750 9450
Text Label 5950 2950 0    50   ~ 0
PA12_OTG_DP
Text Label 5950 3050 0    50   ~ 0
PA11_OTG_DM
Text Label 5950 3150 0    50   ~ 0
PA10_OTG_ID
Wire Wire Line
	6450 2950 5950 2950
Wire Wire Line
	6450 3050 5950 3050
Wire Wire Line
	6450 3150 5950 3150
Wire Wire Line
	6450 2850 5950 2850
Text Label 5950 2850 0    50   ~ 0
PA13_SWDIO
$Comp
L power:GND #PWR0103
U 1 1 5EC25A14
P 6650 2550
F 0 "#PWR0103" H 6650 2300 50  0001 C CNN
F 1 "GND" V 6655 2422 50  0000 R CNN
F 2 "" H 6650 2550 50  0001 C CNN
F 3 "" H 6650 2550 50  0001 C CNN
	1    6650 2550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5950 2750 6350 2750
$Comp
L power:GND #PWR0104
U 1 1 5EC273D4
P 6650 2750
F 0 "#PWR0104" H 6650 2500 50  0001 C CNN
F 1 "GND" V 6655 2622 50  0000 R CNN
F 2 "" H 6650 2750 50  0001 C CNN
F 3 "" H 6650 2750 50  0001 C CNN
	1    6650 2750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6450 3250 5950 3250
Wire Wire Line
	6450 3350 5950 3350
Wire Wire Line
	6450 3450 5950 3450
Wire Wire Line
	6450 3550 5950 3550
Wire Wire Line
	6450 3650 5950 3650
Wire Wire Line
	6450 3750 5950 3750
Wire Wire Line
	6450 3850 5950 3850
Wire Wire Line
	6450 3950 5950 3950
Wire Wire Line
	6450 4050 5950 4050
Text Label 5950 3250 0    50   ~ 0
PA9
Text Label 5950 3350 0    50   ~ 0
PA8
Text Label 5950 3450 0    50   ~ 0
PC9_SDMMC1_D1
Text Label 5950 3550 0    50   ~ 0
PC7
Text Label 5950 3650 0    50   ~ 0
PC6
Text Label 5950 3750 0    50   ~ 0
PB15_SDMMC2_D1
Text Label 5950 3850 0    50   ~ 0
PB14_SDMMC2_D0
Text Label 5950 3950 0    50   ~ 0
PB13_SDMMC1_D0
Text Label 5950 4050 0    50   ~ 0
PB12
Text Label 4900 1700 1    50   ~ 0
PC10_SDMMC1_D2
Text Label 4800 1700 1    50   ~ 0
PC11_SDMMC1_D3
Text Label 4700 1700 1    50   ~ 0
PC12_SDMMC1_CK
Text Label 4600 1700 1    50   ~ 0
PD2_SDMMC1_CMD
Wire Wire Line
	4900 1200 4900 1700
Wire Wire Line
	4800 1200 4800 1700
Wire Wire Line
	4700 1200 4700 1700
Wire Wire Line
	4600 1200 4600 1700
Wire Wire Line
	5000 1200 5000 1700
Wire Wire Line
	5100 1200 5100 1700
Text Label 5100 1700 1    50   ~ 0
PA14_SWCLK
Text Label 5000 1700 1    50   ~ 0
PA15
Wire Wire Line
	4500 1200 4500 1700
Wire Wire Line
	4400 1200 4400 1700
Wire Wire Line
	4300 1200 4300 1700
Wire Wire Line
	4200 1200 4200 1700
Wire Wire Line
	4100 1200 4100 1700
Wire Wire Line
	3900 1200 3900 1700
Wire Wire Line
	4000 1700 4000 950 
Connection ~ 4000 950 
$Comp
L power:GND #PWR0105
U 1 1 5EC489B7
P 3600 950
F 0 "#PWR0105" H 3600 700 50  0001 C CNN
F 1 "GND" V 3605 822 50  0000 R CNN
F 2 "" H 3600 950 50  0001 C CNN
F 3 "" H 3600 950 50  0001 C CNN
	1    3600 950 
	0    1    1    0   
$EndComp
Wire Wire Line
	6650 2650 6650 2550
Wire Wire Line
	5950 2650 6650 2650
Connection ~ 6650 2550
Wire Wire Line
	5950 2550 6350 2550
Text Label 5950 2550 0    50   ~ 0
VCC_3V3
Text Label 4400 950  0    50   ~ 0
VCC_3V3
Wire Wire Line
	4700 950  4400 950 
Wire Wire Line
	3800 1700 3800 1200
Text Label 3600 1700 1    50   ~ 0
VCC_3V3
Wire Wire Line
	3600 1700 3600 1450
Wire Wire Line
	3700 1700 3700 1150
Wire Wire Line
	3700 1150 3600 1150
Wire Wire Line
	3600 1150 3600 950 
Connection ~ 3600 1150
Wire Wire Line
	3700 950  3600 950 
Connection ~ 3600 950 
Text Label 4500 1700 1    50   ~ 0
PB3_SDMMC2_D2
Text Label 4400 1700 1    50   ~ 0
PB4_SDMMC2_D3
Text Label 4300 1700 1    50   ~ 0
PB5
Text Label 4200 1700 1    50   ~ 0
PB6
Text Label 4100 1700 1    50   ~ 0
PB7
Text Label 3900 1700 1    50   ~ 0
PB8
Text Label 3800 1700 1    50   ~ 0
PB9
Wire Wire Line
	2250 2550 2750 2550
$Comp
L Device:C C2
U 1 1 5EC67F81
P 2600 2950
F 0 "C2" V 2550 3000 50  0000 L CNN
F 1 "20pF" V 2650 3000 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2638 2800 50  0001 C CNN
F 3 "~" H 2600 2950 50  0001 C CNN
	1    2600 2950
	0    1    1    0   
$EndComp
$Comp
L Device:C C3
U 1 1 5EC6C99E
P 2500 3050
F 0 "C3" V 2450 2900 50  0000 L CNN
F 1 "20pF" V 2550 3100 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2538 2900 50  0001 C CNN
F 3 "~" H 2500 3050 50  0001 C CNN
	1    2500 3050
	0    1    1    0   
$EndComp
Wire Wire Line
	2750 3050 2650 3050
$Comp
L power:GND #PWR0106
U 1 1 5EC7651D
P 2100 3000
F 0 "#PWR0106" H 2100 2750 50  0001 C CNN
F 1 "GND" V 2000 3050 50  0000 R CNN
F 2 "" H 2100 3000 50  0001 C CNN
F 3 "" H 2100 3000 50  0001 C CNN
	1    2100 3000
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 5EC8635A
P 1700 3000
F 0 "#PWR0107" H 1700 2750 50  0001 C CNN
F 1 "GND" V 1800 3050 50  0000 R CNN
F 2 "" H 1700 3000 50  0001 C CNN
F 3 "" H 1700 3000 50  0001 C CNN
	1    1700 3000
	0    1    1    0   
$EndComp
Wire Wire Line
	2750 3150 2400 3150
Wire Wire Line
	2750 3250 2500 3250
Text Label 2500 3250 0    50   ~ 0
PC0
Text Label 2150 3350 0    50   ~ 0
PC1_SDMMC2_CK
Text Label 6000 7100 2    50   ~ 0
VCC_3V3
Wire Wire Line
	5700 7100 6000 7100
Wire Wire Line
	6300 7200 6150 7200
$Comp
L power:GND #PWR0108
U 1 1 5ECCBE32
P 5850 7200
F 0 "#PWR0108" H 5850 6950 50  0001 C CNN
F 1 "GND" H 5950 7050 50  0000 R CNN
F 2 "" H 5850 7200 50  0001 C CNN
F 3 "" H 5850 7200 50  0001 C CNN
	1    5850 7200
	0    1    1    0   
$EndComp
Wire Wire Line
	2150 3450 2750 3450
Wire Wire Line
	2150 3550 2750 3550
Text Label 2750 3750 2    50   ~ 0
VCC_3V3
Wire Wire Line
	2150 3850 2750 3850
Wire Wire Line
	2150 3950 2750 3950
Wire Wire Line
	2150 4050 2750 4050
Text Label 2150 3850 0    50   ~ 0
PA0_SDMMC2_CMD
Wire Wire Line
	2750 3750 2250 3750
Wire Wire Line
	2750 3650 1900 3650
Wire Wire Line
	1900 3650 1900 3750
Wire Wire Line
	1900 3750 1950 3750
$Comp
L power:GND #PWR0109
U 1 1 5ECF3180
P 1900 3750
F 0 "#PWR0109" H 1900 3500 50  0001 C CNN
F 1 "GND" H 2000 3600 50  0000 R CNN
F 2 "" H 1900 3750 50  0001 C CNN
F 3 "" H 1900 3750 50  0001 C CNN
	1    1900 3750
	0    1    1    0   
$EndComp
Connection ~ 1900 3750
Text Label 2150 3950 0    50   ~ 0
PA1
Text Label 2150 4050 0    50   ~ 0
PA2
Wire Wire Line
	2150 3350 2750 3350
Text Label 2150 3550 0    50   ~ 0
PC3
Text Label 2150 3450 0    50   ~ 0
PC2
Text Label 3800 4900 3    50   ~ 0
VCC_3V3
Text Label 3600 5050 1    50   ~ 0
PA3
Wire Wire Line
	3600 4900 3600 5050
Text Label 3900 5050 1    50   ~ 0
PA4
Wire Wire Line
	3900 4900 3900 5050
Wire Wire Line
	3800 4900 3800 5150
Wire Wire Line
	3700 4900 3700 5450
Wire Wire Line
	3700 5450 3800 5450
$Comp
L power:GND #PWR0110
U 1 1 5ED2450F
P 4900 5450
F 0 "#PWR0110" H 4900 5200 50  0001 C CNN
F 1 "GND" H 5000 5300 50  0000 R CNN
F 2 "" H 4900 5450 50  0001 C CNN
F 3 "" H 4900 5450 50  0001 C CNN
	1    4900 5450
	0    1    1    0   
$EndComp
Text Label 4000 5050 1    50   ~ 0
PA5
Wire Wire Line
	4000 4900 4000 5050
Wire Wire Line
	4100 5050 4100 4900
Wire Wire Line
	4200 5050 4200 4900
Wire Wire Line
	4300 5050 4300 4900
Wire Wire Line
	4400 5050 4400 4900
Wire Wire Line
	4500 5050 4500 4900
Wire Wire Line
	4600 5050 4600 4900
Wire Wire Line
	4700 5050 4700 4900
Wire Wire Line
	4800 5050 4800 4900
Wire Wire Line
	4900 4900 4900 5150
Wire Wire Line
	5100 5450 5000 5450
Connection ~ 4900 5450
Wire Wire Line
	5000 4900 5000 5450
Connection ~ 5000 5450
Wire Wire Line
	5000 5450 4900 5450
Text Label 5100 4900 3    50   ~ 0
VCC_3V3
Wire Wire Line
	5100 5150 5100 4900
$Comp
L power:GND #PWR0111
U 1 1 5ED5A06D
P 3700 5450
F 0 "#PWR0111" H 3700 5200 50  0001 C CNN
F 1 "GND" H 3800 5300 50  0000 R CNN
F 2 "" H 3700 5450 50  0001 C CNN
F 3 "" H 3700 5450 50  0001 C CNN
	1    3700 5450
	0    1    1    0   
$EndComp
Connection ~ 3700 5450
Text Label 4100 5050 1    50   ~ 0
PA6
Text Label 4200 5050 1    50   ~ 0
PA7
Text Label 4300 5050 1    50   ~ 0
PC4
Text Label 4400 5050 1    50   ~ 0
PC5
Text Label 4500 5050 1    50   ~ 0
PB0
Text Label 4600 5050 1    50   ~ 0
PB1
Text Label 4700 5050 1    50   ~ 0
PB2
Text Label 4800 5050 1    50   ~ 0
PB10
Text Label 3000 7750 0    50   ~ 0
SDIO_CMD
Text Label 3000 7500 0    50   ~ 0
PA0_SDMMC2_CMD
Text Label 3000 8000 0    50   ~ 0
PD2_SDMMC1_CMD
Wire Wire Line
	3650 7500 2850 7500
Wire Wire Line
	3650 7750 3000 7750
Text Label 4200 7750 0    50   ~ 0
SDIO_CK
Text Label 4200 7500 0    50   ~ 0
PC1_SDMMC2_CK
Text Label 4200 8000 0    50   ~ 0
PC12_SDMMC1_CK
Wire Wire Line
	3650 8000 2850 8000
Wire Wire Line
	4850 7500 4050 7500
Wire Wire Line
	4850 7750 4200 7750
Wire Wire Line
	4850 8000 4050 8000
Text Label 3000 8850 0    50   ~ 0
PB13_SDMMC1_D0
Text Label 3000 8350 0    50   ~ 0
PB14_SDMMC2_D0
Wire Wire Line
	3700 8350 2850 8350
Wire Wire Line
	3700 8600 3000 8600
Wire Wire Line
	3700 8850 2850 8850
Text Label 5400 9350 0    50   ~ 0
SDIO_D0
Text Label 4200 8600 0    50   ~ 0
SDIO_D1
Text Label 4200 8350 0    50   ~ 0
PB15_SDMMC2_D1
Text Label 4200 8850 0    50   ~ 0
PC9_SDMMC1_D1
Wire Wire Line
	4850 8850 4050 8850
Wire Wire Line
	4850 8600 4200 8600
Wire Wire Line
	4850 8350 4050 8350
Text Label 3000 9450 0    50   ~ 0
SDIO_D2
Text Label 3000 9700 0    50   ~ 0
PC10_SDMMC1_D2
Text Label 3000 9200 0    50   ~ 0
PB3_SDMMC2_D2
Wire Wire Line
	3650 9200 2850 9200
Wire Wire Line
	3650 9450 3000 9450
Wire Wire Line
	3650 9700 2850 9700
Text Label 4200 9450 0    50   ~ 0
SDIO_D3
Text Label 4200 9700 0    50   ~ 0
PC11_SDMMC1_D3
Text Label 4200 9200 0    50   ~ 0
PB4_SDMMC2_D3
Wire Wire Line
	4850 9200 4050 9200
Wire Wire Line
	4850 9450 4200 9450
Wire Wire Line
	4850 9700 4050 9700
$Comp
L Device:LED D0
U 1 1 5EEF4AAF
P 6950 6300
F 0 "D0" H 6943 6045 50  0000 C CNN
F 1 "LED" H 6943 6136 50  0000 C CNN
F 2 "LED_SMD:LED_0402_1005Metric" H 6950 6300 50  0001 C CNN
F 3 "~" H 6950 6300 50  0001 C CNN
	1    6950 6300
	-1   0    0    1   
$EndComp
$Comp
L Device:R R2
U 1 1 5EEF6928
P 6650 6300
F 0 "R2" V 6650 6250 50  0000 L CNN
F 1 "1K" V 6750 6250 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6580 6300 50  0001 C CNN
F 3 "~" H 6650 6300 50  0001 C CNN
	1    6650 6300
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0112
U 1 1 5EEF8FD7
P 7100 6300
F 0 "#PWR0112" H 7100 6050 50  0001 C CNN
F 1 "GND" H 7105 6127 50  0000 C CNN
F 2 "" H 7100 6300 50  0001 C CNN
F 3 "" H 7100 6300 50  0001 C CNN
	1    7100 6300
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R3
U 1 1 5EF17504
P 5250 9000
F 0 "R3" V 5250 8950 50  0000 L CNN
F 1 "10K" V 5150 8900 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5180 9000 50  0001 C CNN
F 3 "~" H 5250 9000 50  0001 C CNN
	1    5250 9000
	-1   0    0    1   
$EndComp
Wire Wire Line
	5250 8850 5750 8850
Wire Wire Line
	5250 9150 5350 9150
Wire Wire Line
	5350 9150 5350 9050
Wire Wire Line
	5350 9050 5750 9050
Wire Wire Line
	2400 3150 2400 3250
Wire Wire Line
	6300 7100 6300 7200
Wire Wire Line
	2400 3250 2150 3250
Text Label 2150 3250 0    50   ~ 0
NRST
Text Label 6450 7400 0    50   ~ 0
NRST
Text Label 6450 7300 0    50   ~ 0
PA13_SWDIO
Text Label 6450 7100 0    50   ~ 0
PA14_SWCLK
$Comp
L power:GND #PWR0113
U 1 1 5EF9131E
P 6900 7200
F 0 "#PWR0113" H 6900 6950 50  0001 C CNN
F 1 "GND" V 6900 7100 50  0000 R CNN
F 2 "" H 6900 7200 50  0001 C CNN
F 3 "" H 6900 7200 50  0001 C CNN
	1    6900 7200
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_DIP_x01 SW0
U 1 1 5EFBD80F
P 6000 7400
F 0 "SW0" H 6000 7350 50  0000 C CNN
F 1 "SW_DIP_x01" H 6000 7250 50  0000 C CNN
F 2 "SimonQian:Key_2Pin_2.9X4.3" H 6000 7400 50  0001 C CNN
F 3 "~" H 6000 7400 50  0001 C CNN
	1    6000 7400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0114
U 1 1 5EFBEDF8
P 5700 7400
F 0 "#PWR0114" H 5700 7150 50  0001 C CNN
F 1 "GND" H 5800 7250 50  0000 R CNN
F 2 "" H 5700 7400 50  0001 C CNN
F 3 "" H 5700 7400 50  0001 C CNN
	1    5700 7400
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_DIP_x01 SW1
U 1 1 5EFBF226
P 6000 7950
F 0 "SW1" H 6000 7900 50  0000 C CNN
F 1 "SW_DIP_x01" H 6000 7800 50  0000 C CNN
F 2 "SimonQian:Key_2Pin_2.9X4.3" H 6000 7950 50  0001 C CNN
F 3 "~" H 6000 7950 50  0001 C CNN
	1    6000 7950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 3050 2200 3150
Wire Wire Line
	2200 3150 1900 3150
Wire Wire Line
	2000 2850 2350 2850
Wire Wire Line
	2000 2750 2000 2850
Wire Wire Line
	2450 2750 2300 2750
$Comp
L Device:C C0
U 1 1 5EC904B2
P 2600 2750
F 0 "C0" V 2550 2850 50  0000 L CNN
F 1 "20pF" V 2650 2800 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2638 2600 50  0001 C CNN
F 3 "~" H 2600 2750 50  0001 C CNN
	1    2600 2750
	0    1    1    0   
$EndComp
$Comp
L Device:C C1
U 1 1 5EC8FDAC
P 2500 2850
F 0 "C1" V 2550 2700 50  0000 L CNN
F 1 "20pF" V 2450 2600 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2538 2700 50  0001 C CNN
F 3 "~" H 2500 2850 50  0001 C CNN
	1    2500 2850
	0    1    1    0   
$EndComp
$Comp
L Device:Crystal Y1
U 1 1 5EB6E1F0
P 2150 2750
F 0 "Y1" V 2150 2750 50  0000 C CNN
F 1 "Crystal" V 1950 2800 50  0000 C CNN
F 2 "Crystal:Crystal_SMD_EuroQuartz_EQ161-2Pin_3.2x1.5mm" H 2150 2750 50  0001 C CNN
F 3 "~" H 2150 2750 50  0001 C CNN
	1    2150 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 2950 2450 2950
Wire Wire Line
	2200 3050 2350 3050
Wire Wire Line
	1900 2850 1950 2850
Wire Wire Line
	1950 2850 1950 2900
Wire Wire Line
	1950 2900 2200 2900
Wire Wire Line
	2200 2900 2200 2950
Wire Wire Line
	2750 2650 2250 2650
Text Label 6300 7950 0    50   ~ 0
PC2_MODE
$Comp
L power:GND #PWR0115
U 1 1 5EFF612D
P 5700 7950
F 0 "#PWR0115" H 5700 7700 50  0001 C CNN
F 1 "GND" H 5800 7800 50  0000 R CNN
F 2 "" H 5700 7950 50  0001 C CNN
F 3 "" H 5700 7950 50  0001 C CNN
	1    5700 7950
	0    1    1    0   
$EndComp
Wire Wire Line
	6300 7950 6700 7950
Text Label 2250 2650 0    50   ~ 0
PC2_MODE
Wire Wire Line
	6450 7100 6900 7100
Wire Wire Line
	6450 7300 6900 7300
Wire Wire Line
	6300 7400 6900 7400
Wire Wire Line
	6300 7400 6300 7200
Connection ~ 6300 7400
Connection ~ 6300 7200
$Comp
L Device:CP C13
U 1 1 5EBF71A6
P 5400 6450
F 0 "C13" V 5550 6450 50  0000 C CNN
F 1 "100uF" H 5550 6350 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 5438 6300 50  0001 C CNN
F 3 "~" H 5400 6450 50  0001 C CNN
	1    5400 6450
	1    0    0    -1  
$EndComp
Connection ~ 5400 6300
Wire Wire Line
	5400 6300 5600 6300
Wire Wire Line
	5400 6600 5900 6600
Wire Wire Line
	2750 2850 2650 2850
$Comp
L Connector_Generic:Conn_01x04 J0
U 1 1 5EC68CD6
P 7100 7200
F 0 "J0" H 7180 7192 50  0000 L CNN
F 1 "Conn_01x04" H 7180 7101 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 7100 7200 50  0001 C CNN
F 3 "~" H 7100 7200 50  0001 C CNN
	1    7100 7200
	1    0    0    -1  
$EndComp
Text Label 1200 7550 2    50   ~ 0
VCC_5V
Wire Wire Line
	700  7550 1200 7550
Wire Wire Line
	700  7450 1200 7450
Wire Wire Line
	700  7350 1200 7350
Wire Wire Line
	700  7250 1200 7250
Wire Wire Line
	700  7150 1200 7150
Wire Wire Line
	700  7050 1200 7050
Wire Wire Line
	700  6950 1200 6950
Wire Wire Line
	700  6850 1200 6850
Wire Wire Line
	700  6750 1200 6750
Wire Wire Line
	700  6650 1200 6650
Wire Wire Line
	700  6550 1200 6550
Wire Wire Line
	700  6450 1200 6450
Wire Wire Line
	700  6350 1200 6350
Wire Wire Line
	700  6250 1200 6250
Wire Wire Line
	700  6150 1200 6150
Wire Wire Line
	1700 7350 2200 7350
Wire Wire Line
	1700 7250 2200 7250
Wire Wire Line
	1700 7150 2200 7150
Wire Wire Line
	1700 7050 2200 7050
Wire Wire Line
	1700 6950 2200 6950
Wire Wire Line
	1700 6850 2200 6850
Wire Wire Line
	1700 6750 2200 6750
Wire Wire Line
	1700 6650 2200 6650
Wire Wire Line
	1700 6550 2200 6550
Wire Wire Line
	1700 6450 2200 6450
Wire Wire Line
	1700 6350 2200 6350
Wire Wire Line
	1700 6250 2200 6250
Wire Wire Line
	1700 6150 2200 6150
Wire Wire Line
	700  9250 1200 9250
Wire Wire Line
	700  9150 1200 9150
Wire Wire Line
	700  9050 1200 9050
Wire Wire Line
	700  8950 1200 8950
Wire Wire Line
	700  8850 1200 8850
Wire Wire Line
	700  8750 1200 8750
Wire Wire Line
	700  8650 1200 8650
Wire Wire Line
	700  8550 1200 8550
Wire Wire Line
	700  8450 1200 8450
Wire Wire Line
	700  8350 1200 8350
Wire Wire Line
	700  8250 1200 8250
Wire Wire Line
	700  8150 1200 8150
Wire Wire Line
	700  8050 1200 8050
Wire Wire Line
	1700 9250 2200 9250
Wire Wire Line
	1700 9150 2200 9150
Wire Wire Line
	1700 9050 2200 9050
Wire Wire Line
	1700 8950 2200 8950
Wire Wire Line
	1700 8850 2200 8850
Wire Wire Line
	1700 8750 2200 8750
Wire Wire Line
	1700 8650 2200 8650
Wire Wire Line
	1700 8550 2200 8550
Wire Wire Line
	1700 8450 2200 8450
Wire Wire Line
	1700 8350 2200 8350
Wire Wire Line
	1700 8250 2200 8250
Wire Wire Line
	1700 8150 2200 8150
Wire Wire Line
	1700 8050 2200 8050
Text Label 1700 9450 0    50   ~ 0
VCC_5V
$Comp
L power:GND #PWR0116
U 1 1 5EBEB0FD
P 1700 7450
F 0 "#PWR0116" H 1700 7200 50  0001 C CNN
F 1 "GND" V 1705 7277 50  0000 C CNN
F 2 "" H 1700 7450 50  0001 C CNN
F 3 "" H 1700 7450 50  0001 C CNN
	1    1700 7450
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0117
U 1 1 5EBEBB8C
P 1700 7550
F 0 "#PWR0117" H 1700 7300 50  0001 C CNN
F 1 "GND" V 1705 7377 50  0000 C CNN
F 2 "" H 1700 7550 50  0001 C CNN
F 3 "" H 1700 7550 50  0001 C CNN
	1    1700 7550
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0118
U 1 1 5EBEC0D3
P 1200 9350
F 0 "#PWR0118" H 1200 9100 50  0001 C CNN
F 1 "GND" V 1205 9177 50  0000 C CNN
F 2 "" H 1200 9350 50  0001 C CNN
F 3 "" H 1200 9350 50  0001 C CNN
	1    1200 9350
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0119
U 1 1 5EBEDE07
P 1200 9450
F 0 "#PWR0119" H 1200 9200 50  0001 C CNN
F 1 "GND" V 1205 9277 50  0000 C CNN
F 2 "" H 1200 9450 50  0001 C CNN
F 3 "" H 1200 9450 50  0001 C CNN
	1    1200 9450
	0    1    1    0   
$EndComp
Text Label 1200 7450 2    50   ~ 0
VCC_3V3
Text Label 1700 9350 0    50   ~ 0
VCC_3V3
Text Label 1700 7350 0    50   ~ 0
NRST
Text Label 1200 7350 2    50   ~ 0
PA13_SWDIO
Text Label 1700 7250 0    50   ~ 0
PA14_SWCLK
Text Label 4000 1700 1    50   ~ 0
BOOT
Text Label 1200 7250 2    50   ~ 0
BOOT
Wire Wire Line
	1700 9450 2200 9450
Wire Wire Line
	1700 9350 2200 9350
Text Label 2250 2550 0    50   ~ 0
VCC_3V3
Text Label 1700 9250 0    50   ~ 0
PC2_MODE
Text Label 1200 9050 2    50   ~ 0
PB8
Text Label 1700 9050 0    50   ~ 0
PB9
Text Label 1700 8950 0    50   ~ 0
PB7
Text Label 1200 8950 2    50   ~ 0
PB6
Text Label 1700 8850 0    50   ~ 0
PB5
Text Label 1200 8850 2    50   ~ 0
PB4_SDMMC2_D3
Text Label 1700 8750 0    50   ~ 0
PB3_SDMMC2_D2
Text Label 1200 8750 2    50   ~ 0
PD2_SDMMC1_CMD
Text Label 1700 8650 0    50   ~ 0
PC12_SDMMC1_CK
Text Label 1200 8650 2    50   ~ 0
PC11_SDMMC1_D3
Text Label 1700 8550 0    50   ~ 0
PC10_SDMMC1_D2
Text Label 1200 8550 2    50   ~ 0
PA15
Text Label 1200 8450 2    50   ~ 0
PA12_OTG_DP
Text Label 1700 8450 0    50   ~ 0
PA11_OTG_DM
Text Label 1700 8350 0    50   ~ 0
PA10_OTG_ID
Text Label 1200 8350 2    50   ~ 0
PA9
Text Label 1700 8250 0    50   ~ 0
PA8
Text Label 1200 8250 2    50   ~ 0
PC9_SDMMC1_D1
Text Label 1700 7150 0    50   ~ 0
PC0
Text Label 1200 7150 2    50   ~ 0
PC1_SDMMC2_CK
Text Label 1700 7050 0    50   ~ 0
PC2
Text Label 1200 7050 2    50   ~ 0
PC3
Text Label 1700 6850 0    50   ~ 0
PA0_SDMMC2_CMD
Text Label 1200 6950 2    50   ~ 0
PA1
Text Label 1700 6950 0    50   ~ 0
PA2
Text Label 1200 6850 2    50   ~ 0
PA3
Text Label 1700 6750 0    50   ~ 0
PA4
Text Label 1200 6750 2    50   ~ 0
PA5
Text Label 1700 6650 0    50   ~ 0
PA6
Text Label 1200 6650 2    50   ~ 0
PA7
Text Label 1700 6550 0    50   ~ 0
PC4
Text Label 1200 6550 2    50   ~ 0
PC5
Text Label 1700 6450 0    50   ~ 0
PB0
Text Label 1200 6450 2    50   ~ 0
PB1
Text Label 1700 6350 0    50   ~ 0
PB2
Text Label 1200 6350 2    50   ~ 0
PB10
Text Label 1700 6250 0    50   ~ 0
PB12
Text Label 1200 6250 2    50   ~ 0
PB13_SDMMC1_D0
Text Label 1700 6150 0    50   ~ 0
PB14_SDMMC2_D0
Text Label 1200 6150 2    50   ~ 0
PB15_SDMMC2_D1
Text Label 1700 8150 0    50   ~ 0
PC7
Text Label 1200 8150 2    50   ~ 0
PC6
$EndSCHEMATC
