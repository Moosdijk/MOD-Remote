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
L Switch:SW_Push SW3
U 1 1 5DF5EE64
P 5250 4250
F 0 "SW3" H 5250 4625 50  0000 C CNN
F 1 "SW_Push" H 5250 4534 50  0000 C CNN
F 2 "userLib:reset_button_2.8mm" H 5250 4443 50  0000 C CNN
F 3 "~" H 5250 4450 50  0001 C CNN
	1    5250 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 4250 4950 4350
$Comp
L Switch:SW_Push SW6
U 1 1 5DF6041A
P 5250 4350
F 0 "SW6" H 5250 4725 50  0000 C CNN
F 1 "SW_Push" H 5250 4634 50  0000 C CNN
F 2 "userLib:reset_button_2.8mm" H 5250 4543 50  0000 C CNN
F 3 "~" H 5250 4550 50  0001 C CNN
	1    5250 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 4250 5050 4250
Wire Wire Line
	5050 4350 4950 4350
Connection ~ 4950 4350
Wire Wire Line
	4950 4350 4950 4450
Wire Wire Line
	5050 4450 4950 4450
Connection ~ 4950 4450
Wire Wire Line
	4950 4450 4950 4550
$Comp
L Switch:SW_Push SW7
U 1 1 5DF60780
P 5250 4450
F 0 "SW7" H 5250 4825 50  0000 C CNN
F 1 "SW_Push" H 5250 4734 50  0000 C CNN
F 2 "userLib:reset_button_2.8mm" H 5250 4643 50  0000 C CNN
F 3 "~" H 5250 4650 50  0001 C CNN
	1    5250 4450
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J6
U 1 1 5DEF78A7
P 6300 4650
F 0 "J6" H 6328 4626 50  0000 L CNN
F 1 "Conn_01x04_Female" H 6328 4535 50  0000 L CNN
F 2 "userLib:PinHeader_2x02_P2.54mm_Vertical" H 6300 4650 50  0001 C CNN
F 3 "~" H 6300 4650 50  0001 C CNN
	1    6300 4650
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Female J5
U 1 1 5DF55E4C
P 6300 4250
F 0 "J5" H 6328 4226 50  0000 L CNN
F 1 "Conn_01x04_Female" H 6328 4135 50  0000 L CNN
F 2 "userLib:PinHeader_2x02_P2.54mm_Vertical" H 6300 4250 50  0001 C CNN
F 3 "~" H 6300 4250 50  0001 C CNN
	1    6300 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 4550 5900 4550
Wire Wire Line
	6100 4250 5950 4250
Wire Wire Line
	6100 4350 6000 4350
Wire Wire Line
	6100 4150 5900 4150
Wire Wire Line
	5900 4150 5900 4550
Connection ~ 5900 4550
Wire Wire Line
	6100 4450 6050 4450
Wire Wire Line
	5900 4550 6100 4550
Wire Wire Line
	6100 4850 6050 4850
Wire Wire Line
	6050 4850 6050 4450
Connection ~ 6050 4450
Wire Wire Line
	6050 4450 5450 4450
Wire Wire Line
	6100 4750 6000 4750
Wire Wire Line
	6000 4750 6000 4350
Connection ~ 6000 4350
Wire Wire Line
	6000 4350 5450 4350
Wire Wire Line
	6100 4650 5950 4650
Wire Wire Line
	5950 4650 5950 4250
Connection ~ 5950 4250
Wire Wire Line
	5950 4250 5450 4250
$EndSCHEMATC
