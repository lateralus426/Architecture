# Getting Started

This documentation provides a comprehensive introduction to working with the Architecture engine. Follow these steps to get up and running quickly.

## Prerequisites

*   CMake 3.5 or higher
*   A C++20 compatible compiler (e.g., GCC 11+, Clang 12+, MSVC 19.30+)
*   Basic familiarity with C++20 syntax features

## Setup Instructions

### 1. Clone the Repository
```bash
git clone https://github.com/yourusername/Archive.git
cd Architecture
```

### 2. Configure Build Environment
```bash
# Create a build directory
mkdir build && cd build

# Configure with default OpenGL backend (choose other backends with CMake options)
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_OPENGL=ON
```

### 3. Build the Project
```bash
cmake --build . --config Release
```

### 4. Run the Application
```bash
# From the build directory
./Architecture
```

### 5. Available Backend Options
| Backend | CMake Option | Platform Support |
|---------|--------------|------------------|
| OpenGL | `-DUSE_OPENGL=ON` | Cross-platform |
| Vulkan | `-DUSE_VULKAN=ON` | Cross-platform |
| DirectX 11 | `-DUSE_DX11=ON` | Windows only |
| DirectX 12 | `-DUSE_DX12=ON` | Windows 10+ |

### 6. Immediate Development Workflow

*   Create a new layer by inheriting from `Core::Layer`
*   Implement lifecycle methods: `OnAttach()`, `OnUpdate(float)`, `OnRender()`, `OnEvent(Event&)`
*   Register in your layer's `main.cpp` file using `app.PushLayer<YourLayer>();`

### 7. Basic Engine Usage

The engine follows a straightforward workflow:
1. **Initialize** the `Application` with specifications
2. **Push** layers to the layer stack for rendering
3. **Run** the main loop until shutdown is requested
4. **Stop** and clean up resources

### 8. Adding Documentation

*   Add documentation files to the `/docs` directory following the Diátaxis framework
*   Keep each Markdown file under 120 lines while maintaining complete concepts
*   Use standard headings for consistency
*   Reference the architecture guide for layout recommendations

### 9. Testing Different Backends

To test alternative graphics backends:
```bash
# Set desired backend flag
cmake .. -DUSE_VULKAN=ON -DUSE_OPENGL=OFF
# Or for DirectX
cmake .. -DUSE_DX11=ON
```
Note: Each backend operates in its own mutually exclusive category. Only one renderer backend can be active at a time.

### Troubleshooting

*   Build errors: Ensure all dependencies are properly installed and CMake was able to locate them
*   Runtime errors: Verify that your GPU drivers support the selected backend
*   Missing dependencies: Use `FetchContent` to automatically pull required libraries

### Directory Structure Reference

```
/docs
  /tutorials          - Learning-oriented guides
  /how_to             - Task-oriented procedures
  /reference          - API documentation and specs
  /explanation        - Architectural rationale and design choices
  /includes           - Shared components and snippets
```

This structure ensures that documentation remains organized, discoverable, and follows established best practices for technical writing.