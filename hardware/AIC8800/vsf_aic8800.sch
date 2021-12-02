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
L simonqian:Module_AIC8800M U0
U 1 1 61927BB3
P 9250 3850
F 0 "U0" H 9250 5165 50  0000 C CNN
F 1 "Module_AIC8800M" H 9250 5074 50  0000 C CNN
F 2 "simonqian:AIC8800M_M1105" H 9250 3850 50  0001 C CNN
F 3 "" H 9250 3850 50  0001 C CNN
	1    9250 3850
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW0
U 1 1 6192FDBE
P 6750 4725
F 0 "SW0" H 6650 4825 50  0000 C CNN
F 1 "SW_Push" H 6750 4919 50  0000 C CNN
F 2 "simonqian:Key_2Pin_2.9X4.3" H 6750 4925 50  0001 C CNN
F 3 "~" H 6750 4925 50  0001 C CNN
	1    6750 4725
	1    0    0    -1  
$EndComp
$Comp
L Connector:USB_C_Receptacle_USB2.0 J0
U 1 1 61930A3C
P 5950 3450
F 0 "J0" H 6057 4317 50  0000 C CNN
F 1 "USB_C_Receptacle_USB2.0" H 5950 4200 50  0000 C CNN
F 2 "simonqian:USB_C_Receptacle_Palconn_UTC16-G_DOWN1.6" H 6100 3450 50  0001 C CNN
F 3 "https://www.usb.org/sites/default/files/documents/usb_type-c.zip" H 6100 3450 50  0001 C CNN
	1    5950 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R0
U 1 1 61931FC8
P 6700 3050
F 0 "R0" V 6650 3200 50  0000 C CNN
F 1 "5.1K" V 6700 3050 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6630 3050 50  0001 C CNN
F 3 "~" H 6700 3050 50  0001 C CNN
	1    6700 3050
	0    1    1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 61932772
P 6700 3150
F 0 "R1" V 6650 3300 50  0000 C CNN
F 1 "5.1K" V 6700 3150 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6630 3150 50  0001 C CNN
F 3 "~" H 6700 3150 50  0001 C CNN
	1    6700 3150
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 61932AC8
P 6700 3350
F 0 "R2" V 6650 3500 50  0000 C CNN
F 1 "22" V 6700 3350 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6630 3350 50  0001 C CNN
F 3 "~" H 6700 3350 50  0001 C CNN
	1    6700 3350
	0    1    1    0   
$EndComp
$Comp
L Device:R R3
U 1 1 61932EDC
P 6700 3550
F 0 "R3" V 6650 3700 50  0000 C CNN
F 1 "22" V 6700 3550 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6630 3550 50  0001 C CNN
F 3 "~" H 6700 3550 50  0001 C CNN
	1    6700 3550
	0    1    1    0   
$EndComp
$Comp
L Device:C C0
U 1 1 61934127
P 7850 5850
F 0 "C0" H 7850 5925 50  0000 L CNN
F 1 "10uF" H 7875 5750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 7888 5700 50  0001 C CNN
F 3 "~" H 7850 5850 50  0001 C CNN
	1    7850 5850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 61935504
P 8775 5850
F 0 "C2" H 8775 5925 50  0000 L CNN
F 1 "10uF" H 8800 5750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 8813 5700 50  0001 C CNN
F 3 "~" H 8775 5850 50  0001 C CNN
	1    8775 5850
	1    0    0    -1  
$EndComp
$Comp
L Device:D_TVS D0
U 1 1 6195A35A
P 6700 2750
F 0 "D0" H 6550 2800 50  0000 C CNN
F 1 "LESD5Z5.0CT1G" H 6700 2900 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-523" H 6700 2750 50  0001 C CNN
F 3 "~" H 6700 2750 50  0001 C CNN
	1    6700 2750
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_NMOS_GSD Q1
U 1 1 619B6078
P 6650 5750
F 0 "Q1" H 6854 5796 50  0000 L CNN
F 1 "AO3402" H 6854 5705 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 6850 5850 50  0001 C CNN
F 3 "~" H 6650 5750 50  0001 C CNN
	1    6650 5750
	1    0    0    -1  
