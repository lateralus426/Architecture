# Render Backend Reference

This engine supports multiple rendering backends selected via CMake options:
* `USE_OPENGL` (default)
* `USE_VULKAN`
* `USE_DX9`, `USE_DX10`, `USE_DX11`, `USE_DX12`

## Design Goals

1.  **Backend Agnostic Core** – The `Application`, `LayerStack`, and `Window` layers contain no API‑specific code.
2.  **Renderer Facade** – All engine code talks to the `Renderer` namespace. The concrete implementation is pulled in via CMake‐driven conditional compilation.
3.  **Opaque Handles** – Resources such as textures, framebuffers, and shaders are described by simple POD structs whose members change based on the active backend.
4.  **Feature Parity** – Core capabilities (texture creation, framebuffer blit, shader binding) must be available on every enabled backend.

## Current State (OpenGL Backend)

When `USE_OPENGL` is ON, the engine pulls in GLAD and GLFW. The following files are active:

* `Core/Source/Core/Renderer/Renderer.h/.cpp`
    * Defines `Texture` (`GLuint Handle`), `Framebuffer`, and helper functions (`CreateTexture`, `LoadTexture`, ...).
* `Core/Source/Core/Window.h/.cpp`
    * Uses `GLFWwindow*` as the native handle.
* `Core/Source/Core/GLUtils.h/.cpp` (if present)
    * Holds helper wrappers (e.g., for compiling shaders).

## Adding a New Backend (e.g., Vulkan)

To support a new API:

1.  **Define CMake Option** – Add `option(USE_NEWAPI "Enable NewAPI backend" OFF)` to the top‐level `CMakeLists.txt`.
2.  **Guard API Includes** – In `Renderer.h` wrap NewAPI headers in `#if USE_NEWAPI … #endif`.
3.  **Provide Implementation** – Create `Core/Source/Core/Renderer/Renderer_NewAPI.h/.cpp` that satisfies the same interface:
    * Replace `GLuint` handles with `VkImage`, `ID3D12Resource`, etc.
    * Implement equivalent functions: `CreateTexture`, `LoadTexture`, `CreateFramebufferWithTexture`, `BlitFramebufferToSwapchain`.
4.  **Link Libraries** – Add the backend’s SDK/libraries in `Dependencies.cmake`.
5.  **Update ImGui** – Add the corresponding imgui backend files (`imgui_impl_newapi.cpp`) in the imgui section of `CMakeLists.txt`.

## Example: Minimal Vulkan Texture Struct

```cpp
#if USE_VULKAN
    struct Texture
    {
        VkImage        image = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkImageView    view  = VK_NULL_HANDLE;
        VkSampler      sampler = VK_NULL_HANDLE;
        uint32_t       width = 0;
        uint32_t       height = 0;
    };
#endif
```

> **Note:** Keeping the struct layout identical across backends is optional; engine code only ever accesses the struct via accessor functions provided by the renderer layer (future improvement).

## Build Matrix

When multiple backends are enabled simultaneously, the linker will contain duplicate symbol definitions.  
**Best Practice:** Enable **only one** graphics backend per build configuration to avoid ODR violations and excessive binary size.

## Testing Across Backends

1.  Select a backend: e.g., `-DUSE_VULKAN=ON -DUSE_OPENGL=OFF`
2.  Regenerate: `cmake .. -S . -B build`
3.  Build: `cmake --build build --config Release`
4.  Run the resulting executable from `build/`.

For CI, matrix‑build each backend option on its supported platform(s) (DirectX → Windows only; Vulkan/Linux/macOS/Windows; OpenGL → cross‑platform).