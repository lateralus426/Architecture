# Adding a New Layer

This guide explains how to extend the application with custom functionality using the Layer system.

## Step 1: Create the Layer Class

Create a new header/cpp pair in your project's source folder. Inherit from `Core::Layer`:

```cpp
// MyCustomLayer.h
#pragma once
#include "Core/Layer.h"

class MyCustomLayer : public Core::Layer
{
public:
    MyCustomLayer() : Layer("MyCustomLayer") {}

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnEvent(Core::Event& event) override;
};
```

## Step 2: Implement Lifecycle Methods

In the `.cpp` file, provide the implementation:

```cpp
// MyCustomLayer.cpp
#include "MyCustomLayer.h"
#include "Core/Renderer/Renderer.h"

void MyCustomLayer::OnAttach()
{
    // Initialize resources: textures, shaders, buffers
    // Example: m_Texture = Renderer::LoadTexture("assets/texture.png");
}

void MyCustomLayer::OnDetach()
{
    // Clean up resources allocated in OnAttach
}

void MyCustomLayer::OnUpdate(float deltaTime)
{
    // Update logic, physics, input handling
    // Use Core::Input or event-driven patterns
}

void MyCustomLayer::OnRender()
{
    // Issue draw calls using Renderer facade
    // Example: Renderer::DrawTexture(m_Texture, position);
}

void MyCustomLayer::OnEvent(Core::Event& event)
{
    // Handle events dispatched from the Application
    // Use event dispatching for specific event types
}
```

## Step 3: Register the Layer

In your application's `main.cpp` (or entry point):

```cpp
#include "MyCustomLayer.h"

int main()
{
    Core::ApplicationSpecification spec;
    spec.Name = "My Application";
    spec.WindowSpec.Width = 1920;
    spec.WindowSpec.Height = 1080;

    Core::Application app(spec);
    app.PushLayer<MyCustomLayer>();  // Add your layer
    app.Run();
}
```

## Step 4: Build and Test

1. Add the new `.cpp`/`.h` files to your CMake target (`App` or a new subproject).
2. Re-run CMake configuration if new files were added:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build .
   ```
3. Run the application to verify the layer is active.

## Best Practices

*   **Single Responsibility**: Each layer should focus on one concern (rendering, gameplay, UI, physics).
*   **Resource Ownership**: Use `OnAttach`/`OnDetach` for RAII resource management.
*   **Event Handling**: Prefer `OnEvent` for input and system events; use `OnUpdate` for frame-independent logic.
*   **Rendering**: Use the `Renderer` namespace for draw calls to maintain backend independence.
*   **Dependencies**: Layers can communicate via the `Application::GetLayer<Type>()` method, but avoid tight coupling.

## Example Use Cases

| Layer Type | Typical Contents |
|------------|------------------|
| GameplayLayer | Game logic, entity management, collision |
| RenderLayer | Scene rendering, post-processing, camera |
| UILayer | ImGui windows, HUD, menus |
| PhysicsLayer | Simulation stepping, rigid body updates |
| AudioLayer | Sound playback, spatial audio |

## Common Pitfalls

*   Forgetting `override` on virtual methods
*   Performing heavy initialization in the constructor instead of `OnAttach`
*   Not cleaning up resources in `OnDetach`, causing leaks
*   Blocking the main thread in `OnUpdate` (use async patterns if needed)

## Advanced: Layer Ordering

Layers are processed in the order they are pushed:
1.  `OnEvent` - from top to bottom (last pushed gets event first)
2.  `OnUpdate` - from bottom to top (first pushed updates first)
3.  `OnRender` - from bottom to top (first pushed renders first, enabling layering)

Use `Application::PushLayer` and `PopLayer` dynamically for runtime layer management.