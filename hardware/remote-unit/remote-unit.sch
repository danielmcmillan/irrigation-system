EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Irrigation Remote Unit"
Date ""
Rev "1"
Comp "Daniel McMillan"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 500  550  0    50   ~ 0
Atmega
Connection ~ 1750 1400
Wire Wire Line
	1750 1400 1650 1400
Wire Wire Line
	1000 950  1850 950 
Wire Wire Line
	1850 1400 1850 950 
Wire Wire Line
	1750 1400 1850 1400
Wire Wire Line
	1750 1550 1750 1400
Connection ~ 600  1400
Wire Wire Line
	600  950  600  1400
Wire Wire Line
	700  950  600  950 
$Comp
L Device:C C2
U 1 1 6137C5A1
P 850 950
F 0 "C2" V 598 950 50  0000 C CNN
F 1 "100nF" V 689 950 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 888 800 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 850 950 50  0001 C CNN
F 4 "C14663" V 850 950 50  0001 C CNN "JLCPCB Part"
F 5 "" H 850 950 50  0001 C CNN "JLCPCB UP"
F 6 "" H 850 950 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 850 950 50  0001 C CNN "Purchase Link"
	1    850  950 
	0    1    1    0   
$EndComp
Wire Wire Line
	600  1850 600  2150
Connection ~ 600  1850
Wire Wire Line
	700  1850 600  1850
Wire Wire Line
	600  1400 600  1850
Wire Wire Line
	700  1400 600  1400
Wire Wire Line
	1650 1400 1000 1400
$Comp
L Device:R R1
U 1 1 6137F098
P 2800 3100
F 0 "R1" H 2870 3146 50  0000 L CNN
F 1 "10k" H 2870 3055 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 2730 3100 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 2800 3100 50  0001 C CNN
F 4 "C25744" H 2800 3100 50  0001 C CNN "JLCPCB Part"
F 5 "" H 2800 3100 50  0001 C CNN "JLCPCB UP"
F 6 "" H 2800 3100 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 2800 3100 50  0001 C CNN "Purchase Link"
	1    2800 3100
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR012
U 1 1 6137DC7A
P 2800 2950
F 0 "#PWR012" H 2800 2800 50  0001 C CNN
F 1 "+3.3V" H 2815 3123 50  0000 C CNN
F 2 "" H 2800 2950 50  0001 C CNN
F 3 "" H 2800 2950 50  0001 C CNN
	1    2800 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 3350 2800 3250
Wire Wire Line
	2300 4550 2950 4550
Connection ~ 2300 4550
Wire Wire Line
	1650 4550 2300 4550
Connection ~ 2950 3350
$Comp
L Device:C C7
U 1 1 613811FE
P 3300 3350
F 0 "C7" V 3048 3350 50  0000 C CNN
F 1 "100nF" V 3139 3350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3338 3200 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 3300 3350 50  0001 C CNN
F 4 "C14663" V 3300 3350 50  0001 C CNN "JLCPCB Part"
F 5 "" H 3300 3350 50  0001 C CNN "JLCPCB UP"
F 6 "" H 3300 3350 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 3300 3350 50  0001 C CNN "Purchase Link"
	1    3300 3350
	0    1    1    0   
$EndComp
Wire Wire Line
	3450 3350 3500 3350
Wire Wire Line
	2950 3350 2800 3350
Wire Wire Line
	2950 3600 2950 3350
Wire Wire Line
	2950 4000 2950 4550
$Comp
L Device:C C3
U 1 1 61391ACB
P 850 1400
F 0 "C3" V 598 1400 50  0000 C CNN
F 1 "100nF" V 689 1400 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 888 1250 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 850 1400 50  0001 C CNN
F 4 "C14663" V 850 1400 50  0001 C CNN "JLCPCB Part"
F 5 "" H 850 1400 50  0001 C CNN "JLCPCB UP"
F 6 "" H 850 1400 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 850 1400 50  0001 C CNN "Purchase Link"
	1    850  1400
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_SPST SW1
U 1 1 6138C16A
P 2950 3800
F 0 "SW1" V 2904 3898 50  0000 L CNN
F 1 "RESET" V 2995 3898 50  0000 L CNN
F 2 "lib:SW_SPST_THT_5.0mm" H 2950 3800 50  0001 C CNN
F 3 "https://www.farnell.com/datasheets/3176355.pdf" H 2950 3800 50  0001 C CNN
F 4 "https://au.element14.com/multicomp-pro/mc32879/switch-tactile-spst-no-50ma-though/dp/171299401" H 2950 3800 50  0001 C CNN "Purchase Link"
	1    2950 3800
	0    1    1    0   
$EndComp
Connection ~ 2800 3350
Wire Wire Line
	2250 3350 2800 3350
Wire Wire Line
	1650 1400 1650 1300
Connection ~ 1650 1400
Wire Wire Line
	1650 1550 1650 1400
$Comp
L power:GND #PWR07
U 1 1 6137B4FD
P 2300 4550
F 0 "#PWR07" H 2300 4300 50  0001 C CNN
F 1 "GND" H 2305 4377 50  0000 C CNN
F 2 "" H 2300 4550 50  0001 C CNN
F 3 "" H 2300 4550 50  0001 C CNN
	1    2300 4550
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR06
U 1 1 61379A6F
P 1650 1300
F 0 "#PWR06" H 1650 1150 50  0001 C CNN
F 1 "+3.3V" H 1665 1473 50  0000 C CNN
F 2 "" H 1650 1300 50  0001 C CNN
F 3 "" H 1650 1300 50  0001 C CNN
	1    1650 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 1850 1050 1850
$Comp
L power:GND #PWR01
U 1 1 61378696
P 600 2150
F 0 "#PWR01" H 600 1900 50  0001 C CNN
F 1 "GND" H 605 1977 50  0000 C CNN
F 2 "" H 600 2150 50  0001 C CNN
F 3 "" H 600 2150 50  0001 C CNN
	1    600  2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 2700 2800 2700
Connection ~ 2950 2700
Wire Wire Line
	2950 2650 2950 2700
Wire Wire Line
	2950 2300 3300 2300
Connection ~ 2950 2300
Wire Wire Line
	2950 2300 2950 2350
Wire Wire Line
	2800 2700 2800 2550
Wire Wire Line
	3300 2700 2950 2700
$Comp
L Device:Crystal Y1
U 1 1 613633A3
P 2950 2500
F 0 "Y1" V 2904 2631 50  0000 L CNN
F 1 "8MHz" V 2995 2631 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_5032-2Pin_5.0x3.2mm" H 2950 2500 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/2103291135_Yangxing-Tech-X50328MSB2GI_C115962.pdf" H 2950 2500 50  0001 C CNN
F 4 "C115962" H 2950 2500 50  0001 C CNN "JLCPCB Part"
F 5 "" H 2950 2500 50  0001 C CNN "JLCPCB UP"
F 6 "" H 2950 2500 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 2950 2500 50  0001 C CNN "Purchase Link"
	1    2950 2500
	0    1    1    0   
$EndComp
Wire Wire Line
	2800 2300 2950 2300
$Comp
L Device:C C9
U 1 1 6136E259
P 3450 2700
F 0 "C9" V 3198 2700 50  0000 C CNN
F 1 "22pF" V 3289 2700 50  0000 C CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 3488 2550 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811141710_FH-Guangdong-Fenghua-Advanced-Tech-0402CG220J500NT_C1555.pdf" H 3450 2700 50  0001 C CNN
F 4 "C1555" V 3450 2700 50  0001 C CNN "JLCPCB Part"
F 5 "" H 3450 2700 50  0001 C CNN "JLCPCB UP"
F 6 "" H 3450 2700 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 3450 2700 50  0001 C CNN "Purchase Link"
	1    3450 2700
	0    1    1    0   
$EndComp
$Comp
L Device:C C4
U 1 1 6136D388
P 850 1850
F 0 "C4" V 598 1850 50  0000 C CNN
F 1 "100nF" V 689 1850 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 888 1700 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 850 1850 50  0001 C CNN
F 4 "C14663" V 850 1850 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 850 1850 50  0001 C CNN "Purchase Link"
	1    850  1850
	0    1    1    0   
