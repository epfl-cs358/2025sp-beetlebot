<h1 align="center">CS-358 Project: BeetleBot 🪲</h1>

---

**Contributors:** Anna Barberis, Omayma Hadiji, Adèle Monjauze, Jonas Zimmermann, Anas El Madhi

![BeetleBot Animation](documentation/full_bot_animation.gif)

## Project Description

BeetleBot is a six-legged robot with 3 degrees of freedom on each leg. The idea is to mimic the walking pattern of an insect, or more specifically to implement a tripod-like motion. Walking on flat ground, a hexapod can implement many different gaits to move itself. The most popular ones include tripod gait, going forward, backward, and rotating, an crab walk-like motion. 
Thus, a hexapod demonstrates flexibility in how it can move and balance its weight. Our robot moves itself based on an input given by a user through a web application. 
It interacts with its surrounding with three sensors posted on the front and the side. Read more on the [project proposal]().

## Table of Contents

(for easier navigation through the document, will most likely be completed last)

## Hardware & Design Specifications

[View 3D Model](documentation/full_bot.stl)

@Adele
Details about the size, weight and other components in the beetlebot. Comments about why certain choices were made (in regards to design, placement of components etc)

The goal of our 3D design was to keep it lightweight, as compact as possible, and avoid any unnecessary plastic. This led to minimalistic legs with sturdy joints, using just enough material to hold the servos tightly, all while eliminating unwanted movement and making sure the servos weren’t supporting their own weight. The base was designed to support the heaviest items on the bottom for better stability, while the lid holds the materials that require the most convenient access. This resulted in having 6 servos and the battery, along with its protection board and the buck converter, on the bottom as they hold the most weight. The ESP32 and the servo multiplexer were placed on top as we often needed to change the ESP's or the multiplexer's cabling. The 6 servos that served as coxas were placed downwards so they could be directly screwed on the floor, which wouldn't require any unecessary additional structure.
-legs design: short, light and minimalistic
- base design: made with lots of holes to make it lighter, made so we can screw the base servos directly 


Add potentially BoM and schematics and wiring

## Software Specifications

@Omayma and Anna
(everything ab the software here ig)
@Jonas
everything about the web serial and sensor handling

## Challenges

- Hardware:
    - The challenges for the CAD was mainly the leg: We initially had a first design where the leg was unecessary long and heavy, with an added effect of the servos having to handle their own weight along with the rest of the leg. We did not realise immediately the importance of the design, thinking the servos would be able to hold it in any case. In fact, even with our now optimised design, they still have some struggly carrying the whole weight. The leg being too long was also problematic because this  It took some time but we finally managed to create a minimalistric design where the servos were tightly screwed to their joint. In consequence, the legs were much lighter and the servos did not have to hold their own weight anymore.
    - Another hardware challenge was the cable management. Indeed, we were facing a trio of cables for each servos, along with cables for the multiplexer, the battery, the sensors and the ESP32, totalling with a strong ~50 cables. We managed to make some of them fit inside the base, but the biggest problems was the cables that had to stay outside of the base since the multiplexer was on the lid. We ended up tying the servo cables together so they would not be everywhere around our hexapod.
      
- Software:
    - IK

(the main challenges we had to keep in mind while making the bot)

## Installation Instructions

Detail the steps required to set up the project. This should include:

- Hardware requirements (e.g., components needed).
    - Electronics:
        - 18 servos (in our case it was servo SG90 but we would heavily recommand stronger ones)
        - 3 sensors to place in front and on the sides
        - PCA multiplexer to plug the servosA
        - ESP32 as a micro controller
        - LiPo Battery (1000 mAh is enough)
        - Protection Board for the battery
        - Buck converter to convert the battery's voltage to 5V (the ESP32 and the servos can't handle more)
        - And a bunch of cables and Id suggest tape for the cable management (very important)
    - CAD components:
        - a base to hold the battery, its protection board and 6 servos that will serve as the Coxas of the legs.
        - a lid to screw on top of the base, where we can place the multiplexer, the ESP32 and the buck converter.
        - 3 sensor holders to place in between the base and the lid
        - 6 legs composed of 3 pieces each:
            - a first joint to link the Coxa and the Femur
            - a second joint to link the Femur and the Tibia
            - a toe with a TCP tip
- Software requirements (e.g., libraries, IDE).
- Step-by-step installation guide.

## Usage Instructions

Explain how to use your project. Include:

- Code snippets for basic operations.
- Instructions on how to run the project.
- Any configuration settings that need to be adjusted.

## Troubleshooting

Provide solutions to common issues that users might encounter. This can help users resolve problems without needing to contact you.

## License

(à voir si on ajoute ou pas)

## Acknowledgments

Mention any resources, libraries, or individuals that helped you during the project
