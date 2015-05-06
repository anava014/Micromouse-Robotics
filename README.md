# Micromouse-Robotics

Objective: In this contest the contestant or team of contestants design and build an autonomous robotic "mouse" that negotiates a maze of standard dimensions from a specified corner to its center in the shortest time.

This micromouse began as a sumo bot. It was a gift from a friend and so we decided to base our mouse off of it.
We first used an Arduino UNO as the brain of the mouse but we wanted to be more compact so we switched to the teensy 3.1. We noticed faster computation with its sensors and its funtionality which was great.
We were using 4.5 Volts to power our motors and our teensy but we wanted longer lasting power and faster wheels. Since the teensy can support 5 Volts and the motors can support 6 Volts Max, we decided to use a 7.4 Volts Lipo battery.

After months of work, we realized that the original Servo Motors on it would give us lots of trouble when attempting to keep track of which cell its in, thus, giving us a difficult time implementing the Flood-Fill Algorithm.

As of now, we plan to abandon the thought of implementing the flood fill algorithm for the sake of time.
We are instead using a "smart"- random algorithm.

The software runs well and traverses through the maze with minimal error. 
We will keep using this software until the end of the season in 2015.

January, 2015
![alt tag](http://i.imgur.com/93ZiiX9.jpg)

March, 2015
![alt tag](http://i.imgur.com/jITsJnL.jpg)

May, 2015
![alt tag](http://i.imgur.com/uzlj2Ws.jpg)
