# Nove3Bo

Nove3Bo is an attempt to port RenPy to C++.

## Requirements
### SDL2

## Installation

Glone the repository
```bash
git clone https://github.com/TimurSennikov/Nove3Bo
```

After cloning, cd to repository directory and run
```bash
g++ src/main.cpp src/core.hpp src/config.hpp src/save.hpp src/log.hpp
src/time.hpp src/math.hpp -o Nove3Bo `sdl2-config --libs --cflags` -ggdb3 -O0 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm
```

## Building a game
### Building file tree
to use the program you may want to create "resources" directory and to put file named "font.ttf" with game font to it to it, if not, feel free to redact game code.

Next, create script.txt, we will get to it later.
### NovBoScript game scripts
game scripts are pretty easy to use: you specify text and it appears on screen, if you want some specific action like buttons - call instruction.

##### EVERY line except for specific instructions SHOULD end with ';'!

example:
To change background for "resources/bus_bg_00.png" just type
```
3 resources/bus_bg_00.png;
```
(this assumes you have the specified file).

At the End of script (EOS) parsing will just be stopped.

#### script instructions list

```
0 path X Y;
```
shows image from path at specified offset, computed as window width / X and window height / Y, which means that "0 path 2 2" will show the image in the center of the screen.

```
1 path;
```
deletes previously shown image from path from screen.

```
3 path;
```
shows background image from path.

```
4 param0/param1/param2/param3;
```
creates choice with specified params, that are splitted by '/'.

```
5 condition{
Here we write some code.;
};
```
if statement, extracts variables from save file and finds whether the variable exists, if so - code in block is executed, else it is just skipped, it is the instruction in which you dont need to place ';' at the end of condition.

```
6 path;
```
Loads sound from path and starts it, not tested so be ready to get errors!

```
7;
```
Please dont use it, it is still not ready, if you wonder - I wanted to code it to work like "break" in C++.

## Warnings
Project is in alpha, so be ready to get meory leaks and segmentation faults, "ugly" UI and so on.