$EndComp
Wire Wire Line
	3850 2500 3850 2700
$Comp
L power:GND #PWR013
U 1 1 6136C5DF
P 3850 2700
F 0 "#PWR013" H 3850 2450 50  0001 C CNN
F 1 "GND" H 3855 2527 50  0000 C CNN
F 2 "" H 3850 2700 50  0001 C CNN
F 3 "" H 3850 2700 50  0001 C CNN
	1    3850 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 2500 3600 2700
Connection ~ 3600 2500
Wire Wire Line
	3850 2500 3600 2500
Wire Wire Line
	3600 2300 3600 2500
Wire Wire Line
	2800 2550 2250 2550
Wire Wire Line
	2800 2450 2800 2300
Wire Wire Line
	2250 2450 2800 2450
$Comp
L Device:C C8
U 1 1 613652C9
P 3450 2300
F 0 "C8" V 3198 2300 50  0000 C CNN
F 1 "22pF" V 3289 2300 50  0000 C CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 3488 2150 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811141710_FH-Guangdong-Fenghua-Advanced-Tech-0402CG220J500NT_C1555.pdf" H 3450 2300 50  0001 C CNN
F 4 "C1555" V 3450 2300 50  0001 C CNN "JLCPCB Part"
F 5 "" H 3450 2300 50  0001 C CNN "JLCPCB UP"
F 6 "" H 3450 2300 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 3450 2300 50  0001 C CNN "Purchase Link"
	1    3450 2300
	0    1    1    0   
$EndComp
$Comp
L MCU_Microchip_ATmega:ATmega328P-AU U1
U 1 1 6136041F
P 1650 3050
F 0 "U1" H 1650 1461 50  0000 C CNN
F 1 "ATmega328P-AU" H 1650 1370 50  0000 C CNN
F 2 "Package_QFP:TQFP-32_7x7mm_P0.8mm" H 1650 3050 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328_P%20AVR%20MCU%20with%20picoPower%20Technology%20Data%20Sheet%2040001984A.pdf" H 1650 3050 50  0001 C CNN
F 4 "C14877" H 1650 3050 50  0001 C CNN "JLCPCB Part"
F 5 "" H 1650 3050 50  0001 C CNN "JLCPCB UP"
F 6 "" H 1650 3050 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 1650 3050 50  0001 C CNN "Purchase Link"
	1    1650 3050
	1    0    0    -1  
$EndComp
Wire Notes Line
	4000 4850 500  4850
Wire Notes Line
	500  4850 500  600 
Wire Notes Line
	4000 600  4000 4850
Wire Notes Line
	500  600  4000 600 
Wire Notes Line
	8650 600  11150 600 
Text Notes 8650 550  0    50   ~ 0
Connectors
$Comp
L TPS54331DR:TPS54331DR U4
U 1 1 6138537B
P 5250 950
F 0 "U4" H 5850 1215 50  0000 C CNN
F 1 "TPS54331DR" H 5850 1124 50  0000 C CNN
F 2 "TPS54331DR:Texas_Instruments-TPS54331DR-Level_A" H 6150 600 50  0001 C CNN
F 3 "https://www.ti.com/lit/ds/symlink/tps54331.pdf" H 6250 500 50  0001 C CNN
F 4 "C9865" H 5250 950 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 5250 950 50  0001 C CNN "Purchase Link"
	1    5250 950 
	1    0    0    -1  
$EndComp
Text Notes 4050 550  0    50   ~ 0
3v3 TPS5430
$Comp
L Device:C C10
U 1 1 61394644
P 4200 1150
F 0 "C10" H 4315 1196 50  0000 L CNN
F 1 "10uF" H 4315 1105 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 4238 1000 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810221112_Samsung-Electro-Mechanics-CL31A106KBHNNNE_C13585.pdf" H 4200 1150 50  0001 C CNN
F 4 "C13585" H 4200 1150 50  0001 C CNN "JLCPCB Part"
F 5 "" H 4200 1150 50  0001 C CNN "JLCPCB UP"
F 6 "" H 4200 1150 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 4200 1150 50  0001 C CNN "Purchase Link"
	1    4200 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 850  4200 950 
NoConn ~ 5350 1150
$Comp
L power:GND #PWR015
U 1 1 613AC985
P 4200 2400
F 0 "#PWR015" H 4200 2150 50  0001 C CNN
F 1 "GND" H 4205 2227 50  0000 C CNN
F 2 "" H 4200 2400 50  0001 C CNN
F 3 "" H 4200 2400 50  0001 C CNN
	1    4200 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 1300 4200 1350
Wire Wire Line
	5350 950  4650 950 
Connection ~ 4200 950 
Wire Wire Line
	4200 950  4200 1000
$Comp
L Device:C C14
U 1 1 613CBCCA
P 5100 1350
F 0 "C14" V 4848 1350 50  0000 C CNN
F 1 "8.2nF" V 4939 1350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5138 1200 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810262007_Samsung-Electro-Mechanics-CL10B822KB8NNNC_C27920.pdf" H 5100 1350 50  0001 C CNN
F 4 "C27920" H 5100 1350 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 5100 1350 50  0001 C CNN "Purchase Link"
	1    5100 1350
	0    1    1    0   
$EndComp
Wire Wire Line
	5250 1350 5350 1350
Connection ~ 4200 1350
Wire Wire Line
	4200 1350 4200 2350
$Comp
L Device:R R4
U 1 1 613D73DD
P 5200 1800
F 0 "R4" H 5270 1846 50  0000 L CNN
F 1 "750" H 5270 1755 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5130 1800 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811081723_UNI-ROYAL-Uniroyal-Elec-0603WAF7500T5E_C23241.pdf" H 5200 1800 50  0001 C CNN
F 4 "C23241" H 5200 1800 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 5200 1800 50  0001 C CNN "Purchase Link"
	1    5200 1800
	1    0    0    -1  
$EndComp
$Comp
L Device:C C15
U 1 1 613D970A
P 5200 2150
F 0 "C15" H 5315 2196 50  0000 L CNN
F 1 "22uF 25V" H 5315 2105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5238 2000 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811151152_Samsung-Electro-Mechanics-CL21A226MAQNNNE_C45783.pdf" H 5200 2150 50  0001 C CNN
F 4 "C45783" H 5200 2150 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 5200 2150 50  0001 C CNN "Purchase Link"
	1    5200 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 1550 5200 1550
Wire Wire Line
	5200 1550 5200 1650
Wire Wire Line
	5200 1550 4650 1550
Connection ~ 5200 1550
Wire Wire Line
	4650 2350 5200 2350
Wire Wire Line
	5200 2350 5200 2300
Wire Wire Line
	4650 2350 4200 2350
Connection ~ 4650 2350
Connection ~ 4200 2350
Wire Wire Line
	4200 2350 4200 2400
Wire Wire Line
	4200 1350 4650 1350
Wire Wire Line
	4650 2050 4650 2350
Wire Wire Line
	4650 1550 4650 1750
$Comp
L Device:C C13
U 1 1 613D89DD
P 4650 1900
F 0 "C13" H 4765 1946 50  0000 L CNN
F 1 "6.8nF" H 4765 1855 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4688 1750 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811061811_FH-Guangdong-Fenghua-Advanced-Tech-0603B682K500NT_C1631.pdf" H 4650 1900 50  0001 C CNN
F 4 "C1631" H 4650 1900 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 4650 1900 50  0001 C CNN "Purchase Link"
	1    4650 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 1950 5200 2000
$Comp
L Device:C C16
U 1 1 6141E59E
P 6600 1150
F 0 "C16" V 6348 1150 50  0000 C CNN
F 1 "100nF" V 6439 1150 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6638 1000 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 6600 1150 50  0001 C CNN
F 4 "C14663" V 6600 1150 50  0001 C CNN "JLCPCB Part"
F 5 "" H 6600 1150 50  0001 C CNN "JLCPCB UP"
F 6 "" H 6600 1150 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 6600 1150 50  0001 C CNN "Purchase Link"
	1    6600 1150
	0    1    1    0   
