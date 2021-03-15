EESchema Schematic File Version 4
LIBS:JTAG_ext-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
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
L Connector_Generic:Conn_02x10_Odd_Even J0
U 1 1 5D974276
P 1450 1050
F 0 "J0" H 1500 1550 50  0000 C CNN
F 1 "Conn_02x10_Odd_Even" H 1500 450 50  0000 C CNN
F 2 "SimonQian:PinSocket_2x10_P2.54mm_Horizontal_2Layer" H 1450 1050 50  0001 C CNN
F 3 "~" H 1450 1050 50  0001 C CNN
	1    1450 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 850  1250 850 
Wire Wire Line
	1000 1450 1250 1450
Wire Wire Line
	1000 1550 1250 1550
Wire Wire Line
	1750 850  2000 850 
Wire Wire Line
	1750 1350 2000 1350
Wire Wire Line
	1750 1450 2000 1450
Wire Wire Line
	1750 1550 2000 1550
Text Label 1000 650  0    50   ~ 0
PB.0
Text Label 1750 1350 0    50   ~ 0
PB.15
Text Label 1000 750  0    50   ~ 0
PB.2
Text Label 1000 850  0    50   ~ 0
PB.4
Text Label 1000 950  0    50   ~ 0
PB.6
Text Label 1000 1050 0    50   ~ 0
PB.8
Text Label 1000 1150 0    50   ~ 0
PB.10
Text Label 1000 1250 0    50   ~ 0
PB.12
Text Label 1000 1350 0    50   ~ 0
PB.14
Text Label 1750 650  0    50   ~ 0
PB.1
Text Label 1750 750  0    50   ~ 0
PB.3
Text Label 1750 850  0    50   ~ 0
PB.5
Text Label 1750 950  0    50   ~ 0
PB.7
Text Label 1750 1050 0    50   ~ 0
PB.9
Text Label 1750 1150 0    50   ~ 0
PB.11
Text Label 1750 1250 0    50   ~ 0
PB.13
Text Label 1000 1450 0    50   ~ 0
VCC
Text Label 1000 1550 0    50   ~ 0
VCC5V
Text Label 1750 1450 0    50   ~ 0
GND
Text Label 1750 1550 0    50   ~ 0
GND
$Comp
L Connector_Generic:Conn_02x10_Odd_Even J1
U 1 1 5D978A31
P 6050 1050
F 0 "J1" H 6100 1550 50  0000 C CNN
F 1 "Conn_02x10_Odd_Even" H 6100 450 50  0000 C CNN
F 2 "SimonQian:PinSocket_2x10_P2.54mm_Horizontal_2Layer" H 6050 1050 50  0001 C CNN
F 3 "~" H 6050 1050 50  0001 C CNN
	1    6050 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 650  5850 650 
Wire Wire Line
	5600 750  5850 750 
Wire Wire Line
	5600 850  5850 850 
Wire Wire Line
	5600 950  5850 950 
Wire Wire Line
	5600 1050 5850 1050
Wire Wire Line
	5600 1150 5850 1150
Wire Wire Line
	5600 1250 5850 1250
Wire Wire Line
	5600 1350 5850 1350
Wire Wire Line
	5600 1450 5850 1450
Wire Wire Line
	5600 1550 5850 1550
Text Label 5600 650  0    50   ~ 0
Vref
Text Label 5600 750  0    50   ~ 0
nTRST
Text Label 5600 850  0    50   ~ 0
TDI
Text Label 5600 950  0    50   ~ 0
TMS
Text Label 5600 1050 0    50   ~ 0
TCK
Text Label 5600 1150 0    50   ~ 0
RTCK
Text Label 5600 1250 0    50   ~ 0
TDO
Text Label 5600 1350 0    50   ~ 0
nRST
Wire Wire Line
	6350 650  6600 650 
Wire Wire Line
	6350 750  6600 750 
Wire Wire Line
	6350 850  6600 850 
Wire Wire Line
	6350 950  6600 950 
Wire Wire Line
	6350 1050 6600 1050
Wire Wire Line
	6350 1150 6600 1150
