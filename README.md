# gametest3d
A seed project for 3D Game Development students.

## Overview
This reposity will be used as the base code project for the 3D Game Development course at NJIT.

This project will contain the absolute basics for setting up OpenGL 4 with windows and linux (and possibly other platforms in the future).

In class examples will be pushed as date-named branches

## Dependencies
These are the libraries that will be needed in order to build this project:
 * SDL2 - Compiled against the 32 bit SDL2 Core as well as SDL_Image.  This library provides most of the non-3d game engine needs like input handling and 2d graphics loading.
 * libGL - The OpenGl Library.
 * GLEW - A companion library that sets up your code to use the correct version of OpenGL
 * GLFW - Another helper library that helps set up GL in a window context

## Roadmap
The plan for the project is to provide these basic source files:
 * gametest3d.c the home of int main();
 * graphics3d.c/.h provides initialization of the 3d graphics system, configuration of the same
 * obj.c/.h provides code to load and parse and store 3d model data of the .obj format
 * shaders.c/.h - provides a basic interface to the gl shader system