$EndComp
$Comp
L Device:Q_PMOS_GSD Q0
U 1 1 619B794D
P 6750 5350
F 0 "Q0" V 6725 5500 50  0000 C CNN
F 1 "AO3401" V 6975 5350 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 6950 5450 50  0001 C CNN
F 3 "~" H 6750 5350 50  0001 C CNN
	1    6750 5350
	0    1    -1   0   
$EndComp
$Comp
L Device:R R7
U 1 1 619B9059
P 6550 5400
F 0 "R7" V 6625 5300 50  0000 L CNN
F 1 "220K" V 6550 5300 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6480 5400 50  0001 C CNN
F 3 "~" H 6550 5400 50  0001 C CNN
	1    6550 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 619B9663
P 6450 5600
F 0 "R8" V 6525 5500 50  0000 L CNN
F 1 "47K" V 6450 5525 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6380 5600 50  0001 C CNN
F 3 "~" H 6450 5600 50  0001 C CNN
	1    6450 5600
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 619B99C2
P 6450 5950
F 0 "SW1" H 6525 6050 50  0000 C CNN
F 1 "SW_Push" H 6450 5875 50  0000 C CNN
F 2 "simonqian:Key_2Pin_2.9X4.3" H 6450 6150 50  0001 C CNN
F 3 "~" H 6450 6150 50  0001 C CNN
	1    6450 5950
	0    1    1    0   
$EndComp
Wire Wire Line
	5950 4350 6550 4350
Wire Wire Line
	6550 4350 6550 4050
Wire Wire Line
	6550 4050 6550 3950
Connection ~ 6550 4050
$Comp
L power:GND #PWR0101
U 1 1 619506DC
P 5950 4350
F 0 "#PWR0101" H 5950 4100 50  0001 C CNN
F 1 "GND" H 5955 4177 50  0000 C CNN
F 2 "" H 5950 4350 50  0001 C CNN
F 3 "" H 5950 4350 50  0001 C CNN
	1    5950 4350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 61950CF7
P 6850 2750
F 0 "#PWR0102" H 6850 2500 50  0001 C CNN
F 1 "GND" V 6855 2622 50  0000 R CNN
F 2 "" H 6850 2750 50  0001 C CNN
F 3 "" H 6850 2750 50  0001 C CNN
	1    6850 2750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6550 3450 6550 3350
Connection ~ 6550 3350
Wire Wire Line
	6550 3650 6550 3550
Connection ~ 6550 3550
Wire Wire Line
	6550 2850 6550 2750
Text Label 6900 3350 0    50   ~ 0
USB_DM
Wire Wire Line
	6850 3350 7200 3350
$Comp
L power:GND #PWR0103
U 1 1 6195C777
P 7000 3050
F 0 "#PWR0103" H 7000 2800 50  0001 C CNN
F 1 "GND" V 7005 2922 50  0000 R CNN
F 2 "" H 7000 3050 50  0001 C CNN
F 3 "" H 7000 3050 50  0001 C CNN
	1    7000 3050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6850 3050 7000 3050
Wire Wire Line
	6850 3150 7000 3150
Wire Wire Line
	7000 3150 7000 3050
Connection ~ 7000 3050
Text Label 6900 3550 0    50   ~ 0
USB_DP
Wire Wire Line
	6850 3550 7200 3550
Text Label 6900 2850 0    50   ~ 0
USB_5V
Wire Wire Line
	6550 2850 7200 2850
Connection ~ 6550 2850
$Comp
L Device:R R5
U 1 1 61978343
P 6750 4825
F 0 "R5" V 6800 5000 50  0000 C CNN
F 1 "NC" V 6750 4825 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6680 4825 50  0001 C CNN
F 3 "~" H 6750 4825 50  0001 C CNN
	1    6750 4825
	0    1    1    0   
$EndComp
$Comp
L Device:R R6
U 1 1 619794DF
P 6400 4725
F 0 "R6" V 6475 4775 50  0000 C CNN
F 1 "47K" V 6400 4725 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6330 4725 50  0001 C CNN
F 3 "~" H 6400 4725 50  0001 C CNN
	1    6400 4725
	0    1    1    0   
$EndComp
Wire Wire Line
	6600 4825 6550 4825
Wire Wire Line
	6550 4825 6550 4725