Wire Wire Line
	6350 1250 6600 1250
Wire Wire Line
	6350 1350 6600 1350
Wire Wire Line
	6350 1450 6600 1450
Wire Wire Line
	6350 1550 6600 1550
Text Label 6350 1450 0    50   ~ 0
GND
Text Label 6350 1550 0    50   ~ 0
GND
Text Label 6350 650  0    50   ~ 0
Vsupply
Text Label 6350 1350 0    50   ~ 0
GND
Text Label 6350 1250 0    50   ~ 0
GND
Text Label 6350 1150 0    50   ~ 0
GND
Text Label 6350 1050 0    50   ~ 0
GND
Text Label 6350 950  0    50   ~ 0
GND
Text Label 6350 850  0    50   ~ 0
GND
Text Label 6350 750  0    50   ~ 0
GND
Text Label 3950 650  0    50   ~ 0
Vsupply
Text Label 2600 650  0    50   ~ 0
VCC
Text Label 5200 650  0    50   ~ 0
Vref
Text Label 5200 850  0    50   ~ 0
nTRST
Text Label 5200 1050 0    50   ~ 0
TDI
Text Label 5200 1150 0    50   ~ 0
TMS
Text Label 5200 1250 0    50   ~ 0
TCK
Text Label 5200 1450 0    50   ~ 0
TDO
Text Label 5200 950  0    50   ~ 0
nRST
Text Label 2000 650  0    50   ~ 0
USCI1_CLK
Text Label 550  750  0    50   ~ 0
USCI1_DAT0
Wire Wire Line
	550  750  1250 750 
Text Label 2000 750  0    50   ~ 0
USCI1_DAT1
Wire Wire Line
	1750 650  2450 650 
Wire Wire Line
	1750 750  2450 750 
Wire Wire Line
	550  650  1250 650 
Text Label 550  650  0    50   ~ 0
EADC0_CH0
Text Label 550  1050 0    50   ~ 0
SPI3_MOSI
Wire Wire Line
	550  1050 1250 1050
Text Label 2000 1050 0    50   ~ 0
SPI3_MISO
Wire Wire Line
	1750 1050 2450 1050
Text Label 2000 1150 0    50   ~ 0
SPI3_CLK
Wire Wire Line
	1750 1150 2450 1150
Text Label 550  1250 0    50   ~ 0
SPI0_MOSI
Wire Wire Line
	550  1250 1250 1250
Text Label 550  1350 0    50   ~ 0
SPI0_CLK
Wire Wire Line
	550  1350 1250 1350
Text Label 2600 750  0    50   ~ 0
EADC0_CH0
Text Label 4400 1250 0    50   ~ 0
SPI3_CLK
Text Label 3950 1250 0    50   ~ 0
SPI0_CLK
Text Label 3950 1150 0    50   ~ 0
SPI0_MOSI
Text Label 4400 1050 0    50   ~ 0
SPI3_MOSI
Text Label 4400 1450 0    50   ~ 0
SPI3_MISO
Text Label 3050 1250 0    50   ~ 0
USCI1_CLK
Text Label 3050 1150 0    50   ~ 0
USCI1_DAT0
Text Label 3500 1150 0    50   ~ 0
USCI1_DAT1
Text Label 5600 1450 0    50   ~ 0
TXD
Text Label 5600 1550 0    50   ~ 0
RXD
Text Label 5200 1550 0    50   ~ 0
TXD
Text Label 5200 1650 0    50   ~ 0
RXD
Wire Wire Line
	1750 1250 2000 1250
Wire Wire Line
	2600 650  4350 650 
Wire Wire Line
	2600 750  4800 750 
Wire Wire Line
	2600 850  4800 850 
Wire Wire Line
	2600 950  4800 950 
Wire Wire Line
	2600 1050 4800 1050
Wire Wire Line
	2600 1150 4800 1150
Wire Wire Line
	2600 1250 4800 1250
Wire Wire Line
	2600 1450 4800 1450
Wire Wire Line
	2600 1550 4800 1550
