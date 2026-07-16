## Legacy Hardware Fallback Strategy

The engine is designed to support **modern** GPUs while also falling back to **legacy APIs** (DirectX 9 and OpenGL 3.3) to reach older hardware.  The fallback path leverages a *simpler* rendering pipeline that omits advanced features such as dynamic lighting, post‑processing, or shader‑based instancing.  The goal is to maintain a usable, albeit less feature‑rich, rendering experience on legacy systems.

### 1. DirectX 9 (DX9) Support
- **Core feature set**: Fixed‑function pipeline, simple vertex shaders (HLSL 2.0+), and limited tessellation support.
- **Legacy source**: Lightweight full‑source engine for Windows built with C/C++ and Direct3D9 – **InteractiveX**: https://github.com/InteractiveX/InteractiveX (example repo).
- **Reference article**: *DirectX versions and legacy hardware support* – https://developer.valvesoftware.com/wiki/DirectX_Versions.
- **Legacy graphics API fallback**: Ashes is a drop‑in replacement or ICD to Vulkan.  It allows writing Vulkan code while selecting the rendering API.  Ashespp provides a C++ wrapper for Vulkan.  https://github.com/ssloy/tinyrenderer?tab=readme-ov-file  

### 2. OpenGL 3.3 Core Profile (Fallback)
- **Minimum required version**: OpenGL 3.3 (core profile) – needed for modern shaders and VAOs.
- **Legacy support**: OpenGL 3.3 on Windows XP requires vendor‑specific drivers (no built‑in OS support).
- **Reference implementation**: “Big Rendering Refactor – Fully Modern OpenGL Support 3.3 Core Profile” – https://castle-engine.io/wp/2023/04/30/big-rendering-refactor-fully-modern-opengl-support-3-3-core-profile-mobile-opengles-more-functional-3d-textures-occlusion-query-more-opengles-3-features-ancient-opengl-better-reliable-fallback
- **Practical example**: https://github.com/Urho3D/Urho3D/issues/270 (discusses legacy‑compatible rendering).

### 3. Linux Graphics API Support
- **OpenGL 3.3+**: Supported via Mesa (since v10.3) with vendor drivers; also available via Zink (OpenGL‑to‑Vulkan translation) for systems without native OpenGL.
- **Vulkan**: Full native support on modern Linux systems.
- **Direct3D 9 fallback**: Supported via **Gallium Nine** (open‑source D3D9 implementation) and **DXVK** (Vulkan translation layer) – https://www.cosmiclearn.com/vulkan/ecs-integration.php.
- **Linux advantage**: Multiple driver options (NVIDIA, AMD, Intel, Mesa) provide varying levels of OpenGL 3.3 and Vulkan support.

### 4. Legacy Rendering Pipeline
- **Fallback pipeline**: Single‑pass forward rendering (no deferred lighting) with simple material and light uniforms.
- **Shader fallback**: Compile both modern (GLSL 4.6 / HLSL 5.0) and legacy (GLSL 1.30 / HLSL 2.0) shaders; select at runtime based on available hardware.
- **Resource handling**: Use opaque handles (e.g., `GLuint` → `void*`) for backend‑agnostic rendering; avoid direct OpenGL/DirectX types in engine code.

### 4. Build‑time Configuration
- CMake already exposes `USE_DX9` and `USE_OPENGL` flags.  Enable `USE_DX9` for Windows XP compatibility or `USE_OPENGL` for Linux/macOS fallback.
- At runtime, query GPU capabilities and select the most capable API available (Vulkan → OpenGL 4.x → OpenGL 3.3 → DirectX 9).

---  

## Next Steps (Legacy Path)
1. Prototype the simplified forward path on an old PC (Windows XP) to validate shader fallback and performance.
2. Refactor the renderer abstraction (`IRenderer`) to expose a *legacy fallback interface*.
3. Add a configuration panel that allows toggling legacy mode for testing.
4. Benchmark against the modern path to quantify performance overhead.

---  

## Next Steps (General)
1. Conduct a comparative analysis of current implementation vs. these modern techniques...