Connection ~ 6550 4725
Wire Wire Line
	6900 4825 6950 4825
Wire Wire Line
	6950 4825 6950 4725
Text Label 6050 4725 0    50   ~ 0
PKEY
$Comp
L power:GND #PWR0104
U 1 1 619AF3D2
P 6750 5950
F 0 "#PWR0104" H 6750 5700 50  0001 C CNN
F 1 "GND" H 6755 5777 50  0000 C CNN
F 2 "" H 6750 5950 50  0001 C CNN
F 3 "" H 6750 5950 50  0001 C CNN
	1    6750 5950
	1    0    0    -1  
$EndComp
Text Label 5925 5250 0    50   ~ 0
USB_5V
$Comp
L power:GND #PWR0105
U 1 1 619BA026
P 6450 6150
F 0 "#PWR0105" H 6450 5900 50  0001 C CNN
F 1 "GND" H 6455 5977 50  0000 C CNN
F 2 "" H 6450 6150 50  0001 C CNN
F 3 "" H 6450 6150 50  0001 C CNN
	1    6450 6150
	1    0    0    -1  
$EndComp
Text Label 7850 5700 0    50   ~ 0
VSYS
$Comp
L Device:CP1 C1
U 1 1 619C2EA6
P 8075 5850
F 0 "C1" H 8075 5925 50  0000 L CNN
F 1 "100uF" H 8100 5750 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 8075 5850 50  0001 C CNN
F 3 "~" H 8075 5850 50  0001 C CNN
	1    8075 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8075 5700 7850 5700
Wire Wire Line
	8075 6000 7850 6000
$Comp
L power:GND #PWR0106
U 1 1 619C4AEC
P 7850 6000
F 0 "#PWR0106" H 7850 5750 50  0001 C CNN
F 1 "GND" H 7855 5827 50  0000 C CNN
F 2 "" H 7850 6000 50  0001 C CNN
F 3 "" H 7850 6000 50  0001 C CNN
	1    7850 6000
	1    0    0    -1  
$EndComp
Connection ~ 7850 6000
$Comp
L power:GND #PWR0107
U 1 1 619DDF19
P 8775 6000
F 0 "#PWR0107" H 8775 5750 50  0001 C CNN
F 1 "GND" H 8780 5827 50  0000 C CNN
F 2 "" H 8775 6000 50  0001 C CNN
F 3 "" H 8775 6000 50  0001 C CNN
	1    8775 6000
	1    0    0    -1  
$EndComp
Text Label 8775 5700 0    50   ~ 0
VCC
$Comp
L Connector_Generic:Conn_01x20 J2
U 1 1 619EA227
P 11000 3800
F 0 "J2" H 10900 4925 50  0000 L CNN
F 1 "Conn_01x20" H 10700 4825 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x20_P2.54mm_Vertical" H 11000 3800 50  0001 C CNN
F 3 "~" H 11000 3800 50  0001 C CNN
	1    11000 3800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x20 J1
U 1 1 619EC284
P 7425 3825
F 0 "J1" H 7343 4942 50  0000 C CNN
F 1 "Conn_01x20" H 7343 4851 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x20_P2.54mm_Vertical" H 7425 3825 50  0001 C CNN
F 3 "~" H 7425 3825 50  0001 C CNN
	1    7425 3825
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8200 4450 8400 4450
Text Label 8400 4450 2    50   ~ 0
VCC
Text Label 8400 4350 2    50   ~ 0
V18
Wire Wire Line
	8650 5200 8650 5000
Text Label 8650 5000 3    50   ~ 0
V33
$Comp
L Device:R R10
U 1 1 619FF3C4
P 8250 4700
F 0 "R10" V 8150 4700 50  0000 C CNN
F 1 "0" V 8250 4700 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 8180 4700 50  0001 C CNN
F 3 "~" H 8250 4700 50  0001 C CNN
	1    8250 4700
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R9
U 1 1 61A00BC0
P 8250 4575
F 0 "R9" V 8325 4575 50  0000 C CNN
F 1 "NC" V 8250 4575 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 8180 4575 50  0001 C CNN
F 3 "~" H 8250 4575 50  0001 C CNN
	1    8250 4575
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8400 4700 8400 4575
Wire Wire Line
	8400 4575 8400 4450
