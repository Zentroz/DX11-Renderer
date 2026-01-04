# DX11-Renderer
A small real-time renderer built using DirectX 11 to explore and implement modern rendering techniques such as deferred rendering, lighting models, and GPU resource management.


## Features
### Core
* DirectX 11 rendering pipeline setup
* Camera movement (WASD + mouse)
* Mesh loading (OBJ, FBX via Assimp)
* Vertex & pixel shaders (HLSL)
* Constant buffer–based CPU ↔ GPU communication

### Rendering Techniques
* Deferred rendering pipeline
* Normal mapping
* Shadow mapping
* Transparent mesh rendering
* Multiple light types (directional, point, spot)

### Lighting Models
* Phong / Blinn-Phong lighting
* Basic Physically Based Rendering (metallic–roughness workflow)

### Tools & Architecture
* Simple render graph to organize render passes
* Abstract renderer layer designed to allow other graphics APIs
* Interactive editor for inspecting scenes and modifying material properties

## Technical Overview
The renderer initializes a DirectX 11 device, swap chain, and render targets.
A deferred rendering approach is used, where geometry data is written to G-buffers and lighting is computed in a separate pass.

Per-frame and per-object data are uploaded using constant buffers.
Materials support both traditional lighting and a simplified PBR workflow using metallic and roughness parameters.

Render passes are organized through a lightweight render graph to control execution order and resource dependencies.

## Concepts Learned
* Designing a clean and scalable rendering architecture
* CPU–GPU data flow using constant buffers
* Transparent object rendering and draw order considerations
* Tangent–Bitangent–Normal (TBN) matrix usage for normal mapping
* Deferred rendering and multi-pass rendering workflows
* Core PBR principles such as energy conservation and material response

## Borrowed Code
WinAPI header optimization adapted from the [Chili Framework](https://wiki.planetchili.net/index.php/Chili_Framework)

## Libraries / Dependencies Used
* [Assimp](https://github.com/assimp/assimp) Mesh Loader

## Build Instructions
### Requirements
* Windows 10+
* Visual Studio 2022
* Windows 10 SDK
* Premake5

````bash
git clone <repository-url>
cd DX11-Renderer
premake5 vs2022
````
