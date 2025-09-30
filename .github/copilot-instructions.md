<!-- Copilot / AI agent instructions for the OpenGL particle / soft-body demo -->

This repo is a small OpenGL-based soft-body/particle simulation. Below are concise, actionable details to help an AI agent be immediately productive.

High-level architecture
- Entry: `src/main.cpp` — creates GLFW window, initializes GLAD, sets up shader and a list of `SoftCube` objects and runs a fixed-step physics + render loop.
- Physics objects: `includes/Test/SoftCube.h` / `includes/Test/SoftCube.cpp` — 8-particle cube, particles + springs, forces, semi-implicit Euler integration, collision with ground.
- Rendering: `includes/Test/shader.h` and shader sources in `src/*.vert` / `src/*.frag` — simple uniform-based shader (objectColor, view/projection/model matrices).
- Build system: CMake with vcpkg toolchain. Project target is `OPENGL_APP` (RUNTIME in `build/../src`). See `CMakeLists.txt` and `CMakePresets.json`.

Important code patterns & conventions
- Fixed time-step for physics: `main.cpp` uses a fixed dt (1/60) and calls `SoftCube::update(dt)` — prefer keeping physics updates deterministic by preserving this pattern.
- Small, self-contained objects: `SoftCube` owns particles/springs and its own VAO/VBO/EBO. Rendering is driven by `updateRenderData()` that calls `glBufferSubData`; change particle layout carefully and update buffer sizes in `SoftCube` ctor.
- Absolute shader paths: `main.cpp` and `Test::Shader` load shader files using absolute paths (e.g. "D:/CODE/ComGraphic/project-rework/src/shader.vert"). When editing relative paths, keep repository-root-relative paths or update `CMake` / run paths accordingly.
- Minimal shader utility: `includes/Test/shader.h` exposes `use()`, `setMat4`, `setVec3`. Prefer these helpers when modifying shader uniforms.

Build / run / debug
- Typical local configure+build (Windows, cmd.exe):
  - Configure with vcpkg toolchain (preset exists): use CMake `vcpkg` preset which sets ninja + toolchain. In VSCode/CMake tools select the `vcpkg` configure preset.
  - From cmd.exe (manual):
    cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-static
    cmake --build build --config Release
  - Executable: after build `src/OPENGL_APP.exe` (CMake sets runtime dir to `build/../src`). The workspace also contains a prebuilt `main.exe` and `OPENGL_APP.exe` in `src/`.
- Dependencies: installed through `vcpkg`. Relevant packages (explicit or discovered): glm, glad, glfw3, OpenMP, Freetype, imgui, stb. Use `vcpkg` to install if missing.

Quick notes for edits and PRs
- When changing particle/vertex layout: update buffer allocation in `SoftCube` constructor (`glBufferData` size) and `updateRenderData()` subdata call. Index buffer lives in EBO and uses `indices` vector.
- When adding new includes under `includes/`, CMake already glob-recurses `includes/*/*.cpp` — add source files there for automatic inclusion.
- OpenGL debug: enable `GL_DEBUG_OUTPUT` and register callback in `main.cpp` early after context creation if you need shader/GL errors during development.

Integration points and gotchas
- Render data is uploaded with `glBufferSubData` every frame — keep updates small to maintain realtime rates. If adding many particles, consider mapping buffers instead.
- Absolute paths to shader files are used in existing code; tests and CI may run from repository root. Prefer using relative paths (e.g. `src/shader.vert`) and update any code that relies on D: hardcoded paths.
- Build presets: `CMakePresets.json` contains a `vcpkg` configure preset (generator: Ninja, binaryDir: build). Use it to reproduce developer builds reliably.

Files to inspect when working on simulation changes
- `src/main.cpp` — loop, window, shader setup.
- `includes/Test/SoftCube.*` — physics + rendering for soft cubes.
- `includes/Test/shader.h` — shader utility used by `SoftCube`.
- `CMakeLists.txt`, `CMakePresets.json`, `vcpkg.json` — build and dependency entry points.

When unsure, do this quick checklist
1. Build with the `vcpkg` preset (ensures deps resolved).
2. Run the binary from repository `src/` (there are prebuilt exe files for quick checks).
3. If a shader or texture fails to load, check for absolute paths in `main.cpp` and `Shader` loader.

If anything here is unclear or you want additional examples (unit tests, CI steps, or refactors), tell me which area to expand.
