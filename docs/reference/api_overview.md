# API Reference Overview

This document provides a detailed reference for the core engine's public interface.

## Application Class

The `Application` class manages the entire application lifecycle, including window creation and layer stack management.

### Constructors

* `Application(const ApplicationSpecification& specification = ApplicationSpecification())`
    * Creates an application with specified configuration.

### Core Methods

* `void Run()`
    * Starts the main application loop. This blocks until `Stop()` is called or the window is closed.
* `void Stop()`
    * Requests application termination.
* `void RaiseEvent(Event& event)`
    * Propagates an event through the application event queue.
* `template<typename TLayer> void PushLayer()`
    * Creates a new `TLayer` instance and adds it to the layer stack.
* `TLayer* GetLayer()`
    * Retrieves a layer of type `TLayer` from the stack.
* `glm::vec2 GetFramebufferSize() const`
    * Returns the current size of the framebuffer.

## Window Class

The `Window` class abstracts away the underlying OS-specific window creation and management details.

### Constructors

* `Window(const WindowSpecification& specification = WindowSpecification())`
    * Creates a window with specified properties.

### Core Methods

* `void Create()`
    * Creates the actual OS-specific window.
* `void Destroy()`
    * Destroys the created window.
* `void Update()`
    * Updates the window state (e.g., transferring events).
* `void RaiseEvent(Event& event)`
    * Dispatches an event to registered event handlers.
* `glm::vec2 GetFramebufferSize() const`
    * Returns the current size of the framebuffer.
* `glm::vec2 GetMousePos() const`
    * Returns the current mouse position within the window.
* `bool ShouldClose() const`
    * Returns true if the window is requested to close via the close button or similar.

## Layer Class

The `Layer` class is the base abstraction for all application layers.

### Virtual Methods

* `virtual void OnAttach()`
    * Called when the layer is pushed onto the stack. Use for resource initialization.
* `virtual void OnDetach()`
    * Called when the layer is popped from the stack. Use for cleanup.
* `virtual void OnUpdate(float deltaTime)`
    * Called every frame for logic updates.
* `virtual void OnRender()`
    * Called every frame for rendering operations.
* `virtual void OnEvent(Core::Event& event)`
    * Called when an event is propagated through the application.

## Texture Class

Represents a texture resource managed by the `Renderer`.

### Members

* `GLuint Handle` - OpenGL texture handle (semantic meaning varies with backend).
* `uint32_t Width` - Textured width in pixels.
* `uint32_t Height` - Textured height in pixels.

### Methods

* `Texture CreateTexture(int width, int height)`
    * Creates and returns a new empty texture.
* `Texture LoadTexture(const std::filesystem::path& path)`
    * Loads a texture from disk and returns the handle.
* `Framebuffer CreateFramebufferWithTexture(const Texture texture)`
    * Constructs a framebuffer bound to the given texture.
* `bool AttachTextureToFramebuffer(Framebuffer& framebuffer, const Texture texture)`
    * Attaches a texture to an existing framebuffer.
* `void BlitFramebufferToSwapchain(const Framebuffer framebuffer)`
    * Performs a blit operation to the backbuffer.

## Framebuffer Class

Represents a framebuffer resource.

### Members

* `GLuint Handle` - OpenGL framebuffer handle.
* `Texture ColorAttachment` - The color attachment reuse.

### Methods

* **[Unspecified - Consider filling in relevant accessor methods]**

## WindowSpecification Structure

Defines the configuration options for window creation.

### Members

* `std::string Title` - Window title text.
* `uint32_t Width` - Width in pixels.
* `uint32_t Height` - Height in pixels.
* `bool IsResizeable` - Whether the window is resizable.
* `bool VSync` - Whether vertical synchronization is enabled.
* `using EventCallbackFn = std::function<void(Event&)>`
    * Function signature for custom event handler callbacks.
* `EventCallbackFn EventCallback` - Event callback to bind to the window.

## Event Class

Base class for system events. Specific event types are derived from this.

## Shader Program Class

Represents a compiled GPU shader program.

### Members

* `GLuint Handle` - OpenGL shader program handle.

### Methods

* `Shader CreateShader(const std::filesystem::path& vsPath, const std::filesystem::path& fsPath)`
    * Creates a shader program from vertex and fragment shader files.
* `void DeleteShader()`
    * Deletes the compiled shader program.
* `void Use()`
    * Sets the shader program to be active for subsequent draw calls.

## Vertex Array Object Class

Represents a VAO in modern OpenGL.

### Members

* `GLuint Handle` - OpenGL VAO handle.

### Methods

* `struct VertexAttribute`
    * Size: int - Size of vertex attribute. Currently must be between 1 and 4.
    * Type: VA_ARGS::EDataType - Data type enum (e.g., Float, Double etc.) to use for this attribute. Default is Float.
    * Normalized: bool - Whether the value should be normalized during attribute fetch. Default is false.
    * Relative offset: int - Relative offset in bytes from the vertex stride's beginning. Default is 0.
* `VertexArrayObject CreateVertexArrayObject()`
    * Creates a new VAO.
* `void SetVertexAttrib(VertexAttribute vertexAttribute, uint32_t index)`
    * Sets a vertex attribute index to use VAArgs.

This reference provides the essential building blocks for extending and customizing applications within the Architecture engine.