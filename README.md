# Engge Framework
![CMake](https://github.com/scemino/EnggeFramework/workflows/CMake/badge.svg)
[![CodeFactor](https://www.codefactor.io/repository/github/scemino/enggeframework/badge)](https://www.codefactor.io/repository/github/scemino/enggeframework)

**Engge Framework** is a C++ framework based on SDL.

## Why

Why developping another framework, there are a ton of frameworks: SDL, SFML, Gamedev Framework, etc.

Acually when I was developping engge, I've been facing several issues with [SFML](https://www.sfml-dev.org) which is a fantastic library, really easy to use and well-written.

But here are the unsolved issues:
* change the default coordinates system, engge requires the Y-axis to be up oriented
* SFML does not recognize my Xbox-360 Gamepad and as I understand gamepads are not really well managed by SFML
* cross-platform engine

So why not choose another framework ?

Well:
* for SFML, you already know
* SDL is a too low-level library
* gf is really nice, but it has too much features and dependencies that engge does not need.
And the second reason is it's really easier to learn/understand our own framework

## Dependencies
I try not have too much dependencies, but also it's not necessary to code something already nice-done.

Here are ngf dependencies:
* [SDL2](https://www.libsdl.org): Simple DirectMedia Layer is a cross-platform development library designed to provide low level access to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL and Direct3D.
* [GLEW](http://glew.sourceforge.net): The OpenGL Extension Wrangler Library.
* [glm](https://glm.g-truc.net/0.9.9/index.html): OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
* [dear imgui](https://github.com/ocornut/imgui): Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies.