Connection ~ 8400 4575
Connection ~ 8400 4450
Wire Wire Line
	8100 4575 8100 4350
Wire Wire Line
	8100 4350 8400 4350
Wire Wire Line
	8650 5200 8100 5200
Wire Wire Line
	8100 5200 8100 4700
Text Label 6950 4725 0    50   ~ 0
VSYS
Wire Wire Line
	7150 4725 6950 4725
Connection ~ 6950 4725
Text Label 6950 5250 0    50   ~ 0
VSYS
Wire Wire Line
	7150 5250 6950 5250
Text Label 8400 4250 2    50   ~ 0
VSYS
Wire Wire Line
	8200 4250 8400 4250
Text Label 8400 4150 2    50   ~ 0
V13
Wire Wire Line
	8200 4150 8400 4150
Wire Wire Line
	6050 4725 6250 4725
Text Label 6200 5750 0    50   ~ 0
PRST
Text Label 8400 4050 2    50   ~ 0
PKEY
Wire Wire Line
	8200 4050 8400 4050
Wire Wire Line
	8200 3950 8400 3950
Wire Wire Line
	8200 3850 8400 3850
Wire Wire Line
	8200 3750 8400 3750
Wire Wire Line
	8200 3650 8400 3650
Wire Wire Line
	8200 3550 8400 3550
Wire Wire Line
	8200 3450 8400 3450
Wire Wire Line
	8200 3350 8400 3350
Text Label 8400 3350 2    50   ~ 0
USB_DM
Text Label 8400 3450 2    50   ~ 0
USB_DP
Text Label 8400 3550 2    50   ~ 0
B13
Text Label 8400 3650 2    50   ~ 0
B0
Text Label 8400 3750 2    50   ~ 0
B1
Text Label 8400 3850 2    50   ~ 0
B2
Text Label 8400 3950 2    50   ~ 0
B3
Wire Wire Line
	8750 5200 8750 5000
Text Label 8750 5000 3    50   ~ 0
B4
Wire Wire Line
	8850 5200 8850 5000
Wire Wire Line
	8950 5200 8950 5000
Wire Wire Line
	9050 5200 9050 5000
Wire Wire Line
	9150 5200 9150 5000
Wire Wire Line
	9250 5200 9250 5000
Wire Wire Line
	9350 5200 9350 5000
Wire Wire Line
	9450 5200 9450 5000
Wire Wire Line
	9550 5200 9550 5000
Wire Wire Line
	9650 5200 9650 5000
Wire Wire Line
	9750 5200 9750 5000
Wire Wire Line
	9850 5200 9850 5000
Text Label 8850 5000 3    50   ~ 0
B5
Text Label 8950 5000 3    50   ~ 0
B6
Text Label 9050 5000 3    50   ~ 0
B7
Text Label 9150 5000 3    50   ~ 0
V09
Text Label 9250 5000 3    50   ~ 0
A15
Text Label 9350 5000 3    50   ~ 0
A14
Text Label 9450 5000 3    50   ~ 0
A13
Text Label 9550 5000 3    50   ~ 0
A12
Text Label 9650 5000 3    50   ~ 0
A11
Text Label 9750 5000 3    50   ~ 0
A10
Text Label 9850 5000 3    50   ~ 0
A9
Wire Wire Line
	10300 4450 10100 4450
Wire Wire Line
	10300 4350 10100 4350
Wire Wire Line
	10300 4250 10100 4250
Wire Wire Line
	10300 4150 10100 4150
Wire Wire Line
	10300 4050 10100 4050
Wire Wire Line
	10300 3950 10100 3950
Wire Wire Line
	10300 3850 10100 3850
Wire Wire Line
	10300 3750 10100 3750
Wire Wire Line
	10300 3650 10100 3650