$EndComp
Wire Wire Line
	6450 1150 6350 1150
Wire Wire Line
	6750 1150 6850 1150
Wire Wire Line
	6850 1150 6850 950 
Connection ~ 6850 950 
Wire Wire Line
	6850 950  7000 950 
Wire Wire Line
	6350 950  6850 950 
$Comp
L Device:D_Schottky D1
U 1 1 6145F7F3
P 7000 1100
F 0 "D1" V 6954 1180 50  0000 L CNN
F 1 "550mV 1A" V 7045 1180 50  0000 L CNN
F 2 "Diode_SMD:D_SMA" H 7000 1100 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/2105061432_MDD%EF%BC%88Microdiode-Electronics%EF%BC%89-SS14_C2480.pdf" H 7000 1100 50  0001 C CNN
F 4 "C2480" H 7000 1100 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 7000 1100 50  0001 C CNN "Purchase Link"
	1    7000 1100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR018
U 1 1 61461209
P 7000 1800
F 0 "#PWR018" H 7000 1550 50  0001 C CNN
F 1 "GND" H 7005 1627 50  0000 C CNN
F 2 "" H 7000 1800 50  0001 C CNN
F 3 "" H 7000 1800 50  0001 C CNN
	1    7000 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7000 1250 7000 1750
Connection ~ 7000 950 
$Comp
L Device:L L1
U 1 1 6146EEBD
P 7250 950
F 0 "L1" V 7440 950 50  0000 C CNN
F 1 "33uH" V 7349 950 50  0000 C CNN
F 2 "lib:L_8x8mm_H4mm" H 7250 950 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809140622_COILANK-INTERNATIONAL-ABG08A40M330_C253606.pdf" H 7250 950 50  0001 C CNN
F 4 "NA (Extended)" H 7250 950 50  0001 C CNN "Purchase Link"
F 5 "C253606" H 7250 950 50  0001 C CNN "JLCPCB Part"
	1    7250 950 
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7000 950  7100 950 
$Comp
L Device:R R5
U 1 1 61498598
P 7550 1150
F 0 "R5" H 7620 1196 50  0000 L CNN
F 1 "10k" H 7620 1105 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 7480 1150 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 7550 1150 50  0001 C CNN
F 4 "C25744" H 7550 1150 50  0001 C CNN "JLCPCB Part"
F 5 "" H 7550 1150 50  0001 C CNN "JLCPCB UP"
F 6 "" H 7550 1150 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 7550 1150 50  0001 C CNN "Purchase Link"
	1    7550 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 950  7550 950 
Wire Wire Line
	7550 950  7550 1000
Wire Wire Line
	6600 1750 7000 1750
Connection ~ 7000 1750
Wire Wire Line
	7000 1750 7000 1800
Wire Wire Line
	6350 1350 7550 1350
Wire Wire Line
	7550 1350 7550 1300
$Comp
L Device:R R6
U 1 1 614B25D9
P 7550 1550
F 0 "R6" H 7620 1596 50  0000 L CNN
F 1 "3k" H 7620 1505 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 7480 1550 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811081711_UNI-ROYAL-Uniroyal-Elec-0603WAF3001T5E_C4211.pdf" H 7550 1550 50  0001 C CNN
F 4 "C4211" H 7550 1550 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 7550 1550 50  0001 C CNN "Purchase Link"
	1    7550 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 1350 7550 1400
Connection ~ 7550 1350
Wire Wire Line
	7550 1700 7550 1750
$Comp
L Device:C C17
U 1 1 614C1773
P 8000 1350
F 0 "C17" H 8115 1396 50  0000 L CNN
F 1 "22uF 6.3V" H 8115 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 8038 1200 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811151138_Samsung-Electro-Mechanics-CL10A226MQ8NRNC_C59461.pdf" H 8000 1350 50  0001 C CNN
F 4 "C59461" H 8000 1350 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 8000 1350 50  0001 C CNN "Purchase Link"
	1    8000 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 950  8000 1200
Connection ~ 7550 950 
Wire Wire Line
	8000 1500 8000 1750
Connection ~ 7550 1750
$Comp
L power:+3.3V #PWR019
U 1 1 614D0516
P 8000 850
F 0 "#PWR019" H 8000 700 50  0001 C CNN
F 1 "+3.3V" H 8015 1023 50  0000 C CNN
F 2 "" H 8000 850 50  0001 C CNN
F 3 "" H 8000 850 50  0001 C CNN
	1    8000 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 850  8000 950 
Connection ~ 8000 950 
Wire Wire Line
	6350 1550 6600 1550
Wire Wire Line
	6600 1550 6600 1750
Wire Wire Line
	7000 1750 7550 1750
Wire Notes Line
	4050 600  8600 600 
Wire Notes Line
	4050 600  4050 2650
Text Notes 8050 950  0    50   ~ 0
3.47v
NoConn ~ 1050 2050
NoConn ~ 1050 2150
Wire Wire Line
	9550 2350 8850 2350
$Comp
L power:+3.3V #PWR024
U 1 1 61384461
P 9750 1100
F 0 "#PWR024" H 9750 950 50  0001 C CNN
F 1 "+3.3V" H 9765 1273 50  0000 C CNN
F 2 "" H 9750 1100 50  0001 C CNN
F 3 "" H 9750 1100 50  0001 C CNN
	1    9750 1100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Male J3
U 1 1 6137F4B1
P 9400 900
F 0 "J3" V 9200 800 50  0000 L CNN
F 1 "ICSP" V 9300 600 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 9400 900 50  0001 C CNN
F 3 "~" H 9400 900 50  0001 C CNN
F 4 "https://au.element14.com/amphenol-icc/77313-801-20lf/conn-header-20pos-2row-2-54mm/dp/3728842" H 9400 900 50  0001 C CNN "Purchase Link"
	1    9400 900 
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x07_Male J2
U 1 1 6138A907
P 9350 1750
F 0 "J2" V 9185 1728 50  0000 C CNN
F 1 "RF Module" V 9276 1728 50  0000 C CNN
F 2 "lib:YL_800T" H 9350 1750 50  0001 C CNN
F 3 "~" H 9350 1750 50  0001 C CNN
F 4 "https://au.element14.com/amphenol-icc/10129378-920003blf/conn-header-20pos-1row-2-54mm/dp/3728430" H 9350 1750 50  0001 C CNN "Purchase Link"
	1    9350 1750
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x06_Male J7
U 1 1 613922D9
P 10550 900
F 0 "J7" V 10385 878 50  0000 C CNN
F 1 "UART" V 10476 878 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 10550 900 50  0001 C CNN
F 3 "~" H 10550 900 50  0001 C CNN
F 4 "https://au.element14.com/amphenol-icc/77313-801-20lf/conn-header-20pos-2row-2-54mm/dp/3728842" H 10550 900 50  0001 C CNN "Purchase Link"
	1    10550 900 
	0    1    1    0   
$EndComp
Wire Wire Line
	10050 1150 10250 1150
Wire Wire Line
	10250 1150 10250 1100
Wire Wire Line
	10750 1100 10950 1100
Wire Wire Line
	9650 1950 9650 2050
Wire Wire Line
	9650 2050 9750 2050
Wire Wire Line
	9550 1950 9550 2350
$Comp
L power:GND #PWR032
U 1 1 614A1B8E
P 10950 1200
F 0 "#PWR032" H 10950 950 50  0001 C CNN
F 1 "GND" H 10955 1027 50  0000 C CNN
F 2 "" H 10950 1200 50  0001 C CNN
F 3 "" H 10950 1200 50  0001 C CNN
	1    10950 1200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR023
