# P1_reader-smart-meter on ESP32
The purpose is to read the output of the P1 connector of smart-meter






RJ11 connector

P1 pin         —->  ESP32 Pin

2 - RTS	       —->  3.3v

3 - GND	       —->  GND

4 -	

5 - RXD (data) —->	RX (gpio4)




Sagecom T211 , S211 , XT211 and on most Landys and Gyr models a 10K resistor should be used between the ESP's 3.3v and the p1's DATA (RXD) pin. 

pin 5 (Data) ───────┬──────── GPIO4
                    │
                10 kΩ
                    │
                  3V3




