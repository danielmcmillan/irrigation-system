EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Irrigation Controller Unit"
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
L Device:C C1
U 1 1 6137C5A1
P 850 950
F 0 "C1" V 598 950 50  0000 C CNN
F 1 "100nF" V 689 950 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 888 800 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 850 950 50  0001 C CNN
F 4 "C14663" V 850 950 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 850 950 50  0001 C CNN "JLCPCB UP"
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
Text GLabel 2250 4250 2    50   Input ~ 0
RFSET
Text GLabel 2250 4150 2    50   Input ~ 0
RFEN
Text GLabel 2250 2050 2    50   Input ~ 0
ASS
Text GLabel 2250 3250 2    50   Input ~ 0
ASCL
Text GLabel 2250 3150 2    50   Input ~ 0
ASDA
$Comp
L Device:R R5
U 1 1 6137F098
P 2800 3100
F 0 "R5" H 2870 3146 50  0000 L CNN
F 1 "10k" H 2870 3055 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 2730 3100 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 2800 3100 50  0001 C CNN
F 4 "C25744" H 2800 3100 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 2800 3100 50  0001 C CNN "JLCPCB UP"
F 6 "" H 2800 3100 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 2800 3100 50  0001 C CNN "Purchase Link"
	1    2800 3100
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR08
U 1 1 6137DC7A
P 2800 2950
F 0 "#PWR08" H 2800 2800 50  0001 C CNN
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
Text GLabel 3150 3550 2    50   Input ~ 0
ARESET
$Comp
L Device:C C4
U 1 1 613811FE
P 3300 3350
F 0 "C4" V 3048 3350 50  0000 C CNN
F 1 "100nF" V 3139 3350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3338 3200 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 3300 3350 50  0001 C CNN
F 4 "C14663" V 3300 3350 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 3300 3350 50  0001 C CNN "JLCPCB UP"
F 6 "" H 3300 3350 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 3300 3350 50  0001 C CNN "Purchase Link"
	1    3300 3350
	0    1    1    0   
$EndComp
Text GLabel 3500 3350 2    50   Input ~ 0
ADTR
Wire Wire Line
	3450 3350 3500 3350
Wire Wire Line
	3150 3550 3100 3550
Wire Wire Line
	3150 3350 3100 3350
Wire Wire Line
	2950 3350 2800 3350
Wire Wire Line
	2950 3600 2950 3350
Wire Wire Line
	2950 4000 2950 4550
Text GLabel 2250 3750 2    50   Input ~ 0
RFAUX
Text GLabel 2250 2350 2    50   Input ~ 0
ASCK
Text GLabel 2250 2150 2    50   Input ~ 0
AMOSI
Text GLabel 2250 2250 2    50   Input ~ 0
AMISO
Text GLabel 2250 3650 2    50   Input ~ 0
ATXD
Text GLabel 2250 3550 2    50   Input ~ 0
ARXD
$Comp
L Device:C C2
U 1 1 61391ACB
P 850 1400
F 0 "C2" V 598 1400 50  0000 C CNN
F 1 "100nF" V 689 1400 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 888 1250 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 850 1400 50  0001 C CNN
F 4 "C14663" V 850 1400 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 850 1400 50  0001 C CNN "JLCPCB UP"
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
F 1 "SPST" V 2995 3898 50  0000 L CNN
F 2 "lib:SW_SPST_THT_5.0mm" H 2950 3800 50  0001 C CNN
F 3 "https://www.farnell.com/datasheets/3176355.pdf" H 2950 3800 50  0001 C CNN
F 4 "" H 2950 3800 50  0001 C CNN "Purchase link"
F 5 "https://au.element14.com/multicomp-pro/mc32879/switch-tactile-spst-no-50ma-though/dp/171299401" H 2950 3800 50  0001 C CNN "Purchase Link"
	1    2950 3800
	0    1    1    0   
$EndComp
Connection ~ 3100 3350
Wire Wire Line
	3100 3550 3100 3350
Connection ~ 2800 3350
Wire Wire Line
	3100 3350 2950 3350
Wire Wire Line
	2250 3350 2800 3350
Wire Wire Line
	1650 1400 1650 1300
Connection ~ 1650 1400
Wire Wire Line
	1650 1550 1650 1400
$Comp
L power:GND #PWR06
U 1 1 6137B4FD
P 2300 4550
F 0 "#PWR06" H 2300 4300 50  0001 C CNN
F 1 "GND" H 2305 4377 50  0000 C CNN
F 2 "" H 2300 4550 50  0001 C CNN
F 3 "" H 2300 4550 50  0001 C CNN
	1    2300 4550
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR04
U 1 1 61379A6F
P 1650 1300
F 0 "#PWR04" H 1650 1150 50  0001 C CNN
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
F 5 "0.26" H 2950 2500 50  0001 C CNN "JLCPCB UP"
F 6 "" H 2950 2500 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 2950 2500 50  0001 C CNN "Purchase Link"
	1    2950 2500
	0    1    1    0   
$EndComp
Wire Wire Line
	2800 2300 2950 2300
$Comp
L Device:C C6
U 1 1 6136E259
P 3450 2700
F 0 "C6" V 3198 2700 50  0000 C CNN
F 1 "22pF" V 3289 2700 50  0000 C CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 3488 2550 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811141710_FH-Guangdong-Fenghua-Advanced-Tech-0402CG220J500NT_C1555.pdf" H 3450 2700 50  0001 C CNN
F 4 "C1555" V 3450 2700 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 3450 2700 50  0001 C CNN "JLCPCB UP"
F 6 "" H 3450 2700 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 3450 2700 50  0001 C CNN "Purchase Link"
	1    3450 2700
	0    1    1    0   
$EndComp
$Comp
L Device:C C3
U 1 1 6136D388
P 850 1850
F 0 "C3" V 598 1850 50  0000 C CNN
F 1 "100nF" V 689 1850 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 888 1700 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 850 1850 50  0001 C CNN
F 4 "C14663" V 850 1850 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 850 1850 50  0001 C CNN "JLCPCB UP"
F 6 "" H 850 1850 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 850 1850 50  0001 C CNN "Purchase Link"
	1    850  1850
	0    1    1    0   
$EndComp
Wire Wire Line
	3850 2500 3850 2700
$Comp
L power:GND #PWR010
U 1 1 6136C5DF
P 3850 2700
F 0 "#PWR010" H 3850 2450 50  0001 C CNN
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
L Device:C C5
U 1 1 613652C9
P 3450 2300
F 0 "C5" V 3198 2300 50  0000 C CNN
F 1 "22pF" V 3289 2300 50  0000 C CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 3488 2150 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811141710_FH-Guangdong-Fenghua-Advanced-Tech-0402CG220J500NT_C1555.pdf" H 3450 2300 50  0001 C CNN
F 4 "C1555" V 3450 2300 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 3450 2300 50  0001 C CNN "JLCPCB UP"
F 6 "" H 3450 2300 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 3450 2300 50  0001 C CNN "Purchase Link"
	1    3450 2300
	0    1    1    0   
$EndComp
$Comp
L MCU_Microchip_ATmega:ATmega328P-AU U2
U 1 1 6136041F
P 1650 3050
F 0 "U2" H 1650 1461 50  0000 C CNN
F 1 "ATmega328P-AU" H 1650 1370 50  0000 C CNN
F 2 "Package_QFP:TQFP-32_7x7mm_P0.8mm" H 1650 3050 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328_P%20AVR%20MCU%20with%20picoPower%20Technology%20Data%20Sheet%2040001984A.pdf" H 1650 3050 50  0001 C CNN
F 4 "C14877" H 1650 3050 50  0001 C CNN "JLCPCB Part"
F 5 "3.96" H 1650 3050 50  0001 C CNN "JLCPCB UP"
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
Text Notes 500  4950 0    50   ~ 0
Relays
$Comp
L Relay_SolidState:TLP222A U1
U 1 1 614F72BF
P 1450 5400
F 0 "U1" H 1450 5725 50  0000 C CNN
F 1 "AQY280EH" H 1450 5634 50  0000 C CNN
F 2 "Package_DIP:DIP-4_W7.62mm" H 1250 5200 50  0001 L CIN
F 3 "https://4donline.ihs.com/images/VipMasterIC/IC/ARMC/ARMCS00681/ARMCS00681-1.pdf?hkey=6D3A4C79FDBF58556ACFDE234799DDF0" H 1450 5400 50  0001 L CNN
F 4 "https://au.element14.com/panasonic/aqy280eh/mosfet-relay-spst-no-0-13a-350v/dp/3588782" H 1450 5400 50  0001 C CNN "Purchase Link"
	1    1450 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 614FD9FA
P 950 5500
F 0 "R2" H 1020 5546 50  0000 L CNN
F 1 "620" H 1020 5455 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 880 5500 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811081712_UNI-ROYAL-Uniroyal-Elec-0603WAF6200T5E_C23220.pdf" H 950 5500 50  0001 C CNN
F 4 "C23220" H 950 5500 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 950 5500 50  0001 C CNN "JLCPCB UP"
F 6 "" H 950 5500 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 950 5500 50  0001 C CNN "Purchase Link"
	1    950  5500
	0    1    1    0   
$EndComp
Wire Wire Line
	1150 5300 1100 5300
Text GLabel 1100 5300 0    50   Input ~ 0
AIO1
Text GLabel 1750 5300 2    50   Input ~ 0
R1A
Text GLabel 1750 5500 2    50   Input ~ 0
R1B
Text GLabel 2250 3950 2    50   Input ~ 0
AIO1
$Comp
L Relay_SolidState:TLP222A U3
U 1 1 6151AE01
P 2850 5400
F 0 "U3" H 2850 5725 50  0000 C CNN
F 1 "AQY280EH" H 2850 5634 50  0000 C CNN
F 2 "Package_DIP:DIP-4_W7.62mm" H 2650 5200 50  0001 L CIN
F 3 "https://4donline.ihs.com/images/VipMasterIC/IC/ARMC/ARMCS00681/ARMCS00681-1.pdf?hkey=6D3A4C79FDBF58556ACFDE234799DDF0" H 2850 5400 50  0001 L CNN
F 4 "https://au.element14.com/panasonic/aqy280eh/mosfet-relay-spst-no-0-13a-350v/dp/3588782" H 2850 5400 50  0001 C CNN "Purchase Link"
	1    2850 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 5300 2500 5300