U 1 1 614A3FA6
P 9000 1250
F 0 "#PWR023" H 9000 1000 50  0001 C CNN
F 1 "GND" H 9005 1077 50  0000 C CNN
F 2 "" H 9000 1250 50  0001 C CNN
F 3 "" H 9000 1250 50  0001 C CNN
	1    9000 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	10950 1100 10950 1200
$Comp
L power:+3.3V #PWR027
U 1 1 614A8723
P 10050 1050
F 0 "#PWR027" H 10050 900 50  0001 C CNN
F 1 "+3.3V" H 10065 1223 50  0000 C CNN
F 2 "" H 10050 1050 50  0001 C CNN
F 3 "" H 10050 1050 50  0001 C CNN
	1    10050 1050
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR020
U 1 1 614A9053
P 8850 2250
F 0 "#PWR020" H 8850 2100 50  0001 C CNN
F 1 "+3.3V" H 8865 2423 50  0000 C CNN
F 2 "" H 8850 2250 50  0001 C CNN
F 3 "" H 8850 2250 50  0001 C CNN
	1    8850 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8850 2350 8850 2250
Wire Wire Line
	10050 1150 10050 1050
Wire Wire Line
	9750 2050 9750 2150
$Comp
L power:GND #PWR025
U 1 1 61384F23
P 9750 2150
F 0 "#PWR025" H 9750 1900 50  0001 C CNN
F 1 "GND" H 9755 1977 50  0000 C CNN
F 2 "" H 9750 2150 50  0001 C CNN
F 3 "" H 9750 2150 50  0001 C CNN
	1    9750 2150
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG04
U 1 1 61923EF8
P 10850 2100
F 0 "#FLG04" H 10850 2175 50  0001 C CNN
F 1 "PWR_FLAG" H 10850 2273 50  0000 C CNN
F 2 "" H 10850 2100 50  0001 C CNN
F 3 "~" H 10850 2100 50  0001 C CNN
	1    10850 2100
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x02_Male J6
U 1 1 61926C7F
P 10400 1750
F 0 "J6" V 10235 1728 50  0000 C CNN
F 1 "PWR 12V" V 10326 1728 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 10400 1750 50  0001 C CNN
F 3 "~" H 10400 1750 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 10400 1750 50  0001 C CNN "Purchase Link"
	1    10400 1750
	0    1    1    0   
$EndComp
Wire Wire Line
	10400 1950 10400 2050
$Comp
L power:GND #PWR028
U 1 1 61962CBA
P 10050 2050
F 0 "#PWR028" H 10050 1800 50  0001 C CNN
F 1 "GND" H 10055 1877 50  0000 C CNN
F 2 "" H 10050 2050 50  0001 C CNN
F 3 "" H 10050 2050 50  0001 C CNN
	1    10050 2050
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG03
U 1 1 619D5328
P 10050 1950
F 0 "#FLG03" H 10050 2025 50  0001 C CNN
F 1 "PWR_FLAG" H 10050 2123 50  0000 C CNN
F 2 "" H 10050 1950 50  0001 C CNN
F 3 "~" H 10050 1950 50  0001 C CNN
	1    10050 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	10050 1950 10050 2050
Wire Wire Line
	10300 1950 10300 2050
Wire Wire Line
	10300 2050 10050 2050
Connection ~ 10050 2050
$Comp
L power:PWR_FLAG #FLG01
U 1 1 619FC90F
P 6850 850
F 0 "#FLG01" H 6850 925 50  0001 C CNN
F 1 "PWR_FLAG" H 6850 1023 50  0000 C CNN
F 2 "" H 6850 850 50  0001 C CNN
F 3 "~" H 6850 850 50  0001 C CNN
	1    6850 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	6850 950  6850 850 
$Comp
L power:PWR_FLAG #FLG02
U 1 1 61A9CB97
P 7550 850
F 0 "#FLG02" H 7550 925 50  0001 C CNN
F 1 "PWR_FLAG" H 7550 1023 50  0000 C CNN
F 2 "" H 7550 850 50  0001 C CNN
F 3 "~" H 7550 850 50  0001 C CNN
	1    7550 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 850  7550 950 
$Comp
L Device:C C12
U 1 1 614F0AA0
P 4650 1150
F 0 "C12" H 4765 1196 50  0000 L CNN
F 1 "10uF" H 4765 1105 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 4688 1000 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810221112_Samsung-Electro-Mechanics-CL31A106KBHNNNE_C13585.pdf" H 4650 1150 50  0001 C CNN
F 4 "C13585" H 4650 1150 50  0001 C CNN "JLCPCB Part"
F 5 "" H 4650 1150 50  0001 C CNN "JLCPCB UP"
F 6 "" H 4650 1150 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 4650 1150 50  0001 C CNN "Purchase Link"
	1    4650 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 1000 4650 950 
Connection ~ 4650 950 
Wire Wire Line
	4650 950  4200 950 
Wire Wire Line
	4650 1300 4650 1350
Connection ~ 4650 1350
Wire Wire Line
	4650 1350 4950 1350
Wire Wire Line
	9500 1100 9500 1450
Wire Wire Line
	9500 1450 9750 1450
Wire Wire Line
	9750 1450 9750 1100
Wire Wire Line
	9000 1250 9000 1150
Wire Wire Line
	9000 1150 9100 1150
Wire Wire Line
	9100 1150 9100 1100
$Comp
L Mechanical:MountingHole_Pad H1
U 1 1 61A279E9
P 9100 4300
F 0 "H1" H 9200 4349 50  0000 L CNN
F 1 "MountingHole_Pad" H 9200 4258 50  0000 L CNN
F 2 "MountingHole:MountingHole_3mm_Pad" H 9100 4300 50  0001 C CNN
F 3 "~" H 9100 4300 50  0001 C CNN
F 4 "NA" H 9100 4300 50  0001 C CNN "Purchase Link"
	1    9100 4300
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H3
U 1 1 61A28377
P 10000 4300
F 0 "H3" H 10100 4349 50  0000 L CNN
F 1 "MountingHole_Pad" H 10100 4258 50  0000 L CNN
F 2 "MountingHole:MountingHole_3mm_Pad" H 10000 4300 50  0001 C CNN
F 3 "~" H 10000 4300 50  0001 C CNN
F 4 "NA" H 10000 4300 50  0001 C CNN "Purchase Link"
	1    10000 4300
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 61A289B4
P 9100 4500
F 0 "H2" H 9000 4457 50  0000 R CNN
F 1 "MountingHole_Pad" H 9000 4548 50  0000 R CNN
F 2 "MountingHole:MountingHole_3mm_Pad" H 9100 4500 50  0001 C CNN
F 3 "~" H 9100 4500 50  0001 C CNN
F 4 "NA" H 9100 4500 50  0001 C CNN "Purchase Link"
	1    9100 4500
	-1   0    0    1   
$EndComp
$Comp
L Mechanical:MountingHole_Pad H4
U 1 1 61A292A3
P 10000 4500
F 0 "H4" H 9900 4457 50  0000 R CNN
F 1 "MountingHole_Pad" H 9900 4548 50  0000 R CNN
F 2 "MountingHole:MountingHole_3mm_Pad" H 10000 4500 50  0001 C CNN
F 3 "~" H 10000 4500 50  0001 C CNN
F 4 "NA" H 10000 4500 50  0001 C CNN "Purchase Link"
	1    10000 4500
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR021
U 1 1 61A29698
P 8900 4500
F 0 "#PWR021" H 8900 4250 50  0001 C CNN
F 1 "GND" H 8905 4327 50  0000 C CNN
F 2 "" H 8900 4500 50  0001 C CNN
F 3 "" H 8900 4500 50  0001 C CNN
	1    8900 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 4500 8900 4400
Wire Wire Line
	8900 4400 9100 4400
Connection ~ 9100 4400
Wire Wire Line
	9100 4400 10000 4400