Text Label 10100 4450 0    50   ~ 0
A8
Text Label 10100 4350 0    50   ~ 0
A7
Text Label 10100 4250 0    50   ~ 0
A6
Text Label 10100 4150 0    50   ~ 0
A5
Text Label 10100 4050 0    50   ~ 0
A4
Text Label 10100 3950 0    50   ~ 0
A3
Text Label 10100 3850 0    50   ~ 0
A2
Text Label 10100 3750 0    50   ~ 0
A1
Text Label 10100 3650 0    50   ~ 0
A0
$Comp
L Connector_Generic:Conn_02x05_Counter_Clockwise J3
U 1 1 61A5A6BD
P 9975 5925
F 0 "J3" H 10025 6342 50  0000 C CNN
F 1 "Conn_02x05_Counter_Clockwise" H 10025 6251 50  0000 C CNN
F 2 "simonqian:PinHeader_2x05_P2.54mm_Vertical_SMD_Clockwise" H 9975 5925 50  0001 C CNN
F 3 "~" H 9975 5925 50  0001 C CNN
	1    9975 5925
	1    0    0    -1  
$EndComp
Wire Wire Line
	9575 5725 9775 5725
Wire Wire Line
	9575 6025 9775 6025
Wire Wire Line
	9575 6125 9775 6125
Wire Wire Line
	10275 5725 10475 5725
Wire Wire Line
	10275 6025 10475 6025
Wire Wire Line
	10275 6125 10475 6125
Text Label 10275 5725 0    50   ~ 0
VCC
Text Label 10275 6025 0    50   ~ 0
A8
Text Label 10275 6125 0    50   ~ 0
A9
$Comp
L Device:R R11
U 1 1 61A7B583
P 10700 5825
F 0 "R11" V 10775 5825 50  0000 C CNN
F 1 "NC" V 10700 5825 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 10630 5825 50  0001 C CNN
F 3 "~" H 10700 5825 50  0001 C CNN
	1    10700 5825
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R12
U 1 1 61A7C5F9
P 10700 5925
F 0 "R12" V 10650 5750 50  0000 C CNN
F 1 "NC" V 10700 5925 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 10630 5925 50  0001 C CNN
F 3 "~" H 10700 5925 50  0001 C CNN
	1    10700 5925
	0    -1   -1   0   
$EndComp
Wire Wire Line
	11050 5825 10850 5825
Wire Wire Line
	11050 5925 10850 5925
Text Label 10850 5825 0    50   ~ 0
B6
Text Label 10850 5925 0    50   ~ 0
B7
Text Label 9775 5825 2    50   ~ 0
A1
Text Label 9775 5925 2    50   ~ 0
A0
Text Label 9775 6025 2    50   ~ 0
PRST
Text Label 9375 5825 0    50   ~ 0
SWDIO
Wire Wire Line
	9375 5825 9775 5825
Text Label 9375 5925 0    50   ~ 0
SWCLK
Wire Wire Line
	9375 5925 9775 5925
Text Label 10475 6025 2    50   ~ 0
RX
Wire Wire Line
	7825 2925 7625 2925
Wire Wire Line
	8200 3250 8400 3250
Text Label 8400 3250 2    50   ~ 0
GND
Wire Wire Line
	10300 3550 10100 3550
Text Label 10100 3550 0    50   ~ 0
GND
Wire Wire Line
	10300 3250 10100 3250
Text Label 10100 3250 0    50   ~ 0
GND
Wire Wire Line
	10300 3100 10100 3100
Text Label 10100 3100 0    50   ~ 0
GND
Wire Wire Line
	7825 3025 7625 3025
Wire Wire Line
	7825 3125 7625 3125
Wire Wire Line
	7825 3225 7625 3225
Wire Wire Line
	7825 3325 7625 3325
Wire Wire Line
	7825 3425 7625 3425
Wire Wire Line
	7825 3525 7625 3525
Wire Wire Line
	7825 3625 7625 3625
Wire Wire Line
	7825 3725 7625 3725
Wire Wire Line
	7825 3825 7625 3825
Wire Wire Line
	7825 3925 7625 3925
Wire Wire Line
	7825 4025 7625 4025
Wire Wire Line
	7825 4125 7625 4125
Wire Wire Line
	7825 4225 7625 4225
Wire Wire Line
	7825 4325 7625 4325
Wire Wire Line
	7825 4425 7625 4425
Wire Wire Line
	7825 4525 7625 4525
Wire Wire Line
	7825 4625 7625 4625
Wire Wire Line
	7825 4725 7625 4725
