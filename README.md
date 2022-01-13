# DigitalVox4 - Metal Graphics Engine

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

## Cloning
This repository contains submodules for external dependencies, so when doing a fresh clone you need to clone recursively:

```
git clone --recursive https://github.com/yangfengzzz/DigitalVox4.git
```

Existing repositories can be updated manually:

```
git submodule init
git submodule update
```