Wire Wire Line
	2600 1650 4800 1650
Wire Wire Line
	550  950  1250 950 
Text Label 550  950  0    50   ~ 0
UART1_RXD
Wire Wire Line
	1750 950  2450 950 
Text Label 2000 950  0    50   ~ 0
UART1_TXD
Text Label 2600 1650 0    50   ~ 0
UART1_RXD
Text Label 2600 1550 0    50   ~ 0
UART1_TXD
Text Label 2600 850  0    50   ~ 0
PB.4
Text Label 2600 950  0    50   ~ 0
PB.5
Text Label 550  1150 0    50   ~ 0
UART4_RXD
Wire Wire Line
	550  1150 1250 1150
Text Label 2600 1450 0    50   ~ 0
UART4_RXD
$Comp
L Device:R R0
U 1 1 5D9E66B6
P 4950 850
F 0 "R0" V 4900 1000 50  0000 C CNN
F 1 "22" V 4950 850 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4880 850 50  0001 C CNN
F 3 "~" H 4950 850 50  0001 C CNN
	1    4950 850 
	0    1    1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 5D9E7A9C
P 4950 950
F 0 "R1" V 4900 1100 50  0000 C CNN
F 1 "22" V 4950 950 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4880 950 50  0001 C CNN
F 3 "~" H 4950 950 50  0001 C CNN
	1    4950 950 
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 5D9F9C46
P 4950 1050
F 0 "R2" V 4900 1200 50  0000 C CNN
F 1 "22" V 4950 1050 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4880 1050 50  0001 C CNN
F 3 "~" H 4950 1050 50  0001 C CNN
	1    4950 1050
	0    1    1    0   
$EndComp
$Comp
L Device:R R3
U 1 1 5D9FA12C
P 4950 1150
F 0 "R3" V 4900 1300 50  0000 C CNN
F 1 "22" V 4950 1150 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4880 1150 50  0001 C CNN
F 3 "~" H 4950 1150 50  0001 C CNN
	1    4950 1150
	0    1    1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 5D9FA2ED
P 4950 1250
F 0 "R4" V 4900 1400 50  0000 C CNN
F 1 "22" V 4950 1250 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4880 1250 50  0001 C CNN
F 3 "~" H 4950 1250 50  0001 C CNN
	1    4950 1250
	0    1    1    0   
$EndComp
$Comp
L Device:R R5
U 1 1 5D9FA626
P 4950 1450
F 0 "R5" V 4900 1600 50  0000 C CNN
F 1 "22" V 4950 1450 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4880 1450 50  0001 C CNN
F 3 "~" H 4950 1450 50  0001 C CNN
	1    4950 1450
	0    1    1    0   
$EndComp
$Comp
L Device:R R6
U 1 1 5D9FA84A
P 4950 1550
F 0 "R6" V 4900 1700 50  0000 C CNN
F 1 "22" V 4950 1550 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4880 1550 50  0001 C CNN
F 3 "~" H 4950 1550 50  0001 C CNN
	1    4950 1550
	0    1    1    0   
$EndComp
$Comp
L Device:R R7
U 1 1 5D9FAA60
P 4950 1650
F 0 "R7" V 4900 1800 50  0000 C CNN
F 1 "22" V 4950 1650 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4880 1650 50  0001 C CNN
F 3 "~" H 4950 1650 50  0001 C CNN
	1    4950 1650
	0    1    1    0   
$EndComp
$Comp
L Device:R R8
U 1 1 5D9FADCA
P 4950 750
F 0 "R8" V 4900 900 50  0000 C CNN
F 1 "10K" V 4950 750 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4880 750 50  0001 C CNN
F 3 "~" H 4950 750 50  0001 C CNN
	1    4950 750 
	0    1    1    0   
$EndComp
Wire Wire Line
	5450 850  5100 850 
Wire Wire Line
	5450 950  5100 950 
Wire Wire Line
	5450 1050 5100 1050
Wire Wire Line
	5450 1150 5100 1150
Wire Wire Line
	5450 1250 5100 1250
Wire Wire Line
	5450 1450 5100 1450