Text GLabel 2500 5300 0    50   Input ~ 0
AIO2
Text GLabel 3150 5300 2    50   Input ~ 0
R2A
Text GLabel 3150 5500 2    50   Input ~ 0
R2B
Text GLabel 2250 1850 2    50   Input ~ 0
AIO2
$Comp
L lib:UA2-5NU K1
U 1 1 6152BDC7
P 1900 7100
F 0 "K1" V 1133 7100 50  0000 C CNN
F 1 "UA2-5NU" V 1224 7100 50  0000 C CNN
F 2 "lib:Relay_DPDT_NEXEM_UA2_Pitch3.2mm" H 1900 7100 50  0001 C CNN
F 3 "https://www.farnell.com/datasheets/2302216.pdf" H 1900 7100 50  0001 C CNN
F 4 "https://au.element14.com/nexem/ua2-5nu/signal-relay-dpdt-1a-250vac-th/dp/2766147" H 1900 7100 50  0001 C CNN "Purchase Link"
	1    1900 7100
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR07
U 1 1 61534921
P 2400 6350
F 0 "#PWR07" H 2400 6200 50  0001 C CNN
F 1 "+5V" H 2415 6523 50  0000 C CNN
F 2 "" H 2400 6350 50  0001 C CNN
F 3 "" H 2400 6350 50  0001 C CNN
	1    2400 6350
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:MMBT3904 Q1
U 1 1 6154A75E
P 850 7050
F 0 "Q1" H 1041 7096 50  0000 L CNN
F 1 "MMBT3904" H 1041 7005 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 1050 6975 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/2N3903-D.PDF" H 850 7050 50  0001 L CNN
F 4 "" H 850 7050 50  0001 C CNN "JLCPCB Basic"
F 5 "C20526" H 850 7050 50  0001 C CNN "JLCPCB Part"
F 6 "0.03" H 850 7050 50  0001 C CNN "JLCPCB UP"
F 7 "NA" H 850 7050 50  0001 C CNN "Purchase Link"
	1    850  7050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 61559592
P 950 7400
F 0 "#PWR03" H 950 7150 50  0001 C CNN
F 1 "GND" H 955 7227 50  0000 C CNN
F 2 "" H 950 7400 50  0001 C CNN
F 3 "" H 950 7400 50  0001 C CNN
	1    950  7400
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  7400 950  7250
Wire Wire Line
	950  6700 950  6850
$Comp
L Device:D D1
U 1 1 615610BE
P 1500 6450
F 0 "D1" H 1500 6233 50  0000 C CNN
F 1 "SM4007PL" H 1500 6324 50  0000 C CNN
F 2 "Diode_SMD:D_SMA" H 1500 6450 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810311713_MDD%EF%BC%88Microdiode-Electronics%EF%BC%89-M7_C95872.pdf" H 1500 6450 50  0001 C CNN
F 4 "" H 1500 6450 50  0001 C CNN "JLCPCB Basic"
F 5 "C95872" H 1500 6450 50  0001 C CNN "JLCPCB Part"
F 6 "0.03" H 1500 6450 50  0001 C CNN "JLCPCB UP"
F 7 "NA" H 1500 6450 50  0001 C CNN "Purchase Link"
	1    1500 6450
	0    1    1    0   
$EndComp
$Comp
L Device:R R3
U 1 1 6151AE10
P 2350 5500
F 0 "R3" H 2420 5546 50  0000 L CNN
F 1 "620" H 2420 5455 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 2280 5500 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811081712_UNI-ROYAL-Uniroyal-Elec-0603WAF6200T5E_C23220.pdf" H 2350 5500 50  0001 C CNN
F 4 "C23220" H 2350 5500 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 2350 5500 50  0001 C CNN "JLCPCB UP"
F 6 "" H 2350 5500 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 2350 5500 50  0001 C CNN "Purchase Link"
	1    2350 5500
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR02
U 1 1 614FA7A8
P 700 5550
F 0 "#PWR02" H 700 5300 50  0001 C CNN
F 1 "GND" H 705 5377 50  0000 C CNN
F 2 "" H 700 5550 50  0001 C CNN
F 3 "" H 700 5550 50  0001 C CNN
	1    700  5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 5500 1100 5500
Wire Wire Line
	800  5500 700  5500
Wire Wire Line
	700  5500 700  5550
Wire Wire Line
	2500 5500 2550 5500
$Comp
L power:GND #PWR05
U 1 1 6162A0EA
P 2100 5550
F 0 "#PWR05" H 2100 5300 50  0001 C CNN
F 1 "GND" H 2105 5377 50  0000 C CNN
F 2 "" H 2100 5550 50  0001 C CNN
F 3 "" H 2100 5550 50  0001 C CNN
	1    2100 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 5500 2100 5500
Wire Wire Line
	2100 5500 2100 5550
Text GLabel 600  6550 1    50   Input ~ 0
AIO3
Wire Wire Line
	600  6550 600  6650
Wire Wire Line
	650  7050 600  7050
Wire Wire Line
	600  7050 600  6950
Text GLabel 1500 7000 1    50   Input ~ 0
R3AC
Wire Wire Line
	1600 7100 1500 7100
Wire Wire Line
	1500 7100 1500 7000
Text GLabel 1500 7400 1    50   Input ~ 0
R3BC
Wire Wire Line
	1600 7500 1500 7500
Wire Wire Line
	1500 7500 1500 7400
Text GLabel 2300 7000 2    50   Input ~ 0
R3ANC
Wire Wire Line
	2200 7000 2300 7000
Text GLabel 2300 7200 2    50   Input ~ 0
R3ANO
Wire Wire Line
	2300 7200 2200 7200
Text GLabel 2300 7400 2    50   Input ~ 0
R3BNC
Wire Wire Line
	2200 7400 2300 7400
Text GLabel 2300 7600 2    50   Input ~ 0
R3BNO
Wire Wire Line
	2300 7600 2200 7600
$Comp
L Device:R R1
U 1 1 6166FFAE
P 600 6800
F 0 "R1" H 530 6754 50  0000 R CNN
F 1 "2.2k" H 530 6845 50  0000 R CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 530 6800 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811022110_UNI-ROYAL-Uniroyal-Elec-0402WGF2201TCE_C25879.pdf" H 600 6800 50  0001 C CNN
F 4 "" H 600 6800 50  0001 C CNN "JLCPCB Basic"
F 5 "C25879" H 600 6800 50  0001 C CNN "JLCPCB Part"
F 6 "0.01" H 600 6800 50  0001 C CNN "JLCPCB UP"
F 7 "NA" H 600 6800 50  0001 C CNN "Purchase Link"
	1    600  6800
	-1   0    0    1   
$EndComp
Text GLabel 2250 1950 2    50   Input ~ 0
AIO3
Text GLabel 2250 3850 2    50   Input ~ 0
ASRX
Text GLabel 2250 4050 2    50   Input ~ 0
ASTX
Text GLabel 4150 5600 0    50   Input ~ 0
ASTX
Text GLabel 7650 5600 0    50   Input ~ 0
ASRX
Text GLabel 10300 1100 3    50   Input ~ 0
R1A
Text GLabel 10200 1100 3    50   Input ~ 0
R1B
Text GLabel 10700 1100 3    50   Input ~ 0
R2A
Text GLabel 10600 1100 3    50   Input ~ 0
R2B
Text GLabel 10400 1950 3    50   Input ~ 0
R3AC
Text GLabel 10300 1950 3    50   Input ~ 0
R3ANO
Text GLabel 10200 1950 3    50   Input ~ 0
R3ANC
Text GLabel 10850 1950 3    50   Input ~ 0
R3BC
Text GLabel 10750 1950 3    50   Input ~ 0
R3BNO
Text GLabel 10650 1950 3    50   Input ~ 0
R3BNC
Text GLabel 9250 4750 3    50   Input ~ 0
ASDA
Text GLabel 9350 4750 3    50   Input ~ 0
ASCL
Text GLabel 10200 3800 3    50   Input ~ 0
ASS
Text GLabel 10300 3800 3    50   Input ~ 0
ASCK
Text GLabel 10500 3800 3    50   Input ~ 0
AMOSI
Text GLabel 10400 3800 3    50   Input ~ 0
AMISO
Text GLabel 6500 7050 2    50   Input ~ 0
RS485A
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
L Device:C C7
U 1 1 61394644
P 4200 1150
F 0 "C7" H 4315 1196 50  0000 L CNN
F 1 "10uF" H 4315 1105 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 4238 1000 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810221112_Samsung-Electro-Mechanics-CL31A106KBHNNNE_C13585.pdf" H 4200 1150 50  0001 C CNN
F 4 "C13585" H 4200 1150 50  0001 C CNN "JLCPCB Part"
F 5 "0.07" H 4200 1150 50  0001 C CNN "JLCPCB UP"
F 6 "" H 4200 1150 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 4200 1150 50  0001 C CNN "Purchase Link"
	1    4200 1150
	1    0    0    -1  
$EndComp
$Comp
L power:+24V #PWR011
U 1 1 61395C8E
P 4200 850
F 0 "#PWR011" H 4200 700 50  0001 C CNN
F 1 "+24V" H 4215 1023 50  0000 C CNN
F 2 "" H 4200 850 50  0001 C CNN
F 3 "" H 4200 850 50  0001 C CNN
	1    4200 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 850  4200 950 
