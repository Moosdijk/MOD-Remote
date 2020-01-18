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
L Connector:Conn_01x04_Female J5
U 1 1 5DF55E4C
P 6650 3700
F 0 "J5" H 6678 3676 50  0000 L CNN
F 1 "Conn_01x04_Female" H 6678 3585 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x02_P2.54mm_Vertical" H 6650 3700 50  0001 C CNN
F 3 "~" H 6650 3700 50  0001 C CNN
	1    6650 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 3900 6150 3900
Wire Wire Line
	5550 3600 5550 3700
Wire Wire Line
	6450 3600 6300 3600
Wire Wire Line
	6050 3700 6250 3700
Wire Wire Line
	6450 3800 6200 3800
Wire Wire Line
	5550 3600 5650 3600
Wire Wire Line
	5650 3700 5550 3700
Connection ~ 5550 3700
Wire Wire Line
	5550 3700 5550 3800
Wire Wire Line
	5650 3800 5550 3800
Connection ~ 5550 3800
Wire Wire Line
	5550 3800 5550 3900
$Comp
L Switch:SW_Push SW1
U 1 1 5DEEBD8B
P 5850 3600
F 0 "SW1" H 5850 3975 50  0000 C CNN
F 1 "SW_Push" H 5850 3884 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm_H5mm" H 5850 3793 50  0000 C CNN
F 3 "~" H 5850 3800 50  0001 C CNN
	1    5850 3600
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 5DEEC61C
P 5850 3700
F 0 "SW2" H 5850 4075 50  0000 C CNN
F 1 "SW_Push" H 5850 3984 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm_H5mm" H 5850 3893 50  0000 C CNN
F 3 "~" H 5850 3900 50  0001 C CNN
	1    5850 3700
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 5DEEC9D3
P 5850 3800
F 0 "SW3" H 5850 4175 50  0000 C CNN
F 1 "SW_Push" H 5850 4084 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm_H5mm" H 5850 3993 50  0000 C CNN
F 3 "~" H 5850 4000 50  0001 C CNN
	1    5850 3800
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J1
U 1 1 5DEF15B1
P 6650 4100
F 0 "J1" H 6678 4076 50  0000 L CNN
F 1 "Conn_01x04_Female" H 6678 3985 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x02_P2.54mm_Vertical" H 6650 4100 50  0001 C CNN
F 3 "~" H 6650 4100 50  0001 C CNN
	1    6650 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 3900 6150 4300
Wire Wire Line
	6150 4300 6450 4300
Connection ~ 6150 3900
Wire Wire Line
	6150 3900 6450 3900
Wire Wire Line
	6200 3800 6200 4200
Wire Wire Line
	6200 4200 6450 4200
Connection ~ 6200 3800
Wire Wire Line
	6200 3800 6050 3800
Wire Wire Line
	6250 3700 6250 4100
Wire Wire Line
	6250 4100 6450 4100
Connection ~ 6250 3700
Wire Wire Line
	6250 3700 6450 3700
Wire Wire Line
	6300 3600 6300 4000
Wire Wire Line
	6300 4000 6450 4000
Connection ~ 6300 3600
Wire Wire Line
	6300 3600 6050 3600
$EndSCHEMATC
