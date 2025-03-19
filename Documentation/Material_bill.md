(Potential) 

 - **Servo MG996R: x18**
from this [datasheet](https://www.electronicoscaldas.com/datasheet/MG996R_Tower-Pro.pdf?srsltid=AfmBOoqqGq_SDS5HIzFjyFYCqbdbtuzulfdJUsTXTpc_476yi_19Uy7X):
-weight : 55g
-specs: 11 kgf·cm - 120 degrees (60 degrees in each direction)
 -voltage: 4.8 to 7.2V
 -current: 500mA to 900mA
 -price: for all <90CHF

(Current choice of servos)    

 - **Servo SG90: x18**
from this [datasheet](https://www.kjell.com/globalassets/mediaassets/701916_87897_datasheet_en.pdf?ref=4287817A7A):
-weight : 11g
-specs: 2.5 kgf·cm - 180 degrees (90 degrees in each direction)
 -voltage: 4.8 to 6V
 -current: <600 mA.
 -3CHF
    
 - **Power supply:**
18 servos at an average draw of 300mA per servo would require 600mA x 18 = 10800mA (10.8A)
Voltage: 7.4V (2S) or 11.1V (3S)
Capacity: 5000mAh - 8000mAh
C Rating: 20C
<5000mAh (or smaller mAh capacity) 3S 20C LiPo battery (for both)
-weight: ~400g (depends on model)
-Price : 40-50 bucks
    ie :  [4000mAh 11.1V](https://www.conrad.ch/fr/p/reely-pack-de-batterie-lipo-11-1-v-4000-mah-nombre-de-cellules-3-20-c-hardcase-systeme-d-enfichage-en-t-2617160.html)
    

 To step down the voltage of the battery:
DC DC converter
-<10g
 -<15CHF (i think)
 -input : battery voltage
  -output : 5V
    
(depends which battery model we get)
protection board
XT-Plugs

  

 - **Microcontroller:**
Wemos D1 R32 ESP board : [datasheet](https://handsontec.com/dataspecs/module/ESP/WeMos%20D1%20R32.pdf)
-In stock: 4 CHF
 -weight : 20g
    
- **Multiplexer:**
pca9685 Board (16 PWM)
-12 CHF
-16 inputs (2 servos won’t be plugged in)
 -48g

- **Distance sensor (implement the narrow gait) :**
HC-SR04 ultrasonic distance sensor
-price : 2CHF
 -5V supply
 -9g
    

  

**Total:
Price: 137 CHF
Weight: 685 g**