NoConn ~ 5350 1150
$Comp
L power:GND #PWR012
U 1 1 613AC985
P 4200 2400
F 0 "#PWR012" H 4200 2150 50  0001 C CNN
F 1 "GND" H 4205 2227 50  0000 C CNN
F 2 "" H 4200 2400 50  0001 C CNN
F 3 "" H 4200 2400 50  0001 C CNN
	1    4200 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 1300 4200 1350
Wire Wire Line
	5350 950  4200 950 
Connection ~ 4200 950 
Wire Wire Line
	4200 950  4200 1000
$Comp
L Device:C C11
U 1 1 613CBCCA
P 4900 1350
F 0 "C11" V 4648 1350 50  0000 C CNN
F 1 "8.2nF" V 4739 1350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4938 1200 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810262007_Samsung-Electro-Mechanics-CL10B822KB8NNNC_C27920.pdf" H 4900 1350 50  0001 C CNN
F 4 "C27920" H 4900 1350 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 4900 1350 50  0001 C CNN "Purchase Link"
	1    4900 1350
	0    1    1    0   
$EndComp
Wire Wire Line
	5050 1350 5350 1350
Connection ~ 4200 1350
Wire Wire Line
	4200 1350 4200 2350
$Comp
L Device:R R8
U 1 1 613D73DD
P 5200 1800
F 0 "R8" H 5270 1846 50  0000 L CNN
F 1 "12k" H 5270 1755 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5130 1800 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811021221_UNI-ROYAL-Uniroyal-Elec-0603WAF1202T5E_C22790.pdf" H 5200 1800 50  0001 C CNN
F 4 "C22790" H 5200 1800 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 5200 1800 50  0001 C CNN "Purchase Link"
	1    5200 1800
	1    0    0    -1  
$EndComp
$Comp
L Device:C C13
U 1 1 613D970A
P 5200 2150
F 0 "C13" H 5315 2196 50  0000 L CNN
F 1 "2.7nF" H 5315 2105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5238 2000 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811151136_FH-Guangdong-Fenghua-Advanced-Tech-0603B272K500NT_C1609.pdf" H 5200 2150 50  0001 C CNN
F 4 "C1609" H 5200 2150 50  0001 C CNN "JLCPCB Part"
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
	4200 1350 4750 1350
Wire Wire Line
	4650 2050 4650 2350
Wire Wire Line
	4650 1550 4650 1750
$Comp
L Device:C C9
U 1 1 613D89DD
P 4650 1900
F 0 "C9" H 4765 1946 50  0000 L CNN
F 1 "27pF" H 4765 1855 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4688 1750 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810261514_Samsung-Electro-Mechanics-CL10C270JB8NNNC_C1656.pdf" H 4650 1900 50  0001 C CNN
F 4 "C1656" H 4650 1900 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 4650 1900 50  0001 C CNN "Purchase Link"
	1    4650 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 1950 5200 2000
$Comp
L Device:C C15
U 1 1 6141E59E
P 6600 1150
F 0 "C15" V 6348 1150 50  0000 C CNN
F 1 "100nF" V 6439 1150 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6638 1000 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 6600 1150 50  0001 C CNN
F 4 "C14663" V 6600 1150 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 6600 1150 50  0001 C CNN "JLCPCB UP"
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
L Device:D_Schottky D3
U 1 1 6145F7F3
P 7000 1100
F 0 "D3" V 6954 1180 50  0000 L CNN
F 1 "40V 1A" V 7045 1180 50  0000 L CNN
F 2 "Diode_SMD:D_SMA" H 7000 1100 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/2105061432_MDD%EF%BC%88Microdiode-Electronics%EF%BC%89-SS14_C2480.pdf" H 7000 1100 50  0001 C CNN
F 4 "" H 7000 1100 50  0001 C CNN "JBCPCB Part"
F 5 "" H 7000 1100 50  0001 C CNN "JBCPCB UP"
F 6 "" H 7000 1100 50  0001 C CNN "JCBPCB Basic"
F 7 "" H 7000 1100 50  0001 C CNN "JLCPCB Basic"
F 8 "C2480" H 7000 1100 50  0001 C CNN "JLCPCB Part"
F 9 "0.05" H 7000 1100 50  0001 C CNN "JLCPCB UP"
F 10 "NA" H 7000 1100 50  0001 C CNN "Purchase Link"
	1    7000 1100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR023
U 1 1 61461209
P 7000 1800
F 0 "#PWR023" H 7000 1550 50  0001 C CNN
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
P 7650 950
F 0 "L1" V 7840 950 50  0000 C CNN
F 1 "4.7uH" V 7749 950 50  0000 C CNN
F 2 "Capacitor_THT:C_Radial_D5.0mm_H5.0mm_P2.00mm" H 7650 950 50  0001 C CNN
F 3 "https://au.mouser.com/datasheet/2/281/kmp_1100r-48592.pdf" H 7650 950 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/Murata-Power-Solutions/11R472C?qs=sGAEpiMZZMv126LJFLh8y6dg16FCDTo2uuZAWc3xT0U%3D" H 7650 950 50  0001 C CNN "Purchase Link"
	1    7650 950 
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7000 950  7500 950 
$Comp
L Device:R R16
U 1 1 61498598
P 7900 1150
F 0 "R16" H 7970 1196 50  0000 L CNN
F 1 "10k" H 7970 1105 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 7830 1150 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 7900 1150 50  0001 C CNN
F 4 "C25744" H 7900 1150 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 7900 1150 50  0001 C CNN "JLCPCB UP"
F 6 "" H 7900 1150 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 7900 1150 50  0001 C CNN "Purchase Link"
	1    7900 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 950  7900 950 
Wire Wire Line
	7900 950  7900 1000
Wire Wire Line
	6450 1750 7000 1750
Connection ~ 7000 1750
Wire Wire Line
	7000 1750 7000 1800
Wire Wire Line
	6350 1350 7900 1350
Wire Wire Line
	7900 1350 7900 1300
$Comp
L Device:R R17
U 1 1 614B25D9
P 7900 1550
F 0 "R17" H 7970 1596 50  0000 L CNN
F 1 "3k" H 7970 1505 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 7830 1550 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811081711_UNI-ROYAL-Uniroyal-Elec-0603WAF3001T5E_C4211.pdf" H 7900 1550 50  0001 C CNN
F 4 "C4211" H 7900 1550 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 7900 1550 50  0001 C CNN "Purchase Link"
	1    7900 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 1350 7900 1400
Connection ~ 7900 1350
Wire Wire Line
	7900 1700 7900 1750
$Comp
L Device:C C17
U 1 1 614C1773
P 8250 1350
F 0 "C17" H 8365 1396 50  0000 L CNN
F 1 "10uF" H 8365 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 8288 1200 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810221112_Samsung-Electro-Mechanics-CL31A106KBHNNNE_C13585.pdf" H 8250 1350 50  0001 C CNN
F 4 "C13585" H 8250 1350 50  0001 C CNN "JLCPCB Part"
F 5 "0.07" H 8250 1350 50  0001 C CNN "JLCPCB UP"
F 6 "" H 8250 1350 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 8250 1350 50  0001 C CNN "Purchase Link"
	1    8250 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 950  8250 950 
Wire Wire Line
	8250 950  8250 1200
Connection ~ 7900 950 
Wire Wire Line
	8250 1500 8250 1750
Wire Wire Line
	8250 1750 7900 1750
Connection ~ 7900 1750
$Comp
L power:+3.3V #PWR027
U 1 1 614D0516
P 8250 850
F 0 "#PWR027" H 8250 700 50  0001 C CNN
F 1 "+3.3V" H 8265 1023 50  0000 C CNN
F 2 "" H 8250 850 50  0001 C CNN
F 3 "" H 8250 850 50  0001 C CNN
	1    8250 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	8250 850  8250 950 
Connection ~ 8250 950 
Wire Wire Line
	6350 1550 6450 1550
Wire Wire Line
	6450 1550 6450 1750
Wire Wire Line
	7000 1750 7900 1750
Wire Notes Line
	8600 600  8600 2650
Wire Notes Line
	8600 2650 4050 2650
Wire Notes Line
	4050 600  8600 600 
Wire Notes Line
	4050 600  4050 2650
Text Notes 8300 950  0    50   ~ 0
3.47v
$Comp
L TPS54331DR:TPS54331DR U5
U 1 1 615282A6
P 5250 3150
F 0 "U5" H 5850 3415 50  0000 C CNN
F 1 "TPS54331DR" H 5850 3324 50  0000 C CNN
F 2 "TPS54331DR:Texas_Instruments-TPS54331DR-Level_A" H 6150 2800 50  0001 C CNN
F 3 "https://www.ti.com/lit/ds/symlink/tps54331.pdf" H 6250 2700 50  0001 C CNN
F 4 "C9865" H 5250 3150 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 5250 3150 50  0001 C CNN "Purchase Link"
	1    5250 3150
	1    0    0    -1  
$EndComp
Text Notes 4050 2750 0    50   ~ 0
5v TPS5430
$Comp
L Device:C C8
U 1 1 615287CA
P 4200 3350
F 0 "C8" H 4315 3396 50  0000 L CNN
F 1 "10uF" H 4315 3305 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 4238 3200 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810221112_Samsung-Electro-Mechanics-CL31A106KBHNNNE_C13585.pdf" H 4200 3350 50  0001 C CNN
F 4 "C13585" H 4200 3350 50  0001 C CNN "JLCPCB Part"
F 5 "0.07" H 4200 3350 50  0001 C CNN "JLCPCB UP"
F 6 "" H 4200 3350 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 4200 3350 50  0001 C CNN "Purchase Link"
	1    4200 3350
	1    0    0    -1  
$EndComp
$Comp
L power:+24V #PWR013
U 1 1 615287D4
P 4200 3050
F 0 "#PWR013" H 4200 2900 50  0001 C CNN
F 1 "+24V" H 4215 3223 50  0000 C CNN
F 2 "" H 4200 3050 50  0001 C CNN
F 3 "" H 4200 3050 50  0001 C CNN
	1    4200 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 3050 4200 3150