Wire Wire Line
	7825 4825 7625 4825
Wire Wire Line
	10800 2900 10600 2900
Wire Wire Line
	10800 3000 10600 3000
Wire Wire Line
	10800 3100 10600 3100
Wire Wire Line
	10800 3200 10600 3200
Wire Wire Line
	10800 3300 10600 3300
Wire Wire Line
	10800 3400 10600 3400
Wire Wire Line
	10800 3500 10600 3500
Wire Wire Line
	10800 3600 10600 3600
Wire Wire Line
	10800 3700 10600 3700
Wire Wire Line
	10800 3800 10600 3800
Wire Wire Line
	10800 3900 10600 3900
Wire Wire Line
	10800 4000 10600 4000
Wire Wire Line
	10800 4100 10600 4100
Wire Wire Line
	10800 4200 10600 4200
Wire Wire Line
	10800 4300 10600 4300
Wire Wire Line
	10800 4400 10600 4400
Wire Wire Line
	10800 4500 10600 4500
Wire Wire Line
	10800 4600 10600 4600
Wire Wire Line
	10800 4700 10600 4700
Wire Wire Line
	10800 4800 10600 4800
$Comp
L Device:LED D1
U 1 1 61B49A20
P 7600 5725
F 0 "D1" V 7700 5725 50  0000 R CNN
F 1 "LED" V 7700 5875 50  0000 R CNN
F 2 "LED_SMD:LED_0402_1005Metric" H 7600 5725 50  0001 C CNN
F 3 "~" H 7600 5725 50  0001 C CNN
	1    7600 5725
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R13
U 1 1 61B51703
P 7600 6025
F 0 "R13" V 7675 6025 50  0000 C CNN
F 1 "4.7K" V 7600 6025 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 7530 6025 50  0001 C CNN
F 3 "~" H 7600 6025 50  0001 C CNN
	1    7600 6025
	-1   0    0    1   
$EndComp
Wire Wire Line
	7600 6175 7750 6175
Wire Wire Line
	7750 6175 7750 6000
Wire Wire Line
	7750 6000 7850 6000
Wire Wire Line
	7600 5575 7750 5575
Wire Wire Line
	7750 5575 7750 5700
Wire Wire Line
	7750 5700 7850 5700
Connection ~ 7850 5700
$Comp
L Device:LED D2
U 1 1 61B5E9BF
P 8575 5725
F 0 "D2" V 8675 5725 50  0000 R CNN
F 1 "LED" V 8675 5875 50  0000 R CNN
F 2 "LED_SMD:LED_0402_1005Metric" H 8575 5725 50  0001 C CNN
F 3 "~" H 8575 5725 50  0001 C CNN
	1    8575 5725
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R14
U 1 1 61B5FC90
P 8575 6025
F 0 "R14" V 8650 6025 50  0000 C CNN
F 1 "4.7K" V 8575 6025 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 8505 6025 50  0001 C CNN
F 3 "~" H 8575 6025 50  0001 C CNN
	1    8575 6025
	-1   0    0    1   
$EndComp
Wire Wire Line
	8575 6175 8675 6175
Wire Wire Line
	8675 6175 8675 6000
Connection ~ 8775 6000
Wire Wire Line
	8575 5575 8675 5575
Wire Wire Line
	8675 5575 8675 5700
Wire Wire Line
	8675 5700 8775 5700