Connection ~ 10000 4400
$Comp
L Device:Fuse F1
U 1 1 61A87884
P 10650 2050
F 0 "F1" V 10453 2050 50  0000 C CNN
F 1 "3.15A" V 10544 2050 50  0000 C CNN
F 2 "Inductor_THT:L_Radial_D8.7mm_P5.00mm_Fastron_07HCP" V 10580 2050 50  0001 C CNN
F 3 "https://au.mouser.com/datasheet/2/643/ds-CP-mrf-series-1313120.pdf" H 10650 2050 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/Bel-Fuse/MRF-315?qs=MvPYbBWWJyscnj%252BLL2OpDA%3D%3D" H 10650 2050 50  0001 C CNN "Purchase Link"
	1    10650 2050
	0    1    1    0   
$EndComp
Wire Wire Line
	10850 1950 10850 2050
Wire Wire Line
	10850 2050 10800 2050
Wire Wire Line
	10500 2050 10400 2050
Wire Wire Line
	10850 2100 10850 2050
Connection ~ 10850 2050
$Comp
L power:+12V #PWR031
U 1 1 6155A069
P 10850 1950
F 0 "#PWR031" H 10850 1800 50  0001 C CNN
F 1 "+12V" H 10865 2123 50  0000 C CNN
F 2 "" H 10850 1950 50  0001 C CNN
F 3 "" H 10850 1950 50  0001 C CNN
	1    10850 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 950  8000 950 
Wire Wire Line
	7550 1750 8000 1750
$Comp
L power:+12V #PWR014
U 1 1 615835B2
P 4200 850
F 0 "#PWR014" H 4200 700 50  0001 C CNN
F 1 "+12V" H 4215 1023 50  0000 C CNN
F 2 "" H 4200 850 50  0001 C CNN
F 3 "" H 4200 850 50  0001 C CNN
	1    4200 850 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR026
U 1 1 61629A77
P 9650 2950
F 0 "#PWR026" H 9650 2700 50  0001 C CNN
F 1 "GND" H 9655 2777 50  0000 C CNN
F 2 "" H 9650 2950 50  0001 C CNN
F 3 "" H 9650 2950 50  0001 C CNN
	1    9650 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 2950 9150 2850
Wire Wire Line
	8950 2950 9150 2950
Wire Wire Line
	8950 2850 8950 2950
$Comp
L power:+3.3V #PWR022
U 1 1 6161AE6C
P 8950 2850
F 0 "#PWR022" H 8950 2700 50  0001 C CNN
F 1 "+3.3V" H 8965 3023 50  0000 C CNN
F 2 "" H 8950 2850 50  0001 C CNN
F 3 "" H 8950 2850 50  0001 C CNN
	1    8950 2850
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR010
U 1 1 615C178D
P 2750 5300
F 0 "#PWR010" H 2750 5150 50  0001 C CNN
F 1 "+12V" H 2765 5473 50  0000 C CNN
F 2 "" H 2750 5300 50  0001 C CNN
F 3 "" H 2750 5300 50  0001 C CNN
	1    2750 5300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 615C9FEC
P 2750 5650
F 0 "C5" V 2498 5650 50  0000 C CNN
F 1 "100nF" V 2589 5650 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2788 5500 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 2750 5650 50  0001 C CNN
F 4 "C14663" V 2750 5650 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 2750 5650 50  0001 C CNN "Purchase Link"
	1    2750 5650
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR03
U 1 1 615D65A7
P 700 6400
F 0 "#PWR03" H 700 6150 50  0001 C CNN
F 1 "GND" H 705 6227 50  0000 C CNN
F 2 "" H 700 6400 50  0001 C CNN
F 3 "" H 700 6400 50  0001 C CNN
	1    700  6400
	1    0    0    -1  
$EndComp
Wire Wire Line
	700  6050 700  6000
Wire Wire Line
	700  6350 700  6400
$Comp
L Driver_Motor:DRV8870DDA U2
U 1 1 615E5FD9
P 1950 5700
F 0 "U2" H 1950 6181 50  0000 C CNN
F 1 "DRV8870DDA" H 1950 6090 50  0000 C CNN
F 2 "Package_SO:Texas_HTSOP-8-1EP_3.9x4.9mm_P1.27mm_EP2.95x4.9mm_Mask2.4x3.1mm_ThermalVias" H 2050 5600 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/drv8870.pdf" H 1700 6050 50  0001 C CNN
F 4 "C86590" H 1950 5700 50  0001 C CNN "JLCPCB Part"
F 5 "NA (Extended)" H 1950 5700 50  0001 C CNN "Purchase Link"
	1    1950 5700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 615E7910
P 2450 6200
F 0 "#PWR08" H 2450 5950 50  0001 C CNN
F 1 "GND" H 2455 6027 50  0000 C CNN
F 2 "" H 2450 6200 50  0001 C CNN
F 3 "" H 2450 6200 50  0001 C CNN
	1    2450 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 5900 2450 5900
Wire Wire Line
	2450 5900 2450 6150
$Comp
L power:+12V #PWR02
U 1 1 6160408B
P 700 6000
F 0 "#PWR02" H 700 5850 50  0001 C CNN
F 1 "+12V" H 715 6173 50  0000 C CNN
F 2 "" H 700 6000 50  0001 C CNN
F 3 "" H 700 6000 50  0001 C CNN
	1    700  6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 6100 1950 6150
Wire Wire Line
	1950 6150 2450 6150
Connection ~ 2450 6150
Wire Wire Line
	2450 6150 2450 6200
$Comp
L power:+3.3V #PWR04
U 1 1 61636063
P 1150 5300
F 0 "#PWR04" H 1150 5150 50  0001 C CNN
F 1 "+3.3V" H 1165 5473 50  0000 C CNN
F 2 "" H 1150 5300 50  0001 C CNN
F 3 "" H 1150 5300 50  0001 C CNN
	1    1150 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 5900 1150 5900
Wire Wire Line
	1150 5900 1150 5300
Text Label 2250 3350 0    50   ~ 0
RESET
Text Label 3500 3350 0    50   ~ 0
DTR
Wire Wire Line
	2950 3350 3150 3350
Text Label 2250 3250 0    50   ~ 0
SCL
Text Label 2250 3150 0    50   ~ 0
SDA
Text Label 2250 3550 0    50   ~ 0
RXD
Text Label 2250 3650 0    50   ~ 0
TXD
Text Label 2250 1850 0    50   ~ 0
LED_1
Text Label 2250 1950 0    50   ~ 0
DRV_A1
Text Label 2250 2050 0    50   ~ 0
DRV_A2
Wire Wire Line
	2250 2050 2600 2050
Text Label 2600 2050 0    50   ~ 0
SS
Text Label 2250 2150 0    50   ~ 0
MOSI
Text Label 2250 2250 0    50   ~ 0
MISO
Text Label 2250 2350 0    50   ~ 0
SCK
Text Label 2250 2750 0    50   ~ 0
BAT_AIN
Text Label 2250 2850 0    50   ~ 0
LED_2
Text Label 2250 2950 0    50   ~ 0
PC2
Text Label 2250 3750 0    50   ~ 0
RF_AUX
Text Label 2250 3850 0    50   ~ 0
PD3
Text Label 2250 3950 0    50   ~ 0
RF_EN
Text Label 2250 4050 0    50   ~ 0
DRV_B1
Text Label 2250 4150 0    50   ~ 0
DRV_B2
Text Label 2250 4250 0    50   ~ 0
RF_SET
Text Label 1550 5700 2    50   ~ 0
DRV_A2
Text Label 1550 5600 2    50   ~ 0
DRV_A1
Text Label 2350 5600 0    50   ~ 0
SOL_A1
Text Label 2350 5700 0    50   ~ 0
SOL_A2
Wire Wire Line
	1950 5400 2750 5400
Wire Wire Line
	2750 5400 2750 5300
Wire Wire Line
	2750 5400 2750 5500
Connection ~ 2750 5400
Wire Wire Line
	2750 5800 2750 6150
Wire Wire Line
	2750 6150 2450 6150