NoConn ~ 5350 3350
$Comp
L power:GND #PWR014
U 1 1 615287E0
P 4200 4600
F 0 "#PWR014" H 4200 4350 50  0001 C CNN
F 1 "GND" H 4205 4427 50  0000 C CNN
F 2 "" H 4200 4600 50  0001 C CNN
F 3 "" H 4200 4600 50  0001 C CNN
	1    4200 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 3500 4200 3550
Wire Wire Line
	5350 3150 4200 3150
Connection ~ 4200 3150
Wire Wire Line
	4200 3150 4200 3200
$Comp
L Device:C C12
U 1 1 615287EF
P 4900 3550
F 0 "C12" V 4648 3550 50  0000 C CNN
F 1 "8.2nF" V 4739 3550 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4938 3400 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810262007_Samsung-Electro-Mechanics-CL10B822KB8NNNC_C27920.pdf" H 4900 3550 50  0001 C CNN
F 4 "C27920" H 4900 3550 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 4900 3550 50  0001 C CNN "Purchase Link"
	1    4900 3550
	0    1    1    0   
$EndComp
Wire Wire Line
	5050 3550 5350 3550
Connection ~ 4200 3550
Wire Wire Line
	4200 3550 4200 4550
$Comp
L Device:R R9
U 1 1 615287FD
P 5200 4000
F 0 "R9" H 5270 4046 50  0000 L CNN
F 1 "20k" H 5270 3955 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 5130 4000 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811132312_UNI-ROYAL-Uniroyal-Elec-0603WAF2002T5E_C4184.pdf" H 5200 4000 50  0001 C CNN
F 4 "C4184" H 5200 4000 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 5200 4000 50  0001 C CNN "Purchase Link"
	1    5200 4000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C14
U 1 1 61528808
P 5200 4350
F 0 "C14" H 5315 4396 50  0000 L CNN
F 1 "1.5nF" H 5315 4305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 5238 4200 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810191211_Samsung-Electro-Mechanics-CL05B152KB5NNNC_C23967.pdf" H 5200 4350 50  0001 C CNN
F 4 "C23967" H 5200 4350 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 5200 4350 50  0001 C CNN "Purchase Link"
	1    5200 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 3750 5200 3750
Wire Wire Line
	5200 3750 5200 3850
Wire Wire Line
	5200 3750 4650 3750
Connection ~ 5200 3750
Wire Wire Line
	4650 4550 5200 4550
Wire Wire Line
	5200 4550 5200 4500
Wire Wire Line
	4650 4550 4200 4550
Connection ~ 4650 4550
Connection ~ 4200 4550
Wire Wire Line
	4200 4550 4200 4600
Wire Wire Line
	4200 3550 4750 3550
Wire Wire Line
	4650 4250 4650 4550
Wire Wire Line
	4650 3750 4650 3950
$Comp
L Device:C C10
U 1 1 61528820
P 4650 4100
F 0 "C10" H 4765 4146 50  0000 L CNN
F 1 "15pF" H 4765 4055 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4688 3950 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810261515_Samsung-Electro-Mechanics-CL10C150JB8NNNC_C1644.pdf" H 4650 4100 50  0001 C CNN
F 4 "C1644" H 4650 4100 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 4650 4100 50  0001 C CNN "Purchase Link"
	1    4650 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 4150 5200 4200
$Comp
L Device:C C16
U 1 1 6152882E
P 6600 3350
F 0 "C16" V 6348 3350 50  0000 C CNN
F 1 "100nF" V 6439 3350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6638 3200 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301912_YAGEO-CC0603KRX7R9BB104_C14663.pdf" H 6600 3350 50  0001 C CNN
F 4 "C14663" V 6600 3350 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 6600 3350 50  0001 C CNN "JLCPCB UP"
F 6 "" H 6600 3350 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 6600 3350 50  0001 C CNN "Purchase Link"
	1    6600 3350
	0    1    1    0   
$EndComp
Wire Wire Line
	6450 3350 6350 3350
Wire Wire Line
	6750 3350 6850 3350
Wire Wire Line
	6850 3350 6850 3150
Connection ~ 6850 3150
Wire Wire Line
	6850 3150 7000 3150
Wire Wire Line
	6350 3150 6850 3150
$Comp
L Device:D_Schottky D4
U 1 1 61528844
P 7000 3300
F 0 "D4" V 6954 3380 50  0000 L CNN
F 1 "40V 1A" V 7045 3380 50  0000 L CNN
F 2 "Diode_SMD:D_SMA" H 7000 3300 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/2105061432_MDD%EF%BC%88Microdiode-Electronics%EF%BC%89-SS14_C2480.pdf" H 7000 3300 50  0001 C CNN
F 4 "" H 7000 3300 50  0001 C CNN "JBCPCB Part"
F 5 "" H 7000 3300 50  0001 C CNN "JBCPCB UP"
F 6 "" H 7000 3300 50  0001 C CNN "JCBPCB Basic"
F 7 "" H 7000 3300 50  0001 C CNN "JLCPCB Basic"
F 8 "C2480" H 7000 3300 50  0001 C CNN "JLCPCB Part"
F 9 "0.05" H 7000 3300 50  0001 C CNN "JLCPCB UP"
F 10 "NA" H 7000 3300 50  0001 C CNN "Purchase Link"
	1    7000 3300
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR024
U 1 1 6152884E
P 7000 4000
F 0 "#PWR024" H 7000 3750 50  0001 C CNN
F 1 "GND" H 7005 3827 50  0000 C CNN
F 2 "" H 7000 4000 50  0001 C CNN
F 3 "" H 7000 4000 50  0001 C CNN
	1    7000 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	7000 3450 7000 3950
Connection ~ 7000 3150
$Comp
L Device:L L2
U 1 1 6152885B
P 7650 3150
F 0 "L2" V 7840 3150 50  0000 C CNN
F 1 "4.7uH" V 7749 3150 50  0000 C CNN
F 2 "Capacitor_THT:C_Radial_D5.0mm_H5.0mm_P2.00mm" H 7650 3150 50  0001 C CNN
F 3 "https://au.mouser.com/datasheet/2/281/kmp_1100r-48592.pdf" H 7650 3150 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/Murata-Power-Solutions/11R472C?qs=sGAEpiMZZMv126LJFLh8y6dg16FCDTo2uuZAWc3xT0U%3D" H 7650 3150 50  0001 C CNN "Purchase Link"
	1    7650 3150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7000 3150 7500 3150
$Comp
L Device:R R18
U 1 1 61528869
P 7900 3350
F 0 "R18" H 7970 3396 50  0000 L CNN
F 1 "10k" H 7970 3305 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 7830 3350 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 7900 3350 50  0001 C CNN
F 4 "C25744" H 7900 3350 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 7900 3350 50  0001 C CNN "JLCPCB UP"
F 6 "" H 7900 3350 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 7900 3350 50  0001 C CNN "Purchase Link"
	1    7900 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 3150 7900 3150
Wire Wire Line
	7900 3150 7900 3200
Wire Wire Line
	6450 3950 7000 3950
Connection ~ 7000 3950
Wire Wire Line
	7000 3950 7000 4000
Wire Wire Line
	6350 3550 7900 3550
Wire Wire Line
	7900 3550 7900 3500
$Comp
L Device:R R19
U 1 1 6152887B
P 7900 3750
F 0 "R19" H 7970 3796 50  0000 L CNN
F 1 "1.8k" H 7970 3705 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 7830 3750 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811081714_UNI-ROYAL-Uniroyal-Elec-0603WAF1801T5E_C4177.pdf" H 7900 3750 50  0001 C CNN
F 4 "C4177" H 7900 3750 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 7900 3750 50  0001 C CNN "Purchase Link"
	1    7900 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 3550 7900 3600
Connection ~ 7900 3550
Wire Wire Line
	7900 3900 7900 3950
$Comp
L Device:C C18
U 1 1 6152888B
P 8250 3550
F 0 "C18" H 8365 3596 50  0000 L CNN
F 1 "10uF" H 8365 3505 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 8288 3400 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810221112_Samsung-Electro-Mechanics-CL31A106KBHNNNE_C13585.pdf" H 8250 3550 50  0001 C CNN
F 4 "C13585" H 8250 3550 50  0001 C CNN "JLCPCB Part"
F 5 "0.07" H 8250 3550 50  0001 C CNN "JLCPCB UP"
F 6 "" H 8250 3550 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 8250 3550 50  0001 C CNN "Purchase Link"
	1    8250 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 3150 8250 3150
Wire Wire Line
	8250 3150 8250 3400
Connection ~ 7900 3150
Wire Wire Line
	8250 3700 8250 3950
Wire Wire Line
	8250 3950 7900 3950
Connection ~ 7900 3950
Wire Wire Line
	8250 3050 8250 3150
Connection ~ 8250 3150
Wire Wire Line
	6350 3750 6450 3750
Wire Wire Line
	6450 3750 6450 3950
Wire Wire Line
	7000 3950 7900 3950
Wire Notes Line
	8600 2800 8600 4850
Wire Notes Line
	8600 4850 4050 4850
Wire Notes Line
	4050 2800 8600 2800
Wire Notes Line
	4050 2800 4050 4850
Text Notes 8300 3150 0    50   ~ 0
5.24v
$Comp
L power:+5V #PWR028
U 1 1 61541CF1
P 8250 3050
F 0 "#PWR028" H 8250 2900 50  0001 C CNN
F 1 "+5V" H 8265 3223 50  0000 C CNN
F 2 "" H 8250 3050 50  0001 C CNN
F 3 "" H 8250 3050 50  0001 C CNN
	1    8250 3050
	1    0    0    -1  
