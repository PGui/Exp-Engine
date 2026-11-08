# Exp-Engine
A learning and experimentation purpose OpenGL engine.

## Features
- Deferred rendering (GBuffer, directional and point lights, post-processing)
- Directional light shadow maps (hardware PCF + 3x3 kernel, normal-offset bias, texel snapping)
- Entity Component System (EnTT): `Transform`, `MeshRenderer`, `Spin` components and an in-game inspector
- Variable framerate rendering (VSync on/off, optional FPS cap) with a simulation that is either
  fixed-step + interpolated (default, smooth at any refresh rate) or variable-step
- Module system (Engine, Rendering, MaterialLibrary, Input, ECS, Remotery profiling)
- Assimp model loading, ImGui debug UI, shader hot-reload

## Building
```sh
git submodule update --init --recursive
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/Exp-Engine            # runs until the window is closed
./build/Exp-Engine --frames 60 # quits after 60 frames (smoke test)
```
Options: `--vsync on|off`, `--max-fps N` (with vsync off, 0 = uncapped),
`--sim fixed|variable`, `--fixed-rate HZ`. All of them are also editable live in the "Core" panel.
### Visual Studio 2026 (Windows)
```bat
git clone --recursive -b improvements https://github.com/PGui/Exp-Engine.git
cd Exp-Engine
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config Debug
```
Or open `build/Exp-Engine.sln` (or open the folder directly in Visual Studio as a CMake project);
`Exp-Engine` is the startup project, so F5 runs the engine.

On Linux you need the X11/OpenGL development packages
(`libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl-dev`).
Assets are resolved from the source directory (`ROOT_DIR`), so the executable can be launched from anywhere.

In Debug builds an OpenGL debug context is created and driver messages are logged.

## Architecture
```
main.cpp                 registers modules, runs an Application
core/EngineModule        window, main loop, fixed timestep
core/Application         interface implemented by game / sandbox code
sandbox/DemoApplication  the demo scene (entities, lights, camera)
ecs/ECSModule            registry, systems (FixedUpdate, SubmitRenderables), inspector UI
rendering/               RenderingModule (deferred + shadows), ShadowMap, materials, targets
```
Frame order: `OnUpdate` -> simulation -> ECS submit -> render -> frame cap.

Simulation modes:
- **Fixed + interpolation** (default): systems run 0..N times per frame at `updatePeriod`
  (20-240 Hz); rendering blends each entity between its previous and current transform
  with `interpolationAlpha`, so motion is smooth at 30, 60, 144 Hz or uncapped.
- **Variable**: systems run once per frame with the real frame time (clamped to 100 ms).

## Dependencies
Git submodules in `thirdparty/` (the engine is C++20, dependencies are built as C++17):

| Library | Version |
|---|---|
| [Assimp](https://github.com/assimp/assimp) | 6.0.5 |
| [GLFW](https://www.glfw.org/) | 3.5.1 |
| [glm](https://github.com/g-truc/glm) | 1.0.3 |
| [Dear ImGui](https://github.com/ocornut/imgui) | 1.93.0 |
| [EnTT](https://github.com/skypjack/entt) | 4.0.0 |
| [spdlog](https://github.com/gabime/spdlog) | 1.17.0 |
| [stb](https://github.com/nothings/stb) | master (2026-11) |
| [Remotery](https://github.com/Celtoys/Remotery) | main (2024-08) |
| [FiberTaskingLib](https://github.com/RichieSams/FiberTaskingLib) | 2.1.0 (not used yet) |
| glad | vendored, OpenGL 4.6 core |
| [CommandBuffer](https://github.com/tuxalin/CommandBuffer) | design reference |