$Comp
L Device:CP C1
U 1 1 616D3744
P 700 6200
F 0 "C1" H 818 6246 50  0000 L CNN
F 1 "4700uF" H 818 6155 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D16.0mm_P7.50mm" H 738 6050 50  0001 C CNN
F 3 "https://www.farnell.com/datasheets/2861351.pdf" H 700 6200 50  0001 C CNN
F 4 "https://au.element14.com/multicomp-pro/mcgpr25v478m16x32/cap-4700-f-25v-20/dp/190289501" H 700 6200 50  0001 C CNN "Purchase Link"
	1    700  6200
	1    0    0    -1  
$EndComp
Text Notes 500  4950 0    50   ~ 0
Solenoid drivers
$Comp
L power:+12V #PWR011
U 1 1 616E3ED0
P 2750 6600
F 0 "#PWR011" H 2750 6450 50  0001 C CNN
F 1 "+12V" H 2765 6773 50  0000 C CNN
F 2 "" H 2750 6600 50  0001 C CNN
F 3 "" H 2750 6600 50  0001 C CNN
	1    2750 6600
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 616E4436
P 2750 6950
F 0 "C6" V 2498 6950 50  0000 C CNN
F 1 "100nF" V 2589 6950 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2788 6800 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 2750 6950 50  0001 C CNN
F 4 "C14663" V 2750 6950 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 2750 6950 50  0001 C CNN "Purchase Link"
	1    2750 6950
	-1   0    0    1   
$EndComp
$Comp
L Driver_Motor:DRV8870DDA U3
U 1 1 616E4440
P 1950 7000
F 0 "U3" H 1950 7481 50  0000 C CNN
F 1 "DRV8870DDA" H 1950 7390 50  0000 C CNN
F 2 "Package_SO:Texas_HTSOP-8-1EP_3.9x4.9mm_P1.27mm_EP2.95x4.9mm_Mask2.4x3.1mm_ThermalVias" H 2050 6900 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/drv8870.pdf" H 1700 7350 50  0001 C CNN
F 4 "C86590" H 1950 7000 50  0001 C CNN "JLCPCB Part"
F 5 "NA (Extended)" H 1950 7000 50  0001 C CNN "Purchase Link"
	1    1950 7000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 616E444A
P 2450 7500
F 0 "#PWR09" H 2450 7250 50  0001 C CNN
F 1 "GND" H 2455 7327 50  0000 C CNN
F 2 "" H 2450 7500 50  0001 C CNN
F 3 "" H 2450 7500 50  0001 C CNN
	1    2450 7500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 7200 2450 7200
Wire Wire Line
	2450 7200 2450 7450
Wire Wire Line
	1950 7400 1950 7450
Wire Wire Line
	1950 7450 2450 7450
Connection ~ 2450 7450
Wire Wire Line
	2450 7450 2450 7500
$Comp
L power:+3.3V #PWR05
U 1 1 616E445A
P 1150 6600
F 0 "#PWR05" H 1150 6450 50  0001 C CNN
F 1 "+3.3V" H 1165 6773 50  0000 C CNN
F 2 "" H 1150 6600 50  0001 C CNN
F 3 "" H 1150 6600 50  0001 C CNN
	1    1150 6600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 7200 1150 7200
Wire Wire Line
	1150 7200 1150 6600
Text Label 1550 7000 2    50   ~ 0
DRV_B2
Text Label 1550 6900 2    50   ~ 0
DRV_B1
Text Label 2350 6900 0    50   ~ 0
SOL_B1
Text Label 2350 7000 0    50   ~ 0
SOL_B2
Wire Wire Line
	1950 6700 2750 6700
Wire Wire Line
	2750 6700 2750 6600
Wire Wire Line
	2750 6700 2750 6800
Connection ~ 2750 6700
Wire Wire Line
	2750 7100 2750 7450
Wire Wire Line
	2750 7450 2450 7450
Wire Notes Line
	500  5000 500  7750
Wire Notes Line
	500  7750 4000 7750
Wire Notes Line
	4000 7750 4000 5000
Wire Notes Line
	4000 5000 500  5000
$Comp
L Connector:Conn_01x02_Male J1
U 1 1 61710337
P 8900 3450
F 0 "J1" H 9000 3650 50  0000 C CNN
F 1 "SOL_A" H 9000 3550 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 8900 3450 50  0001 C CNN
F 3 "~" H 8900 3450 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 8900 3450 50  0001 C CNN "Purchase Link"
	1    8900 3450
	1    0    0    -1  
$EndComp
Text Label 9100 3450 0    50   ~ 0
SOL_A1
Text Label 9100 3550 0    50   ~ 0
SOL_A2
Text Label 9600 1100 3    50   ~ 0
MISO
Text Label 9400 1100 3    50   ~ 0
SCK
Text Label 9300 1100 3    50   ~ 0
MOSI
Text Label 9200 1100 3    50   ~ 0
RESET
Text Label 10350 1100 3    50   ~ 0
RXD
Text Label 10450 1100 3    50   ~ 0
TXD
Text Label 10550 1100 3    50   ~ 0
DTR
Text Label 10650 1100 3    50   ~ 0
RESET
Text Label 9050 1950 3    50   ~ 0
RF_SET
Text Label 9150 1950 3    50   ~ 0
RF_AUX
Text Label 9250 1950 3    50   ~ 0
RXD
Text Label 9350 1950 3    50   ~ 0
TXD
Text Label 9450 1950 3    50   ~ 0
RF_EN
$Comp
L Connector:Conn_01x02_Male J5
U 1 1 6173E740
P 9550 3450
F 0 "J5" H 9650 3650 50  0000 C CNN
F 1 "SOL_B" H 9650 3550 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 9550 3450 50  0001 C CNN
F 3 "~" H 9550 3450 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 9550 3450 50  0001 C CNN "Purchase Link"
	1    9550 3450
	1    0    0    -1  
$EndComp
Text Label 9750 3450 0    50   ~ 0
SOL_B1
Text Label 9750 3550 0    50   ~ 0
SOL_B2
$Comp
L power:+12V #PWR016
U 1 1 61762646
P 4800 5400
F 0 "#PWR016" H 4800 5250 50  0001 C CNN
F 1 "+12V" H 4815 5573 50  0000 C CNN
F 2 "" H 4800 5400 50  0001 C CNN
F 3 "" H 4800 5400 50  0001 C CNN
	1    4800 5400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR017
U 1 1 61763B17
P 4800 6200
F 0 "#PWR017" H 4800 5950 50  0001 C CNN
F 1 "GND" H 4805 6027 50  0000 C CNN
F 2 "" H 4800 6200 50  0001 C CNN
F 3 "" H 4800 6200 50  0001 C CNN
	1    4800 6200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 61764ECC
P 4800 5550
F 0 "R2" H 4870 5596 50  0000 L CNN
F 1 "5.1M" H 4870 5505 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4730 5550 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811081710_UNI-ROYAL-Uniroyal-Elec-0603WAF5104T5E_C13320.pdf" H 4800 5550 50  0001 C CNN
F 4 "C13320" H 4800 5550 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 4800 5550 50  0001 C CNN "Purchase Link"
	1    4800 5550
	1    0    0    -1  
$EndComp
Text Label 5050 5750 0    50   ~ 0
BAT_AIN
$Comp
L Device:C C11
U 1 1 6177C5B8
P 4500 5950
F 0 "C11" V 4248 5950 50  0000 C CNN
F 1 "100nF" V 4339 5950 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4538 5800 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 4500 5950 50  0001 C CNN
F 4 "C14663" V 4500 5950 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 4500 5950 50  0001 C CNN "Purchase Link"
	1    4500 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 5750 5050 5750
$Comp
L Device:R R3
U 1 1 617657AD
P 4800 5950
F 0 "R3" H 4870 5996 50  0000 L CNN
F 1 "390k" H 4870 5905 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4730 5950 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811081731_UNI-ROYAL-Uniroyal-Elec-0603WAF3903T5E_C23150.pdf" H 4800 5950 50  0001 C CNN
F 4 "C23150" H 4800 5950 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 4800 5950 50  0001 C CNN "Purchase Link"
	1    4800 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 6200 4800 6150
Wire Wire Line
	4500 6150 4800 6150