Text Label 7625 3225 0    50   ~ 0
GND
Text Label 7625 2925 0    50   ~ 0
USB_5V
Text Label 7625 3025 0    50   ~ 0
USB_DM
Text Label 7625 3125 0    50   ~ 0
USB_DP
Text Label 7625 3325 0    50   ~ 0
B13
Text Label 7625 3425 0    50   ~ 0
B0
Text Label 7625 3525 0    50   ~ 0
B1
Text Label 7625 3625 0    50   ~ 0
B2
Text Label 7625 3725 0    50   ~ 0
B3
Text Label 7625 3825 0    50   ~ 0
PKEY
Text Label 7625 3925 0    50   ~ 0
V13
Text Label 7625 4825 0    50   ~ 0
VSYS
Text Label 7625 4225 0    50   ~ 0
VCC
Text Label 7625 4325 0    50   ~ 0
V33
Text Label 7625 4425 0    50   ~ 0
B4
Text Label 7625 4525 0    50   ~ 0
B5
Text Label 10800 2900 2    50   ~ 0
GND
Text Label 10800 3000 2    50   ~ 0
A0
Text Label 10800 3100 2    50   ~ 0
A1
Text Label 10800 3200 2    50   ~ 0
A2
Text Label 10800 3300 2    50   ~ 0
A3
Text Label 10800 3400 2    50   ~ 0
A4
Text Label 10800 3500 2    50   ~ 0
A5
Text Label 10800 3600 2    50   ~ 0
A6
Text Label 10800 3700 2    50   ~ 0
A7
Text Label 10800 3800 2    50   ~ 0
A8
Text Label 10800 3900 2    50   ~ 0
A9
Text Label 10800 4000 2    50   ~ 0
A10
Text Label 10800 4100 2    50   ~ 0
A11
Text Label 10800 4200 2    50   ~ 0
A12
Text Label 10800 4300 2    50   ~ 0
A13
Text Label 10800 4400 2    50   ~ 0
A14
Text Label 10800 4500 2    50   ~ 0
A15
Text Label 7625 4625 0    50   ~ 0
B6
Text Label 7625 4725 0    50   ~ 0
B7
Wire Wire Line
	10475 5825 10275 5825
Text Label 10275 5825 0    50   ~ 0
GND
Text Label 10275 5925 0    50   ~ 0
DL
Text Label 10800 4700 2    50   ~ 0
DL
Text Label 10800 4800 2    50   ~ 0
PRST
Text Label 10800 4600 2    50   ~ 0
GND
Connection ~ 5950 4350
$Comp
L Device:R R4
U 1 1 61BABEE3
P 5800 4350
F 0 "R4" V 5725 4350 50  0000 C CNN
F 1 "0" V 5800 4350 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5730 4350 50  0001 C CNN
F 3 "~" H 5800 4350 50  0001 C CNN
	1    5800 4350
	0    1    1    0   
$EndComp
$Comp
L Device:Polyfuse F0
U 1 1 6195339D
P 6300 5250
F 0 "F0" V 6075 5250 50  0000 C CNN
F 1 "Polyfuse" V 6166 5250 50  0000 C CNN
F 2 "Fuse:Fuse_0805_2012Metric" H 6350 5050 50  0001 L CNN
F 3 "~" H 6300 5250 50  0001 C CNN
	1    6300 5250
	0    1    1    0   
$EndComp
Connection ~ 6550 5250
Wire Wire Line
	6550 5550 6750 5550
Connection ~ 6750 5550
Connection ~ 6450 5750
Wire Wire Line
	6450 5450 6450 5250
Wire Wire Line
	6450 5250 6550 5250
Connection ~ 6450 5250
Wire Wire Line
	5925 5250 6150 5250
Wire Wire Line
	6200 5750 6450 5750
Text Label 10475 6125 2    50   ~ 0
TX
Text Label 7625 4025 0    50   ~ 0
V18
Text Label 7625 4125 0    50   ~ 0
V09
$Comp
L Device:CP1 C3
U 1 1 619E2333
P 9000 5850
F 0 "C3" H 9000 5925 50  0000 L CNN
F 1 "100uF" H 9025 5750 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 9000 5850 50  0001 C CNN
F 3 "~" H 9000 5850 50  0001 C CNN
	1    9000 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 5700 8775 5700
Connection ~ 8775 5700
Wire Wire Line
	8675 6000 8775 6000
Wire Wire Line
	9000 6000 8775 6000
$Comp
L Device:R R15
U 1 1 61A9C7F5
P 10700 6025
F 0 "R15" V 10600 6025 50  0000 C CNN
F 1 "NC" V 10700 6025 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 10630 6025 50  0001 C CNN
F 3 "~" H 10700 6025 50  0001 C CNN
	1    10700 6025
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10850 6025 11050 6025
Text Label 10950 6025 0    50   ~ 0
GND
Wire Wire Line
	10550 6025 10550 5925
Connection ~ 10550 5925
Wire Wire Line
	10550 5825 10550 5925
Wire Wire Line
	10550 5925 10275 5925
$EndSCHEMATC
