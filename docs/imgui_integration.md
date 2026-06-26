# ImGui Integration

This project now supports building Dear ImGui with several renderer backends:

- OpenGL 2 / 3 / 4
- DirectX 9 / 10 / 11 / 12
- Vulkan

## What changed

### 1. CMake backend options
In the root [CMakeLists.txt](../CMakeLists.txt), the build now exposes options for selecting a renderer backend:

- `USE_OPENGL`
- `USE_DX9`
- `USE_DX10`
- `USE_DX11`
- `USE_DX12`
- `USE_VULKAN`

For OpenGL, you can also select the version with:

- `OPENGL_VERSION=2`
- `OPENGL_VERSION=3`
- `OPENGL_VERSION=4`

### 2. ImGui dependency wiring
In [Dependencies.cmake](../Dependencies.cmake), the ImGui target now includes the correct backend source files based on the chosen renderer:

- OpenGL uses `imgui_impl_glfw.cpp` and `imgui_impl_opengl3.cpp` (or `imgui_impl_opengl2.cpp` for OpenGL 2)
- DirectX uses `imgui_impl_win32.cpp` plus the matching DirectX backend
- Vulkan uses `imgui_impl_glfw.cpp` and `imgui_impl_vulkan.cpp`

The ImGui target also receives:

- the GLFW include directory
- the GLFW target linkage
- the matching platform libraries for DirectX/Vulkan

### 3. Core propagation
The Core target in [Core/CMakeLists.txt](../Core/CMakeLists.txt) now propagates the dependency include paths to downstream targets so headers such as `GLFW/glfw3.h` resolve correctly.

## How to configure a backend

### OpenGL 4
```powershell
cmake -S . -B build -DUSE_OPENGL=ON -DOPENGL_VERSION=4
```

### OpenGL 3
```powershell
cmake -S . -B build -DUSE_OPENGL=ON -DOPENGL_VERSION=3
```

### OpenGL 2
```powershell
cmake -S . -B build -DUSE_OPENGL=ON -DOPENGL_VERSION=2
```

### DirectX 11
```powershell
cmake -S . -B build -DUSE_DX11=ON
```

### DirectX 12
```powershell
cmake -S . -B build -DUSE_DX12=ON
```

### Vulkan
```powershell
cmake -S . -B build -DUSE_VULKAN=ON
```

## Building the ImGui target

```powershell
cmake --build build --config Debug --target imgui
```

## Visual Studio note

If Visual Studio still shows stale IntelliSense or build errors after changing backend options, reload the CMake project or reopen the solution so it regenerates the project files.