Connection ~ 4800 6150
Wire Wire Line
	4800 6150 4800 6100
Wire Wire Line
	4800 5700 4800 5750
Connection ~ 4800 5750
Wire Wire Line
	4800 5750 4800 5800
Wire Notes Line
	4050 5000 8600 5000
Text Notes 4050 4950 0    50   ~ 0
Misc.
Wire Wire Line
	9650 2850 9650 2950
Text Label 9550 2850 3    50   ~ 0
MOSI
Text Label 9450 2850 3    50   ~ 0
PC2
Text Label 9750 2850 3    50   ~ 0
SCL
Text Label 9250 2850 3    50   ~ 0
PD3
$Comp
L Connector:Conn_01x08_Male J4
U 1 1 61618367
P 9550 2650
F 0 "J4" V 9350 2500 50  0000 L CNN
F 1 "IO" V 9450 2500 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x04_P2.54mm_Vertical" H 9550 2650 50  0001 C CNN
F 3 "~" H 9550 2650 50  0001 C CNN
F 4 "https://au.element14.com/amphenol-icc/77313-801-20lf/conn-header-20pos-2row-2-54mm/dp/3728842" H 9550 2650 50  0001 C CNN "Purchase Link"
	1    9550 2650
	0    1    1    0   
$EndComp
Wire Wire Line
	10800 2900 10800 2950
Wire Wire Line
	10750 2900 10800 2900
Wire Wire Line
	10750 2850 10750 2900
$Comp
L power:GND #PWR030
U 1 1 6154C177
P 10800 2950
F 0 "#PWR030" H 10800 2700 50  0001 C CNN
F 1 "GND" H 10805 2777 50  0000 C CNN
F 2 "" H 10800 2950 50  0001 C CNN
F 3 "" H 10800 2950 50  0001 C CNN
	1    10800 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	10450 2950 10450 2850
Wire Wire Line
	10250 2950 10450 2950
Wire Wire Line
	10250 2850 10250 2950
$Comp
L power:+3.3V #PWR029
U 1 1 6153EEDD
P 10250 2850
F 0 "#PWR029" H 10250 2700 50  0001 C CNN
F 1 "+3.3V" H 10265 3023 50  0000 C CNN
F 2 "" H 10250 2850 50  0001 C CNN
F 3 "" H 10250 2850 50  0001 C CNN
	1    10250 2850
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Male J8
U 1 1 6153C961
P 10650 2650
F 0 "J8" V 10485 2628 50  0000 C CNN
F 1 "I2C" V 10576 2628 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B4B-XH-A_1x04_P2.50mm_Vertical" H 10650 2650 50  0001 C CNN
F 3 "~" H 10650 2650 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 10650 2650 50  0001 C CNN "Purchase Link"
	1    10650 2650
	0    1    1    0   
$EndComp
Text Label 10650 2850 3    50   ~ 0
SDA
Text Label 10550 2850 3    50   ~ 0
SCL
Wire Notes Line
	8650 600  8650 4850
Wire Notes Line
	8650 4850 11150 4850
Wire Notes Line
	11150 4850 11150 600 
Wire Wire Line
	4500 6150 4500 6100
Wire Wire Line
	4500 5800 4500 5750
Wire Wire Line
	4500 5750 4800 5750
Text Label 2250 3050 0    50   ~ 0
PC3
Text Label 9350 2850 3    50   ~ 0
PC3
$Comp
L Device:LED D2
U 1 1 61461EF9
P 5900 5750
F 0 "D2" H 5893 5967 50  0000 C CNN
F 1 "LED" H 5893 5876 50  0000 C CNN
F 2 "LED_SMD:LED_0603_1608Metric_Castellated" H 5900 5750 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810231112_Hubei-KENTO-Elec-KT-0603R_C2286.pdf" H 5900 5750 50  0001 C CNN
F 4 "C2286" H 5900 5750 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 5900 5750 50  0001 C CNN "Purchase Link"
	1    5900 5750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 6146276C
P 5600 6150
F 0 "#PWR0101" H 5600 5900 50  0001 C CNN
F 1 "GND" H 5605 5977 50  0000 C CNN
F 2 "" H 5600 6150 50  0001 C CNN
F 3 "" H 5600 6150 50  0001 C CNN
	1    5600 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 5750 5600 5750
Wire Wire Line
	5600 5750 5600 5850
Text Label 6050 5750 0    50   ~ 0
LED_1
$Comp
L Device:R R7
U 1 1 6146B048
P 5600 6000
F 0 "R7" H 5670 6046 50  0000 L CNN
F 1 "330" H 5670 5955 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5530 6000 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811151642_UNI-ROYAL-Uniroyal-Elec-0402WGF3300TCE_C25104.pdf" H 5600 6000 50  0001 C CNN
F 4 "C25104" H 5600 6000 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 5600 6000 50  0001 C CNN "Purchase Link"
	1    5600 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 61483E25
P 6750 5750
F 0 "D3" H 6743 5967 50  0000 C CNN
F 1 "LED" H 6743 5876 50  0000 C CNN
F 2 "LED_SMD:LED_0603_1608Metric_Castellated" H 6750 5750 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810231112_Hubei-KENTO-Elec-KT-0603R_C2286.pdf" H 6750 5750 50  0001 C CNN
F 4 "C2286" H 6750 5750 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 6750 5750 50  0001 C CNN "Purchase Link"
	1    6750 5750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 6148449D
P 6450 6150
F 0 "#PWR0102" H 6450 5900 50  0001 C CNN
F 1 "GND" H 6455 5977 50  0000 C CNN
F 2 "" H 6450 6150 50  0001 C CNN
F 3 "" H 6450 6150 50  0001 C CNN
	1    6450 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6600 5750 6450 5750
Wire Wire Line
	6450 5750 6450 5850
Text Label 6900 5750 0    50   ~ 0
LED_2
$Comp
L Device:R R8
U 1 1 614844AC
P 6450 6000
F 0 "R8" H 6520 6046 50  0000 L CNN
F 1 "330" H 6520 5955 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6380 6000 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811151642_UNI-ROYAL-Uniroyal-Elec-0402WGF3300TCE_C25104.pdf" H 6450 6000 50  0001 C CNN
F 4 "C25104" H 6450 6000 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 6450 6000 50  0001 C CNN "Purchase Link"
	1    6450 6000
	1    0    0    -1  
$EndComp
Text Label 9850 2850 3    50   ~ 0
SDA
Wire Wire Line
	10600 3600 10600 3700
$Comp
L power:GND #PWR0103
U 1 1 617171B6
P 10250 3700
F 0 "#PWR0103" H 10250 3450 50  0001 C CNN
F 1 "GND" H 10255 3527 50  0000 C CNN
F 2 "" H 10250 3700 50  0001 C CNN
F 3 "" H 10250 3700 50  0001 C CNN
	1    10250 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	10500 3600 10500 3700
Wire Wire Line
	10500 3700 10250 3700
Wire Wire Line
	10700 3700 10600 3700
$Comp
L Connector:Conn_01x02_Male J9
U 1 1 617171AB
P 10600 3400
F 0 "J9" V 10435 3378 50  0000 C CNN
F 1 "SOLAR" V 10526 3378 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 10600 3400 50  0001 C CNN
F 3 "~" H 10600 3400 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 10600 3400 50  0001 C CNN "Purchase Link"
	1    10600 3400
	0    1    1    0   
$EndComp
Wire Notes Line
	8600 5000 8600 6500
Wire Notes Line
	8600 6500 4050 6500
Wire Notes Line
	4050 6500 4050 5000
Text Label 10700 3700 0    50   ~ 0
SOLAR
Wire Notes Line
	8600 2650 4050 2650
Wire Notes Line
	8600 600  8600 2650