Wire Wire Line
	5450 1550 5100 1550
Wire Wire Line
	5450 1650 5100 1650
$Comp
L Device:R R9
U 1 1 5DA0C1B5
P 4950 650
F 0 "R9" V 4900 800 50  0000 C CNN
F 1 "10K" V 4950 650 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4880 650 50  0001 C CNN
F 3 "~" H 4950 650 50  0001 C CNN
	1    4950 650 
	0    1    1    0   
$EndComp
Text Label 5200 750  0    50   ~ 0
GND
Wire Wire Line
	5450 650  5100 650 
Wire Wire Line
	5450 750  5100 750 
Wire Wire Line
	4800 650  4800 750 
Connection ~ 4800 750 
$Comp
L Connector_Generic:Conn_02x10_Odd_Even J2
U 1 1 5DA19744
P 7200 1050
F 0 "J2" H 7250 1550 50  0000 C CNN
F 1 "Conn_02x10_Odd_Even" H 7250 450 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x10_P2.54mm_Vertical" H 7200 1050 50  0001 C CNN
F 3 "~" H 7200 1050 50  0001 C CNN
	1    7200 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	6750 650  7000 650 
Wire Wire Line
	6750 750  7000 750 
Wire Wire Line
	6750 850  7000 850 
Wire Wire Line
	6750 950  7000 950 
Wire Wire Line
	6750 1050 7000 1050
Wire Wire Line
	6750 1150 7000 1150
Wire Wire Line
	6750 1250 7000 1250
Wire Wire Line
	6750 1350 7000 1350
Wire Wire Line
	6750 1450 7000 1450
Wire Wire Line
	6750 1550 7000 1550
Text Label 6750 650  0    50   ~ 0
Vref
Text Label 6750 750  0    50   ~ 0
nTRST
Text Label 6750 850  0    50   ~ 0
TDI
Text Label 6750 950  0    50   ~ 0
TMS
Text Label 6750 1050 0    50   ~ 0
TCK
Text Label 6750 1150 0    50   ~ 0
RTCK
Text Label 6750 1250 0    50   ~ 0
TDO
Text Label 6750 1350 0    50   ~ 0
nRST
Wire Wire Line
	7500 650  7750 650 
Wire Wire Line
	7500 750  7750 750 
Wire Wire Line
	7500 850  7750 850 
Wire Wire Line
	7500 950  7750 950 
Wire Wire Line
	7500 1050 7750 1050
Wire Wire Line
	7500 1150 7750 1150
Wire Wire Line
	7500 1250 7750 1250
Wire Wire Line
	7500 1350 7750 1350
Wire Wire Line
	7500 1450 7750 1450
Wire Wire Line
	7500 1550 7750 1550
Text Label 7500 1450 0    50   ~ 0
GND
Text Label 7500 1550 0    50   ~ 0
GND
Text Label 7500 650  0    50   ~ 0
Vsupply
Text Label 7500 1350 0    50   ~ 0
GND
Text Label 7500 1250 0    50   ~ 0
GND
Text Label 7500 1150 0    50   ~ 0
GND
Text Label 7500 1050 0    50   ~ 0
GND
Text Label 7500 950  0    50   ~ 0
GND
Text Label 7500 850  0    50   ~ 0
GND
Text Label 7500 750  0    50   ~ 0
GND
Text Label 6750 1450 0    50   ~ 0
TXD
Text Label 6750 1550 0    50   ~ 0
RXD
Wire Wire Line
	2600 1350 4800 1350
$Comp
L Device:R R10
U 1 1 5D976622
P 4950 1350
F 0 "R10" V 4900 1550 50  0000 C CNN
F 1 "22" V 4950 1350 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4880 1350 50  0001 C CNN
F 3 "~" H 4950 1350 50  0001 C CNN
	1    4950 1350
	0    1    1    0   
$EndComp
Wire Wire Line
	5450 1350 5100 1350
Text Label 5200 1350 0    50   ~ 0
RTCK
Text Label 2600 1350 0    50   ~ 0
PB.15
$EndSCHEMATC
