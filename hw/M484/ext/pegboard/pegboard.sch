EESchema Schematic File Version 4
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
P 1400 1650
F 0 "J0" H 1450 2150 50  0000 C CNN
F 1 "Conn_02x10_Odd_Even" H 1450 1050 50  0000 C CNN
F 2 "SimonQian:PinSocket_2x10_P2.54mm_Horizontal_2Layer" H 1400 1650 50  0001 C CNN
F 3 "~" H 1400 1650 50  0001 C CNN
	1    1400 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  1450 1200 1450
Wire Wire Line
	950  2050 1200 2050
Wire Wire Line
	950  2150 1200 2150
Wire Wire Line
	1700 1450 1950 1450
Wire Wire Line
	1700 1950 1950 1950
Wire Wire Line
	1700 2050 1950 2050
Wire Wire Line
	1700 2150 1950 2150
Text Label 950  1250 0    50   ~ 0
PB.0
Text Label 1700 1950 0    50   ~ 0
PB.15
Text Label 950  1350 0    50   ~ 0
PB.2
Text Label 950  1450 0    50   ~ 0
PB.4
Text Label 950  1550 0    50   ~ 0
PB.6
Text Label 950  1650 0    50   ~ 0
PB.8
Text Label 950  1750 0    50   ~ 0
PB.10
Text Label 950  1850 0    50   ~ 0
PB.12
Text Label 950  1950 0    50   ~ 0
PB.14
Text Label 1700 1250 0    50   ~ 0
PB.1
Text Label 1700 1350 0    50   ~ 0
PB.3
Text Label 1700 1450 0    50   ~ 0
PB.5
Text Label 1700 1550 0    50   ~ 0
PB.7
Text Label 1700 1650 0    50   ~ 0
PB.9
Text Label 1700 1750 0    50   ~ 0
PB.11
Text Label 1700 1850 0    50   ~ 0
PB.13
Text Label 950  2050 0    50   ~ 0
VCC
Text Label 950  2150 0    50   ~ 0
VCC5V
Text Label 1700 2050 0    50   ~ 0
GND
Text Label 1700 2150 0    50   ~ 0
GND
Wire Wire Line
	1700 1850 1950 1850
Wire Wire Line
	1200 1250 950  1250
Wire Wire Line
	1200 1350 950  1350
Wire Wire Line
	1200 1550 950  1550
Wire Wire Line
	1200 1650 950  1650
Wire Wire Line
	1200 1750 950  1750
Wire Wire Line
	1200 1850 950  1850
Wire Wire Line
	1200 1950 950  1950
Wire Wire Line
	1950 1250 1700 1250
Wire Wire Line
	1950 1350 1700 1350
Wire Wire Line
	1950 1550 1700 1550
Wire Wire Line
	1950 1650 1700 1650
Wire Wire Line
	1950 1750 1700 1750
$Comp
L Connector_Generic:Conn_01x20 J1
U 1 1 5E5BE297
P 2650 1600
F 0 "J1" H 2730 1592 50  0000 L CNN
F 1 "Conn_01x20" H 2730 1501 50  0000 L CNN
F 2 "Connector_PinHeader_1.27mm:PinHeader_1x20_P1.27mm_Vertical" H 2650 1600 50  0001 C CNN
F 3 "~" H 2650 1600 50  0001 C CNN
	1    2650 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 1100 2450 1100
Text Label 2200 700  0    50   ~ 0
PB.0
Text Label 2200 900  0    50   ~ 0
PB.2
Text Label 2200 1100 0    50   ~ 0
PB.4
Text Label 2200 1300 0    50   ~ 0
PB.6
Text Label 2200 1500 0    50   ~ 0
PB.8
Text Label 2200 1700 0    50   ~ 0
PB.10
Text Label 2200 1900 0    50   ~ 0
PB.12
Text Label 2200 2100 0    50   ~ 0
PB.14
Wire Wire Line
	2450 700  2200 700 
Wire Wire Line
	2450 900  2200 900 
Wire Wire Line
	2450 1300 2200 1300
Wire Wire Line
	2450 1500 2200 1500
Wire Wire Line
	2450 1700 2200 1700
Wire Wire Line
	2450 1900 2200 1900
Wire Wire Line
	2450 2100 2200 2100
Wire Wire Line
	2200 1200 2450 1200
Wire Wire Line
	2200 2200 2450 2200
Text Label 2200 2200 0    50   ~ 0
PB.15
Text Label 2200 1400 0    50   ~ 0
PB.7
Text Label 2200 1600 0    50   ~ 0
PB.9
Text Label 2200 1800 0    50   ~ 0
PB.11
Text Label 2200 2000 0    50   ~ 0
PB.13
Wire Wire Line
	2200 2000 2450 2000
Wire Wire Line
	2450 800  2200 800 
Wire Wire Line
	2450 1000 2200 1000
Wire Wire Line
	2450 1400 2200 1400
Wire Wire Line
	2450 1600 2200 1600
Wire Wire Line
	2450 1800 2200 1800
Text Label 2200 1200 0    50   ~ 0
PB.5
Text Label 2200 1000 0    50   ~ 0
PB.3
Text Label 2200 800  0    50   ~ 0
PB.1
Wire Wire Line
	2200 2300 2450 2300
Wire Wire Line
	2200 2500 2450 2500
Text Label 2200 2300 0    50   ~ 0
VCC
Text Label 2200 2500 0    50   ~ 0
VCC5V
Wire Wire Line
	2200 2400 2450 2400
Wire Wire Line
	2200 2600 2450 2600
Text Label 2200 2400 0    50   ~ 0
GND
Text Label 2200 2600 0    50   ~ 0
GND
$EndSCHEMATC