$Comp
L Regulator_Switching:LM27313XMF U5
U 1 1 616DCF83
P 5450 3700
F 0 "U5" H 5450 4067 50  0000 C CNN
F 1 "LM27313XMF" H 5450 3976 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 5500 3450 50  0001 L CIN
F 3 "http://www.ti.com/lit/ds/symlink/lm27313.pdf" H 5450 3800 50  0001 C CNN
F 4 "C32355" H 5450 3700 50  0001 C CNN "JLCPCB Part"
F 5 "NA (Extended)" H 5450 3700 50  0001 C CNN "Purchase Link"
	1    5450 3700
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0104
U 1 1 616E07F9
P 7200 3150
F 0 "#PWR0104" H 7200 3000 50  0001 C CNN
F 1 "+12V" H 7215 3323 50  0000 C CNN
F 2 "" H 7200 3150 50  0001 C CNN
F 3 "" H 7200 3150 50  0001 C CNN
	1    7200 3150
	1    0    0    -1  
$EndComp
Text Label 4400 3100 2    50   ~ 0
SOLAR
$Comp
L Device:C C18
U 1 1 616E3688
P 4400 3700
F 0 "C18" H 4515 3746 50  0000 L CNN
F 1 "10uF" H 4515 3655 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 4438 3550 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810221112_Samsung-Electro-Mechanics-CL31A106KBHNNNE_C13585.pdf" H 4400 3700 50  0001 C CNN
F 4 "C13585" H 4400 3700 50  0001 C CNN "JLCPCB Part"
F 5 "" H 4400 3700 50  0001 C CNN "JLCPCB UP"
F 6 "" H 4400 3700 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 4400 3700 50  0001 C CNN "Purchase Link"
	1    4400 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 3100 4400 3200
$Comp
L power:GND #PWR0105
U 1 1 616ED141
P 4400 4300
F 0 "#PWR0105" H 4400 4050 50  0001 C CNN
F 1 "GND" H 4405 4127 50  0000 C CNN
F 2 "" H 4400 4300 50  0001 C CNN
F 3 "" H 4400 4300 50  0001 C CNN
	1    4400 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 3850 4400 4200
Wire Wire Line
	4800 3800 5150 3800
Connection ~ 4400 3200
Wire Wire Line
	4400 3200 4400 3550
Wire Wire Line
	4800 3600 4800 3800
$Comp
L Device:R R9
U 1 1 616FD6AE
P 4800 3450
F 0 "R9" H 4870 3496 50  0000 L CNN
F 1 "51k" H 4870 3405 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4730 3450 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810301313_UNI-ROYAL-Uniroyal-Elec-0402WGF5102TCE_C25794.pdf" H 4800 3450 50  0001 C CNN
F 4 "C25794" H 4800 3450 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 4800 3450 50  0001 C CNN "Purchase Link"
	1    4800 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 3200 4800 3200
Wire Wire Line
	4800 3200 4800 3300
$Comp
L Device:L L2
U 1 1 6176DE75
P 5450 3200
F 0 "L2" V 5640 3200 50  0000 C CNN
F 1 "10uH" V 5549 3200 50  0000 C CNN
F 2 "lib:L_4x4mm_H2mm" H 5450 3200 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810251524_Chilisin-Elec-LVS404018-100M-N_C108279.pdf" H 5450 3200 50  0001 C CNN
F 4 "C108279" H 5450 3200 50  0001 C CNN "JLCPCB Part"
F 5 "NA (Extended)" H 5450 3200 50  0001 C CNN "Purchase Link"
	1    5450 3200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5150 3600 5100 3600
Wire Wire Line
	5100 3600 5100 3200
Wire Wire Line
	5100 3200 5300 3200
Wire Wire Line
	5750 3600 5800 3600
Wire Wire Line
	5800 3600 5800 3200
Wire Wire Line
	5800 3200 5600 3200
$Comp
L Device:D_Schottky D4
U 1 1 617836AC
P 6050 3200
F 0 "D4" H 6000 3300 50  0000 L CNN
F 1 "550mV 1A" H 5850 3100 50  0000 L CNN
F 2 "Diode_SMD:D_SMA" H 6050 3200 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/2105061432_MDD%EF%BC%88Microdiode-Electronics%EF%BC%89-SS14_C2480.pdf" H 6050 3200 50  0001 C CNN
F 4 "C2480" H 6050 3200 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 6050 3200 50  0001 C CNN "Purchase Link"
	1    6050 3200
	-1   0    0    1   
$EndComp
Wire Wire Line
	5800 3200 5900 3200
Connection ~ 5800 3200
Wire Wire Line
	5450 4000 5450 4200
Wire Wire Line
	5450 4200 4400 4200
Connection ~ 4400 4200
Wire Wire Line
	4400 4200 4400 4300
$Comp
L Device:C C20
U 1 1 6179FEE7
P 7200 3700
F 0 "C20" H 7315 3746 50  0000 L CNN
F 1 "10uF" H 7315 3655 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 7238 3550 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810221112_Samsung-Electro-Mechanics-CL31A106KBHNNNE_C13585.pdf" H 7200 3700 50  0001 C CNN
F 4 "C13585" H 7200 3700 50  0001 C CNN "JLCPCB Part"
F 5 "" H 7200 3700 50  0001 C CNN "JLCPCB UP"
F 6 "" H 7200 3700 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 7200 3700 50  0001 C CNN "Purchase Link"
	1    7200 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 3200 6300 3200
Wire Wire Line
	5450 4200 6300 4200
Wire Wire Line
	7200 4200 7200 3850
Connection ~ 5450 4200
$Comp
L Device:C C19
U 1 1 617D0BA9
P 6750 3500
F 0 "C19" H 6865 3546 50  0000 L CNN
F 1 "200pF" H 6865 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6788 3350 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811061810_FH-Guangdong-Fenghua-Advanced-Tech-0603B201K500NT_C1600.pdf" H 6750 3500 50  0001 C CNN
F 4 "C1600" H 6750 3500 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 6750 3500 50  0001 C CNN "Purchase Link"
	1    6750 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R11
U 1 1 618428C1
P 6300 4000
F 0 "R11" H 6370 4046 50  0000 L CNN
F 1 "13k" H 6370 3955 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6230 4000 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811081713_UNI-ROYAL-Uniroyal-Elec-0603WAF1302T5E_C22797.pdf" H 6300 4000 50  0001 C CNN
F 4 "C22797" H 6300 4000 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 6300 4000 50  0001 C CNN "Purchase Link"
	1    6300 4000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R10
U 1 1 6184877F
P 6300 3500
F 0 "R10" H 6370 3546 50  0000 L CNN
F 1 "130k" H 6370 3455 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6230 3500 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811081723_UNI-ROYAL-Uniroyal-Elec-0603WAF1303T5E_C22795.pdf" H 6300 3500 50  0001 C CNN
F 4 "C22795" H 6300 3500 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 6300 3500 50  0001 C CNN "Purchase Link"
	1    6300 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 3800 6300 3850
Connection ~ 6300 3800
Wire Wire Line
	6300 4150 6300 4200
Connection ~ 6300 4200
Wire Wire Line
	6300 4200 7200 4200
Connection ~ 6300 3200
Wire Wire Line
	6300 3200 6750 3200
Wire Wire Line
	6300 3800 6300 3650
Wire Wire Line
	6300 3200 6300 3350
Wire Wire Line
	5750 3800 6300 3800
Wire Wire Line
	6300 3800 6750 3800
Wire Wire Line
	6750 3800 6750 3650
Wire Wire Line
	7200 3200 7200 3550
Wire Wire Line
	6750 3200 6750 3350
Connection ~ 6750 3200
Wire Wire Line
	6750 3200 7200 3200
Wire Wire Line
	7200 3150 7200 3200
Connection ~ 7200 3200
Wire Notes Line
	4050 2800 8600 2800
Wire Notes Line
	8600 4850 4050 4850
Text Notes 4050 2750 0    50   ~ 0
13.5v LM27313X
Wire Notes Line
	4050 2800 4050 4850
Wire Notes Line
	8600 4850 8600 2800
Wire Wire Line
	4800 3200 5100 3200
Connection ~ 4800 3200
Connection ~ 5100 3200
$EndSCHEMATC
