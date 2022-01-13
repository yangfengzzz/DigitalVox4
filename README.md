# DigitalVox4 - Metal Graphics Engine

DigitalVox is the name of a series. Although each version is based on a component system, the specific implementation is
very different. Therefore, different code repositories are used for management. Please obtain the code you need on
demand:

1. DigitalVox2: Engine written in Swift, 3rd party library bridged by Objective-C++.
2. DigitalVox3: Engine developed in C++, based on the math library provided by Ozz.
3. **DigitalVox4** : The engine developed by C++ uses [Imath](https://github.com/AcademySoftwareFoundation/Imath)
   provided by Industrial Light and Magic as the common mathematical bottom layer for rendering and simulation. The
   rendering layer abstraction for Metal makes it possible to use the C++ compiler instead of Objective-C++ globally,
   and encapsulates GLFW events to achieve cross-platform. Further, clean up the component system and eliminate the
   global dependency on the Engine class. And make the rendering pipeline independent of the camera, so it is easier to
   customize the rendering pipeline.

## Motivation

This project is inspired by [Oasis](https://github.com/oasis-engine) which is an ECS-liked based engine(not very strict)
. Based on entity and component, it is easy to combine other open-source ability:

1. [ImGui](https://github.com/ocornut/imgui): GUI system
2. [OZZ-Animation](https://github.com/guillaumeblanc/ozz-animation): CPU Animation System
3. [PhysX](https://github.com/NVIDIAGameWorks/PhysX): Physical System
4. [fluid-engine-dev](https://github.com/doyubkim/fluid-engine-dev): Fluid Simulation and CPU Particle System

Which can load a lot of model format including:

1. [FBX](https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2016-1-2): FBX loader with Ozz
   animation
2. [GLTF](https://github.com/syoyo/tinygltf): GLTF Loader with GPU-based Skinning Animation
3. [Other](https://developer.apple.com/documentation/modelio/mdlasset/1391813-canimportfileextension): OBJ and other
   format loaded by ModelIO Framework

## Cloning && Install

This repository contains submodules for external dependencies, so when doing a fresh clone you need to clone
recursively:

```
git clone --recursive https://github.com/yangfengzzz/DigitalVox4.git
```

Existing repositories can be updated manually:

```
git submodule init
git submodule update
```

After clone all third-party dependencies, use the script in third_party folder to build them all:

```
./build.sh
```
