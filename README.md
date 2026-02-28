# Overview
A small real-time renderer built using DirectX 11 to explore and implement modern rendering techniques such as deferred rendering, lighting models, and GPU resource management.

<img width="1918" height="940" alt="renderer-1" src="https://github.com/user-attachments/assets/77cadefa-507e-49d9-9fb6-dbf9bb983299" />

## Features
### In-Progress
* Deforming mesh vertices with tools
### Core
* DirectX 11 rendering pipeline setup
* Camera movement (WASD + mouse)
* Mesh loading (OBJ, FBX via Assimp)
* Vertex & pixel shaders (HLSL)
* Constant buffer–based CPU ↔ GPU communication

### Rendering Techniques
* Deferred rendering pipeline
* Normal mapping
* Alpha Clipping & Transparent mesh rendering
* Multiple light types (directional, point, spot)

### Lighting Models
* Phong / Blinn-Phong lighting
* Basic Physically Based Rendering (metallic–roughness workflow)

### Tools & Architecture
* Simple render graph to organize render passes
* Interactive editor for inspecting scenes and modifying material properties

## G-Buffers
<img width="3836" height="1886" alt="renderer-gbuffers" src="https://github.com/user-attachments/assets/ebc9f7c0-062b-4823-a8b8-a30113a5246b" />

## Aplha Clipping / Blending
<img width="1422" height="950" alt="renderer-aplha" src="https://github.com/user-attachments/assets/7c4bdf2e-7fe6-49d5-ab7b-4604d976ba1c" />

## Technical Overview
The engine initializes DirectX 11 device, and swap chain. Also resource provider and render context which are provided to renderer.
A deferred rendering approach is used, where geometry data is written to G-buffers and lighting is computed in a separate pass.

Per-frame and per-object data are uploaded using constant buffers.
Materials support simplified PBR workflow using metallic and roughness parameters.

Render passes are organized through a lightweight render graph to control execution order and resource dependencies.

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
git clone https://github.com/Zentroz/DX11-Renderer.git
cd DX11-Renderer
premake5 vs2022
````