$EndComp
Text Notes 3500 4950 0    50   ~ 0
5v TPS5430
$Comp
L Interface_UART:MAX485E U6
U 1 1 6160463C
P 5700 6850
F 0 "U6" H 5700 7531 50  0000 C CNN
F 1 "MAX485" H 5700 7440 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 5700 6150 50  0001 C CNN
F 3 "https://datasheets.maximintegrated.com/en/ds/MAX1487E-MAX491E.pdf" H 5700 6900 50  0001 C CNN
F 4 "https://au.element14.com/maxim-integrated-products/max485cpa/ic-cmos-bus-transceiver-dip8-485/dp/2519431?st=max485" H 5700 6850 50  0001 C CNN "Purchase Link"
	1    5700 6850
	1    0    0    -1  
$EndComp
Text GLabel 2250 2750 2    50   Input ~ 0
AIO4
Text GLabel 2250 2850 2    50   Input ~ 0
AIO5
Text GLabel 6500 6750 2    50   Input ~ 0
RS485B
Text GLabel 10700 3800 3    50   Input ~ 0
AIO4
Text GLabel 2250 2950 2    50   Input ~ 0
ARSDE
Text GLabel 2250 3050 2    50   Input ~ 0
ARSRE
Text GLabel 5300 5600 0    50   Input ~ 0
ARSDE
Wire Wire Line
	6000 6350 5700 6350
$Comp
L power:+5V #PWR021
U 1 1 6162E314
P 6000 6350
F 0 "#PWR021" H 6000 6200 50  0001 C CNN
F 1 "+5V" H 6015 6523 50  0000 C CNN
F 2 "" H 6000 6350 50  0001 C CNN
F 3 "" H 6000 6350 50  0001 C CNN
	1    6000 6350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR019
U 1 1 61636142
P 5700 7500
F 0 "#PWR019" H 5700 7250 50  0001 C CNN
F 1 "GND" H 5705 7327 50  0000 C CNN
F 2 "" H 5700 7500 50  0001 C CNN
F 3 "" H 5700 7500 50  0001 C CNN
	1    5700 7500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 7450 5700 7500
$Comp
L Device:R R12
U 1 1 6163E99B
P 6250 6900
F 0 "R12" H 6320 6946 50  0000 L CNN
F 1 "120" H 6320 6855 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6180 6900 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811021215_UNI-ROYAL-Uniroyal-Elec-0603WAF1200T5E_C22787.pdf" H 6250 6900 50  0001 C CNN
F 4 "C22787" H 6250 6900 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 6250 6900 50  0001 C CNN "Purchase Link"
	1    6250 6900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 6750 6250 6750
Wire Wire Line
	6100 7050 6250 7050
Wire Wire Line
	6250 6750 6500 6750
Connection ~ 6250 6750
Wire Wire Line
	6500 7050 6250 7050
Connection ~ 6250 7050
$Comp
L Transistor_FET:2N7002 Q3
U 1 1 61695B19
P 4600 5500
F 0 "Q3" V 4849 5500 50  0000 C CNN
F 1 "2N7002" V 4940 5500 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4800 5425 50  0001 L CIN
F 3 "https://datasheet.lcsc.com/lcsc/1810151612_Changjiang-Electronics-Tech--CJ-2N7002_C8545.pdf" H 4600 5500 50  0001 L CNN
F 4 "C8545" H 4600 5500 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 4600 5500 50  0001 C CNN "Purchase Link"
	1    4600 5500
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR015
U 1 1 616AE4F4
P 4200 5250
F 0 "#PWR015" H 4200 5100 50  0001 C CNN
F 1 "+3.3V" H 4215 5423 50  0000 C CNN
F 2 "" H 4200 5250 50  0001 C CNN
F 3 "" H 4200 5250 50  0001 C CNN
	1    4200 5250
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR017
U 1 1 616B7EFD
P 4850 5250
F 0 "#PWR017" H 4850 5100 50  0001 C CNN
F 1 "+5V" H 4865 5423 50  0000 C CNN
F 2 "" H 4850 5250 50  0001 C CNN
F 3 "" H 4850 5250 50  0001 C CNN
	1    4850 5250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 616B8783
P 4200 5400
F 0 "R6" H 4270 5446 50  0000 L CNN
F 1 "10k" H 4270 5355 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4130 5400 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 4200 5400 50  0001 C CNN
F 4 "C25744" H 4200 5400 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 4200 5400 50  0001 C CNN "JLCPCB UP"
F 6 "NA" H 4200 5400 50  0001 C CNN "Purchase Link"
	1    4200 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 5550 4200 5600
Wire Wire Line
	4200 5600 4400 5600
Wire Wire Line
	4200 5600 4150 5600
Connection ~ 4200 5600
Wire Wire Line
	4600 5250 4600 5300
Wire Wire Line
	4200 5250 4600 5250
Connection ~ 4200 5250
$Comp
L Device:R R7
U 1 1 61705727
P 4850 5400
F 0 "R7" H 4920 5446 50  0000 L CNN
F 1 "10k" H 4920 5355 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4780 5400 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 4850 5400 50  0001 C CNN
F 4 "C25744" H 4850 5400 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 4850 5400 50  0001 C CNN "JLCPCB UP"
F 6 "" H 4850 5400 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 4850 5400 50  0001 C CNN "Purchase Link"
	1    4850 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 5550 4850 5600
Wire Wire Line
	4850 5600 4800 5600
$Comp
L Transistor_FET:2N7002 Q4
U 1 1 61729E08
P 5750 5500
F 0 "Q4" V 5999 5500 50  0000 C CNN
F 1 "2N7002" V 6090 5500 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 5950 5425 50  0001 L CIN
F 3 "https://datasheet.lcsc.com/lcsc/1810151612_Changjiang-Electronics-Tech--CJ-2N7002_C8545.pdf" H 5750 5500 50  0001 L CNN
F 4 "C8545" H 5750 5500 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 5750 5500 50  0001 C CNN "Purchase Link"
	1    5750 5500
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR018
U 1 1 61729E12
P 5350 5250
F 0 "#PWR018" H 5350 5100 50  0001 C CNN
F 1 "+3.3V" H 5365 5423 50  0000 C CNN
F 2 "" H 5350 5250 50  0001 C CNN
F 3 "" H 5350 5250 50  0001 C CNN
	1    5350 5250
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR020
U 1 1 61729E1C
P 6000 5250
F 0 "#PWR020" H 6000 5100 50  0001 C CNN
F 1 "+5V" H 6015 5423 50  0000 C CNN
F 2 "" H 6000 5250 50  0001 C CNN
F 3 "" H 6000 5250 50  0001 C CNN
	1    6000 5250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R10
U 1 1 61729E28
P 5350 5400
F 0 "R10" H 5420 5446 50  0000 L CNN
F 1 "10k" H 5420 5355 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5280 5400 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 5350 5400 50  0001 C CNN
F 4 "C25744" H 5350 5400 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 5350 5400 50  0001 C CNN "JLCPCB UP"
F 6 "NA" H 5350 5400 50  0001 C CNN "Purchase Link"
	1    5350 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 5550 5350 5600
Wire Wire Line
	5350 5600 5550 5600
Wire Wire Line
	5350 5600 5300 5600
Connection ~ 5350 5600
Wire Wire Line
	5750 5250 5750 5300
Wire Wire Line
	5350 5250 5750 5250
Connection ~ 5350 5250
$Comp
L Device:R R11
U 1 1 61729E3C
P 6000 5400
F 0 "R11" H 6070 5446 50  0000 L CNN
F 1 "10k" H 6070 5355 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5930 5400 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 6000 5400 50  0001 C CNN
F 4 "C25744" H 6000 5400 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 6000 5400 50  0001 C CNN "JLCPCB UP"
F 6 "" H 6000 5400 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 6000 5400 50  0001 C CNN "Purchase Link"
	1    6000 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 5550 6000 5600
Wire Wire Line
	6000 5600 5950 5600
Text GLabel 6500 5600 0    50   Input ~ 0
ARSRE
$Comp
L Transistor_FET:2N7002 Q5
U 1 1 6175791F
P 6950 5500
F 0 "Q5" V 7199 5500 50  0000 C CNN
F 1 "2N7002" V 7290 5500 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 7150 5425 50  0001 L CIN
F 3 "https://datasheet.lcsc.com/lcsc/1810151612_Changjiang-Electronics-Tech--CJ-2N7002_C8545.pdf" H 6950 5500 50  0001 L CNN
F 4 "C8545" H 6950 5500 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 6950 5500 50  0001 C CNN "Purchase Link"
	1    6950 5500
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR022
U 1 1 61757929
P 6550 5250
F 0 "#PWR022" H 6550 5100 50  0001 C CNN
F 1 "+3.3V" H 6565 5423 50  0000 C CNN
F 2 "" H 6550 5250 50  0001 C CNN
F 3 "" H 6550 5250 50  0001 C CNN
	1    6550 5250
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR025
U 1 1 61757933
P 7200 5250
F 0 "#PWR025" H 7200 5100 50  0001 C CNN
F 1 "+5V" H 7215 5423 50  0000 C CNN
F 2 "" H 7200 5250 50  0001 C CNN
F 3 "" H 7200 5250 50  0001 C CNN
	1    7200 5250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R13
U 1 1 6175793F
P 6550 5400
F 0 "R13" H 6620 5446 50  0000 L CNN
F 1 "10k" H 6620 5355 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6480 5400 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 6550 5400 50  0001 C CNN
F 4 "C25744" H 6550 5400 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 6550 5400 50  0001 C CNN "JLCPCB UP"
F 6 "NA" H 6550 5400 50  0001 C CNN "Purchase Link"
	1    6550 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6550 5550 6550 5600
Wire Wire Line
	6550 5600 6750 5600
Wire Wire Line
	6550 5600 6500 5600
Connection ~ 6550 5600
Wire Wire Line
	6950 5250 6950 5300
Wire Wire Line
	6550 5250 6950 5250
