# DX11-Renderer
A small real-time renderer built using DirectX 11 to demonstrate core graphics pipeline concepts such as rasterization, shaders, constant buffers, and basic lighting.

## Features
* DirectX 11 rendering pipeline setup
* Mesh loading (OBJ, FBX)
* Camera movement (WASD + mouse)
* Normal & Shadow Mapping
* Transparent mesh rendering
* Deferred Rendering pipeline and Render Graph
* Multiple light support such as point, spot and directional lights
* Basic Phong / Blinn-Phong lighting / Physical Based Rendering (PBR)
* Abstract Renderer architecture that supports the implementation of other Graphics API's
* Interactalbe editor to inspect, change material properties and load scenes from mesh formats. For example FBX, OBJ etc.

## Concepts Learned
* Clean and scalable architecture
* How transparent objects are rendererd.
* How CPU & GPU share information thorugh buffers
* The purpose and use of Tangent - Bitangent - Normal (TBN) Matrix
* Deferred Rendering pipeline and how render passes work and interact in a frame
* Physical Based Rendering concepts such as rendering objects based on thier physical properties like metallic & roughness and energy conservation

## Borrowed
* A WinApi header file to reduces build time by excluding unnecessary declarations from [Chili Framework](https://wiki.planetchili.net/index.php/Chili_Framework)

## Libraries / Dependencies Used
* Assimp Mesh Loader [Assimp](https://github.com/assimp/assimp)

## Build Instructions
1.Clone the repo with command
````bash
git clone <repository-url>
````
2. Setup premake [Premake website](https://premake.github.io/)
3. Run premake build command on project root directory [Premake commands documentation](https://premake.github.io/docs/Using-Premake)
4. Open generated project files and build
