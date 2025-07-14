# SpriteAnimationMinigameEnhanced

A simple mini-game built with SDL2 featuring sprite animations, basic physics movement, and a collision system with multiple entities.

---

## Description

This project implements a basic sprite animation system along with a player-controlled entity that can move with acceleration, friction, and maximum speed. It detects and resolves collisions with multiple obstacles on screen. Developed in C++ using SDL2 and SDL_image.

---

## Features

- Sprite sheet based animations.
- Movement with acceleration, friction, and max speed.
- Axis-Aligned Bounding Box (AABB) collision detection for multiple entities.
- Basic collision resolution to prevent the player from passing through obstacles.
- Modular code design with classes for animation, entities, and collisions.

---

## Requirements

- SDL2
- SDL2_image
- C++ compiler supporting C++11 or higher

---

## How to Build

```bash
g++ -o game main.cpp -lSDL2 -lSDL2_image