Connection ~ 6550 5250
$Comp
L Device:R R14
U 1 1 61757953
P 7200 5400
F 0 "R14" H 7270 5446 50  0000 L CNN
F 1 "10k" H 7270 5355 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 7130 5400 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 7200 5400 50  0001 C CNN
F 4 "C25744" H 7200 5400 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 7200 5400 50  0001 C CNN "JLCPCB UP"
F 6 "" H 7200 5400 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 7200 5400 50  0001 C CNN "Purchase Link"
	1    7200 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	7200 5550 7200 5600
Wire Wire Line
	7200 5600 7150 5600
$Comp
L Transistor_FET:2N7002 Q6
U 1 1 617BF354
P 8100 5500
F 0 "Q6" V 8349 5500 50  0000 C CNN
F 1 "2N7002" V 8440 5500 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 8300 5425 50  0001 L CIN
F 3 "https://datasheet.lcsc.com/lcsc/1810151612_Changjiang-Electronics-Tech--CJ-2N7002_C8545.pdf" H 8100 5500 50  0001 L CNN
F 4 "C8545" H 8100 5500 50  0001 C CNN "JLCPCB Part"
F 5 "NA" H 8100 5500 50  0001 C CNN "Purchase Link"
	1    8100 5500
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR026
U 1 1 617BF35E
P 7700 5250
F 0 "#PWR026" H 7700 5100 50  0001 C CNN
F 1 "+3.3V" H 7715 5423 50  0000 C CNN
F 2 "" H 7700 5250 50  0001 C CNN
F 3 "" H 7700 5250 50  0001 C CNN
	1    7700 5250
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR029
U 1 1 617BF368
P 8350 5250
F 0 "#PWR029" H 8350 5100 50  0001 C CNN
F 1 "+5V" H 8365 5423 50  0000 C CNN
F 2 "" H 8350 5250 50  0001 C CNN
F 3 "" H 8350 5250 50  0001 C CNN
	1    8350 5250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R15
U 1 1 617BF374
P 7700 5400
F 0 "R15" H 7770 5446 50  0000 L CNN
F 1 "10k" H 7770 5355 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 7630 5400 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 7700 5400 50  0001 C CNN
F 4 "C25744" H 7700 5400 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 7700 5400 50  0001 C CNN "JLCPCB UP"
F 6 "NA" H 7700 5400 50  0001 C CNN "Purchase Link"
	1    7700 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 5550 7700 5600
Wire Wire Line
	7700 5600 7900 5600
Wire Wire Line
	7700 5600 7650 5600
Connection ~ 7700 5600
Wire Wire Line
	8100 5250 8100 5300
Wire Wire Line
	7700 5250 8100 5250
Connection ~ 7700 5250
$Comp
L Device:R R20
U 1 1 617BF388
P 8350 5400
F 0 "R20" H 8420 5446 50  0000 L CNN
F 1 "10k" H 8420 5355 50  0000 L CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 8280 5400 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1809301717_UNI-ROYAL-Uniroyal-Elec-0402WGF1002TCE_C25744.pdf" H 8350 5400 50  0001 C CNN
F 4 "C25744" H 8350 5400 50  0001 C CNN "JLCPCB Part"
F 5 "0.01" H 8350 5400 50  0001 C CNN "JLCPCB UP"
F 6 "" H 8350 5400 50  0001 C CNN "JLCPCB Basic"
F 7 "NA" H 8350 5400 50  0001 C CNN "Purchase Link"
	1    8350 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	8350 5550 8350 5600
Wire Wire Line
	8350 5600 8300 5600
Wire Wire Line
	6000 5600 6000 5900
Wire Wire Line
	6000 5900 5050 5900
Connection ~ 6000 5600
Wire Wire Line
	7200 5600 7200 6000
Connection ~ 7200 5600
Wire Wire Line
	8350 5600 8350 6100
Connection ~ 8350 5600
Wire Wire Line
	5300 7050 4950 7050
Wire Wire Line
	5250 6100 5250 6750
Wire Wire Line
	5250 6750 5300 6750
Wire Wire Line
	5150 6000 5150 6850
Wire Wire Line
	5150 6850 5300 6850
Wire Wire Line
	5050 6950 5300 6950
Wire Wire Line
	5050 5900 5050 6950
Wire Wire Line
	4950 7050 4950 5900
Wire Wire Line
	4950 5900 4850 5900
Wire Wire Line
	4850 5900 4850 5600
Connection ~ 4850 5600
Wire Notes Line
	6900 7750 6900 6450
Wire Notes Line
	8600 6450 8600 5000
Text GLabel 9250 3800 3    50   Input ~ 0
RS485B
Text GLabel 9350 3800 3    50   Input ~ 0
RS485A
NoConn ~ 1050 2050
NoConn ~ 1050 2150
$Comp
L Connector:Conn_01x02_Male J7
U 1 1 613ABED8
P 10300 900
F 0 "J7" V 10135 878 50  0000 C CNN
F 1 "RELAY_1" V 10226 878 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 10300 900 50  0001 C CNN
F 3 "~" H 10300 900 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 10300 900 50  0001 C CNN "Purchase Link"
	1    10300 900 
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x02_Male J12
U 1 1 613DF424
P 10700 900
F 0 "J12" V 10535 878 50  0000 C CNN
F 1 "RELAY_2" V 10626 878 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 10700 900 50  0001 C CNN
F 3 "~" H 10700 900 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 10700 900 50  0001 C CNN "Purchase Link"
	1    10700 900 
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x03_Male J8
U 1 1 613F8F16
P 10300 1750
F 0 "J8" V 10135 1728 50  0000 C CNN
F 1 "RELAY_3A" V 10226 1728 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B3B-XH-A_1x03_P2.50mm_Vertical" H 10300 1750 50  0001 C CNN
F 3 "~" H 10300 1750 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 10300 1750 50  0001 C CNN "Purchase Link"
	1    10300 1750
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x03_Male J13
U 1 1 6142E79F
P 10750 1750
F 0 "J13" V 10585 1728 50  0000 C CNN
F 1 "RELAY_3B" V 10676 1728 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B3B-XH-A_1x03_P2.50mm_Vertical" H 10750 1750 50  0001 C CNN
F 3 "~" H 10750 1750 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 10750 1750 50  0001 C CNN "Purchase Link"
	1    10750 1750
	0    1    1    0   
$EndComp
Wire Wire Line
	9600 3150 8900 3150
Text GLabel 9200 1100 3    50   Input ~ 0
AMOSI
Text GLabel 9300 1100 3    50   Input ~ 0
AMISO
Text GLabel 9400 1100 3    50   Input ~ 0
ASCK
Text GLabel 9300 1950 3    50   Input ~ 0
ARXD
Text GLabel 9200 1950 3    50   Input ~ 0
ATXD
Text GLabel 9500 1100 3    50   Input ~ 0
ARESET
$Comp
L power:+3.3V #PWR030
U 1 1 61384461
P 8900 1100
F 0 "#PWR030" H 8900 950 50  0001 C CNN
F 1 "+3.3V" H 8915 1273 50  0000 C CNN
F 2 "" H 8900 1100 50  0001 C CNN
F 3 "" H 8900 1100 50  0001 C CNN
	1    8900 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 1200 9100 1200
Wire Wire Line
	9100 1200 9100 1100
Wire Wire Line
	9600 1200 9600 1100
Text GLabel 9400 1950 3    50   Input ~ 0
ADTR
$Comp
L Connector:Conn_01x06_Male J3
U 1 1 6137F4B1
P 9400 900
F 0 "J3" V 9200 800 50  0000 L CNN
F 1 "ATMEGA ICSP" V 9300 600 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 9400 900 50  0001 C CNN
F 3 "~" H 9400 900 50  0001 C CNN
F 4 "https://au.element14.com/amphenol-icc/77313-801-20lf/conn-header-20pos-2row-2-54mm/dp/3728842" H 9400 900 50  0001 C CNN "Purchase Link"
	1    9400 900 
	0    1    1    0   
$EndComp
Text GLabel 9500 1950 3    50   Input ~ 0
ARESET
$Comp
L Connector:Conn_01x07_Male J5
U 1 1 6138A907
P 9400 2550
F 0 "J5" V 9235 2528 50  0000 C CNN
F 1 "RF Module" V 9326 2528 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x07_P2.54mm_Vertical" H 9400 2550 50  0001 C CNN
F 3 "~" H 9400 2550 50  0001 C CNN
F 4 "https://au.element14.com/amphenol-icc/10129378-920003blf/conn-header-20pos-1row-2-54mm/dp/3728430" H 9400 2550 50  0001 C CNN "Purchase Link"
	1    9400 2550
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x06_Male J4
U 1 1 613922D9
P 9400 1750
F 0 "J4" V 9235 1728 50  0000 C CNN
F 1 "ATMEGA UART" V 9326 1728 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 9400 1750 50  0001 C CNN
F 3 "~" H 9400 1750 50  0001 C CNN
F 4 "https://au.element14.com/amphenol-icc/77313-801-20lf/conn-header-20pos-2row-2-54mm/dp/3728842" H 9400 1750 50  0001 C CNN "Purchase Link"
	1    9400 1750
	0    1    1    0   
$EndComp
Wire Wire Line
	8900 2000 9100 2000
Wire Wire Line
	9100 2000 9100 1950
Wire Wire Line
	9800 1200 9600 1200
Wire Wire Line
	9600 1950 9800 1950
Wire Wire Line
	9700 2750 9700 2850
Wire Wire Line
	9700 2850 9800 2850
Text GLabel 9500 2750 3    50   Input ~ 0
RFEN
Text GLabel 9400 2750 3    50   Input ~ 0
ATXD
Text GLabel 9300 2750 3    50   Input ~ 0
ARXD
Text GLabel 9200 2750 3    50   Input ~ 0
RFAUX
Wire Wire Line
	9600 2750 9600 3150
