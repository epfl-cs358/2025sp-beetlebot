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

@Adele
Details about the size, weight and other components in the beetlebot. Comments about why certain choices were made (in regards to design, placement of components etc)

Add potentially BoM and schematics and wiring

## Software Specifications

@Omayma and Anna
(everything ab the software here ig)
@Jonas
everything about the web serial and sensor handling

## Challenges

- Hardware:
    - For 3D design, the challenge was to make something not too big, that would hold well, that would make enough sense and be light enough so that all the servos could hold its weight and move in harmony.
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