Text GLabel 9100 2750 3    50   Input ~ 0
RFSET
$Comp
L power:GND #PWR038
U 1 1 614A1B8E
P 9800 2050
F 0 "#PWR038" H 9800 1800 50  0001 C CNN
F 1 "GND" H 9805 1877 50  0000 C CNN
F 2 "" H 9800 2050 50  0001 C CNN
F 3 "" H 9800 2050 50  0001 C CNN
	1    9800 2050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR037
U 1 1 614A3FA6
P 9800 1300
F 0 "#PWR037" H 9800 1050 50  0001 C CNN
F 1 "GND" H 9805 1127 50  0000 C CNN
F 2 "" H 9800 1300 50  0001 C CNN
F 3 "" H 9800 1300 50  0001 C CNN
	1    9800 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	9800 1200 9800 1300
Wire Wire Line
	9800 1950 9800 2050
$Comp
L power:+3.3V #PWR031
U 1 1 614A8723
P 8900 1900
F 0 "#PWR031" H 8900 1750 50  0001 C CNN
F 1 "+3.3V" H 8915 2073 50  0000 C CNN
F 2 "" H 8900 1900 50  0001 C CNN
F 3 "" H 8900 1900 50  0001 C CNN
	1    8900 1900
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR032
U 1 1 614A9053
P 8900 3050
F 0 "#PWR032" H 8900 2900 50  0001 C CNN
F 1 "+3.3V" H 8915 3223 50  0000 C CNN
F 2 "" H 8900 3050 50  0001 C CNN
F 3 "" H 8900 3050 50  0001 C CNN
	1    8900 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 3150 8900 3050
Wire Wire Line
	8900 2000 8900 1900
Wire Wire Line
	8900 1100 8900 1200
$Comp
L Connector:Conn_01x03_Male J1
U 1 1 614FE796
P 9250 3600
F 0 "J1" V 9085 3578 50  0000 C CNN
F 1 "RS485" V 9176 3578 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B3B-XH-A_1x03_P2.50mm_Vertical" H 9250 3600 50  0001 C CNN
F 3 "~" H 9250 3600 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 9250 3600 50  0001 C CNN "Purchase Link"
	1    9250 3600
	0    1    1    0   
$EndComp
Wire Wire Line
	9150 3800 9150 3900
$Comp
L Connector:Conn_01x04_Male J2
U 1 1 6153C961
P 9350 4550
F 0 "J2" V 9185 4528 50  0000 C CNN
F 1 "I2C_1" V 9276 4528 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B4B-XH-A_1x04_P2.50mm_Vertical" H 9350 4550 50  0001 C CNN
F 3 "~" H 9350 4550 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 9350 4550 50  0001 C CNN "Purchase Link"
	1    9350 4550
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR033
U 1 1 6153EEDD
P 8950 4750
F 0 "#PWR033" H 8950 4600 50  0001 C CNN
F 1 "+3.3V" H 8965 4923 50  0000 C CNN
F 2 "" H 8950 4750 50  0001 C CNN
F 3 "" H 8950 4750 50  0001 C CNN
	1    8950 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 4750 8950 4850
Wire Wire Line
	8950 4850 9150 4850
Wire Wire Line
	9150 4850 9150 4750
$Comp
L power:GND #PWR036
U 1 1 6154C177
P 9500 4850
F 0 "#PWR036" H 9500 4600 50  0001 C CNN
F 1 "GND" H 9505 4677 50  0000 C CNN
F 2 "" H 9500 4850 50  0001 C CNN
F 3 "" H 9500 4850 50  0001 C CNN
	1    9500 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	9450 4750 9450 4800
Wire Wire Line
	9450 4800 9500 4800
Wire Wire Line
	9500 4800 9500 4850
$Comp
L power:+3.3V #PWR040
U 1 1 6161AE6C
P 9900 3800
F 0 "#PWR040" H 9900 3650 50  0001 C CNN
F 1 "+3.3V" H 9915 3973 50  0000 C CNN
F 2 "" H 9900 3800 50  0001 C CNN
F 3 "" H 9900 3800 50  0001 C CNN
	1    9900 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	9900 3800 9900 3900
Wire Wire Line
	9900 3900 10100 3900
Wire Wire Line
	10100 3900 10100 3800
$Comp
L power:GND #PWR044
U 1 1 61629A77
P 10850 3900
F 0 "#PWR044" H 10850 3650 50  0001 C CNN
F 1 "GND" H 10855 3727 50  0000 C CNN
F 2 "" H 10850 3900 50  0001 C CNN
F 3 "" H 10850 3900 50  0001 C CNN
	1    10850 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	10850 3900 10850 3850
Wire Wire Line
	10850 3850 10800 3850
Wire Wire Line
	10800 3850 10800 3800
Text GLabel 10600 3800 3    50   Input ~ 0
AIO5
$Comp
L Connector:Conn_01x08_Male J11
U 1 1 61618367
P 10500 3600
F 0 "J11" V 10300 3500 50  0000 L CNN
F 1 "IO" V 10400 3300 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x04_P2.54mm_Vertical" H 10500 3600 50  0001 C CNN
F 3 "~" H 10500 3600 50  0001 C CNN
F 4 "https://au.element14.com/amphenol-icc/77313-801-20lf/conn-header-20pos-2row-2-54mm/dp/3728842" H 10500 3600 50  0001 C CNN "Purchase Link"
	1    10500 3600
	0    1    1    0   
$EndComp
Wire Notes Line
	500  5000 3450 5000
Wire Notes Line
	500  5000 500  7750
Wire Notes Line
	500  7750 4800 7750
Wire Notes Line
	3500 5000 3500 6000
Wire Notes Line
	3500 6000 4850 6000
Wire Notes Line
	4850 6000 4850 7750
Wire Notes Line
	6900 7750 4850 7750
Wire Notes Line
	3450 5000 3450 6050
Wire Notes Line
	3500 5000 8600 5000
Wire Notes Line
	6900 6450 8600 6450
Wire Wire Line
	2250 6700 2200 6700
Wire Wire Line
	950  6700 1500 6700
Wire Wire Line
	1500 6700 1500 6600
Connection ~ 1500 6700
Wire Wire Line
	1500 6700 1600 6700
Wire Wire Line
	1500 6300 1500 6250
$Comp
L lib:UA2-5NU K2
U 1 1 615A0129
P 4000 7100
F 0 "K2" V 3233 7100 50  0000 C CNN
F 1 "UA2-5NU" V 3324 7100 50  0000 C CNN
F 2 "lib:Relay_DPDT_NEXEM_UA2_Pitch3.2mm" H 4000 7100 50  0001 C CNN
F 3 "https://www.farnell.com/datasheets/2302216.pdf" H 4000 7100 50  0001 C CNN
F 4 "https://au.element14.com/nexem/ua2-5nu/signal-relay-dpdt-1a-250vac-th/dp/2766147" H 4000 7100 50  0001 C CNN "Purchase Link"
	1    4000 7100
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR016
U 1 1 615A0CD1
P 4500 6350
F 0 "#PWR016" H 4500 6200 50  0001 C CNN
F 1 "+5V" H 4515 6523 50  0000 C CNN
F 2 "" H 4500 6350 50  0001 C CNN
F 3 "" H 4500 6350 50  0001 C CNN
	1    4500 6350
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:MMBT3904 Q2
U 1 1 615A0CDF
P 2950 7050
F 0 "Q2" H 3141 7096 50  0000 L CNN
F 1 "MMBT3904" H 3141 7005 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 3150 6975 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/2N3903-D.PDF" H 2950 7050 50  0001 L CNN
F 4 "" H 2950 7050 50  0001 C CNN "JLCPCB Basic"
F 5 "C20526" H 2950 7050 50  0001 C CNN "JLCPCB Part"
F 6 "0.03" H 2950 7050 50  0001 C CNN "JLCPCB UP"
F 7 "NA" H 2950 7050 50  0001 C CNN "Purchase Link"
	1    2950 7050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 615A0CE9
P 3050 7400
F 0 "#PWR09" H 3050 7150 50  0001 C CNN
F 1 "GND" H 3055 7227 50  0000 C CNN
F 2 "" H 3050 7400 50  0001 C CNN
F 3 "" H 3050 7400 50  0001 C CNN
	1    3050 7400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 7400 3050 7250
Wire Wire Line
	3050 6700 3050 6850
$Comp
L Device:D D2
U 1 1 615A0CF9
P 3600 6450
F 0 "D2" H 3600 6233 50  0000 C CNN
F 1 "SM4007PL" H 3600 6324 50  0000 C CNN
F 2 "Diode_SMD:D_SMA" H 3600 6450 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1810311713_MDD%EF%BC%88Microdiode-Electronics%EF%BC%89-M7_C95872.pdf" H 3600 6450 50  0001 C CNN
F 4 "" H 3600 6450 50  0001 C CNN "JLCPCB Basic"
F 5 "C95872" H 3600 6450 50  0001 C CNN "JLCPCB Part"
F 6 "0.03" H 3600 6450 50  0001 C CNN "JLCPCB UP"
F 7 "NA" H 3600 6450 50  0001 C CNN "Purchase Link"
	1    3600 6450
	0    1    1    0   
$EndComp
Text GLabel 2700 6550 1    50   Input ~ 0
AIO4
Wire Wire Line
	2700 6550 2700 6650
Wire Wire Line
	2750 7050 2700 7050
Wire Wire Line
	2700 7050 2700 6950
Text GLabel 3600 7000 1    50   Input ~ 0
R4AC
Wire Wire Line
	3700 7100 3600 7100
Wire Wire Line
	3600 7100 3600 7000
Text GLabel 3600 7400 1    50   Input ~ 0
R4BC
Wire Wire Line
	3700 7500 3600 7500
Wire Wire Line
	3600 7500 3600 7400
Text GLabel 4400 7000 2    50   Input ~ 0
R4ANC
Wire Wire Line
	4300 7000 4400 7000
Text GLabel 4400 7200 2    50   Input ~ 0
R4ANO
Wire Wire Line
	4400 7200 4300 7200
Text GLabel 4400 7400 2    50   Input ~ 0
R4BNC
Wire Wire Line
	4300 7400 4400 7400
Text GLabel 4400 7600 2    50   Input ~ 0
R4BNO
Wire Wire Line
	4400 7600 4300 7600
$Comp
L Device:R R4
U 1 1 615A0D19
P 2700 6800
F 0 "R4" H 2630 6754 50  0000 R CNN
F 1 "2.2k" H 2630 6845 50  0000 R CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 2630 6800 50  0001 C CNN
F 3 "https://datasheet.lcsc.com/lcsc/1811022110_UNI-ROYAL-Uniroyal-Elec-0402WGF2201TCE_C25879.pdf" H 2700 6800 50  0001 C CNN
F 4 "" H 2700 6800 50  0001 C CNN "JLCPCB Basic"
F 5 "C25879" H 2700 6800 50  0001 C CNN "JLCPCB Part"
F 6 "0.01" H 2700 6800 50  0001 C CNN "JLCPCB UP"
F 7 "NA" H 2700 6800 50  0001 C CNN "Purchase Link"
	1    2700 6800
	-1   0    0    1   
$EndComp
Wire Wire Line
	4350 6700 4300 6700
Wire Wire Line
	4350 6250 4350 6450
Wire Wire Line
	3050 6700 3600 6700
Wire Wire Line
	3600 6700 3600 6600
Connection ~ 3600 6700
Wire Wire Line
	3600 6700 3700 6700
Wire Wire Line
	3600 6300 3600 6250
Wire Wire Line
	3600 6250 4350 6250
Wire Notes Line
	4800 7750 4800 6050
Wire Notes Line
	4800 6050 3450 6050
Wire Wire Line
	2250 6250 2250 6450
Wire Wire Line
	1500 6250 2250 6250
Wire Wire Line
	2400 6450 2250 6450
Connection ~ 2250 6450
Wire Wire Line
	2250 6450 2250 6700
Wire Wire Line
	2400 6450 2400 6350
Wire Wire Line
	4500 6350 4500 6450
Wire Wire Line
	4500 6450 4350 6450
Connection ~ 4350 6450
Wire Wire Line
	4350 6450 4350 6700
Wire Wire Line
	9800 2850 9800 2950
$Comp
L power:GND #PWR039
U 1 1 61384F23
P 9800 2950
F 0 "#PWR039" H 9800 2700 50  0001 C CNN
F 1 "GND" H 9805 2777 50  0000 C CNN
F 2 "" H 9800 2950 50  0001 C CNN
F 3 "" H 9800 2950 50  0001 C CNN
	1    9800 2950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR034
U 1 1 61692CC5
P 9050 4000
F 0 "#PWR034" H 9050 3750 50  0001 C CNN
F 1 "GND" H 9055 3827 50  0000 C CNN
F 2 "" H 9050 4000 50  0001 C CNN
F 3 "" H 9050 4000 50  0001 C CNN
	1    9050 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 3900 9050 3900
Wire Wire Line
	9050 3900 9050 4000
Text GLabel 10400 2750 3    50   Input ~ 0
R4AC
Text GLabel 10300 2750 3    50   Input ~ 0
R4ANO
Text GLabel 10200 2750 3    50   Input ~ 0
R4ANC
Text GLabel 10850 2750 3    50   Input ~ 0
R4BC
Text GLabel 10750 2750 3    50   Input ~ 0
R4BNO
Text GLabel 10650 2750 3    50   Input ~ 0
R4BNC
$Comp
L Connector:Conn_01x03_Male J9
U 1 1 6170C91D
P 10300 2550
F 0 "J9" V 10135 2528 50  0000 C CNN
F 1 "RELAY_4A" V 10226 2528 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B3B-XH-A_1x03_P2.50mm_Vertical" H 10300 2550 50  0001 C CNN
F 3 "~" H 10300 2550 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 10300 2550 50  0001 C CNN "Purchase Link"
	1    10300 2550
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x03_Male J14
U 1 1 6170C928
P 10750 2550
F 0 "J14" V 10585 2528 50  0000 C CNN
F 1 "RELAY_4B" V 10676 2528 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B3B-XH-A_1x03_P2.50mm_Vertical" H 10750 2550 50  0001 C CNN
F 3 "~" H 10750 2550 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 10750 2550 50  0001 C CNN "Purchase Link"
	1    10750 2550
	0    1    1    0   
$EndComp
Text Notes 9800 3350 0    50   ~ 0
Note: AI04 shared with RELAY_4
Text GLabel 10300 4750 3    50   Input ~ 0
ASDA
Text GLabel 10400 4750 3    50   Input ~ 0
ASCL
$Comp
L Connector:Conn_01x04_Male J10
U 1 1 618BA1F4
P 10400 4550
F 0 "J10" V 10235 4528 50  0000 C CNN
F 1 "I2C_2" V 10326 4528 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B4B-XH-A_1x04_P2.50mm_Vertical" H 10400 4550 50  0001 C CNN
F 3 "~" H 10400 4550 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 10400 4550 50  0001 C CNN "Purchase Link"
	1    10400 4550
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR042
U 1 1 618BA1FE
P 10000 4750
F 0 "#PWR042" H 10000 4600 50  0001 C CNN
F 1 "+3.3V" H 10015 4923 50  0000 C CNN
F 2 "" H 10000 4750 50  0001 C CNN
F 3 "" H 10000 4750 50  0001 C CNN
	1    10000 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 4750 10000 4850
Wire Wire Line
	10000 4850 10200 4850
Wire Wire Line
	10200 4850 10200 4750
$Comp
L power:GND #PWR043
U 1 1 618BA20B
P 10550 4850
F 0 "#PWR043" H 10550 4600 50  0001 C CNN
F 1 "GND" H 10555 4677 50  0000 C CNN
F 2 "" H 10550 4850 50  0001 C CNN
F 3 "" H 10550 4850 50  0001 C CNN
	1    10550 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	10500 4750 10500 4800
Wire Wire Line
	10500 4800 10550 4800
Wire Wire Line
	10550 4800 10550 4850
Wire Notes Line
	8650 6450 11150 6450
Wire Notes Line
	11150 600  11150 6450
Wire Notes Line
	8650 600  8650 6450
$Comp
L power:PWR_FLAG #FLG04
U 1 1 61923EF8
P 9950 5700
F 0 "#FLG04" H 9950 5775 50  0001 C CNN
F 1 "PWR_FLAG" H 9950 5873 50  0000 C CNN
F 2 "" H 9950 5700 50  0001 C CNN
F 3 "~" H 9950 5700 50  0001 C CNN
	1    9950 5700
	-1   0    0    1   
$EndComp
$Comp
L power:+24V #PWR041
U 1 1 61924F74
P 9950 5600
F 0 "#PWR041" H 9950 5450 50  0001 C CNN
F 1 "+24V" H 9965 5773 50  0000 C CNN
F 2 "" H 9950 5600 50  0001 C CNN
F 3 "" H 9950 5600 50  0001 C CNN
	1    9950 5600
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J6
U 1 1 61926C7F
P 9750 5400
F 0 "J6" V 9585 5378 50  0000 C CNN
F 1 "PWR" V 9676 5378 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B2B-XH-A_1x02_P2.50mm_Vertical" H 9750 5400 50  0001 C CNN
F 3 "~" H 9750 5400 50  0001 C CNN
F 4 "https://au.mouser.com/ProductDetail/426-FIT0255" V 9750 5400 50  0001 C CNN "Purchase Link"
	1    9750 5400
	0    1    1    0   
$EndComp
Wire Wire Line
	9750 5600 9750 5700
Wire Wire Line
	9750 5700 9950 5700
Wire Wire Line
	9950 5700 9950 5600
Connection ~ 9950 5700
$Comp
L power:GND #PWR035
U 1 1 61962CBA
P 9400 5700
F 0 "#PWR035" H 9400 5450 50  0001 C CNN
F 1 "GND" H 9405 5527 50  0000 C CNN
F 2 "" H 9400 5700 50  0001 C CNN
F 3 "" H 9400 5700 50  0001 C CNN
	1    9400 5700
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG03
U 1 1 619D5328
P 9400 5600
F 0 "#FLG03" H 9400 5675 50  0001 C CNN
F 1 "PWR_FLAG" H 9400 5773 50  0000 C CNN
F 2 "" H 9400 5600 50  0001 C CNN
F 3 "~" H 9400 5600 50  0001 C CNN
	1    9400 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 5600 9400 5700
Wire Wire Line
	9650 5600 9650 5700
Wire Wire Line
	9650 5700 9400 5700
Connection ~ 9400 5700
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
U 1 1 61A113AC
P 6850 3050
F 0 "#FLG02" H 6850 3125 50  0001 C CNN
F 1 "PWR_FLAG" H 6850 3223 50  0000 C CNN
F 2 "" H 6850 3050 50  0001 C CNN
F 3 "~" H 6850 3050 50  0001 C CNN
	1    6850 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	6850 3050 6850 3150
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 61A89071
P 7900 3050
F 0 "#FLG0101" H 7900 3125 50  0001 C CNN
F 1 "PWR_FLAG" H 7900 3223 50  0000 C CNN
F 2 "" H 7900 3050 50  0001 C CNN
F 3 "~" H 7900 3050 50  0001 C CNN
	1    7900 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 3050 7900 3150
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 61A9CB97
P 7900 850
F 0 "#FLG0102" H 7900 925 50  0001 C CNN
F 1 "PWR_FLAG" H 7900 1023 50  0000 C CNN
F 2 "" H 7900 850 50  0001 C CNN
F 3 "~" H 7900 850 50  0001 C CNN
	1    7900 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 850  7900 950 
Wire Wire Line
	5150 6000 7200 6000
Wire Wire Line
	5250 6100 8350 6100
Text Notes 6100 7200 0    50   ~ 0
RS-485 Termination
$EndSCHEMATC
