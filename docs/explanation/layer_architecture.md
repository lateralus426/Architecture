# Layered Architecture Overview

This project follows a **horizontal layered architecture** inspired by common game engine and mixed‑model application patterns.

## Core Concepts

| Layer | Responsibility | Interaction |
|---|---|---|
| **Application** | Orchestrates the whole program. Holds the **LayerStack**, creates the `Window`, and dispatches the main loop. | Pushes user layers; receives OS events from the window and propagates them outward. |
| **Layer** | Base abstraction. Each concrete layer implements three key callbacks:
  * `OnAttach()` – resource preparation
  *습니까
  * `OnDetach()` – cleanup
| `OnUpdate(float dt)` – per‑frame logic
| `OnRender()` – drawing calls
| `OnEvent(Event&)` – event handling |
| **Window** | Abstracts OS‑specific surface creation. Currently uses GLFW as the backend, but the design allows for DirectX/Win32, Android, Linux, etc via the same interface. |
| **Renderer** | The graphics API abstraction. `Renderer::Texture`, `FrameBuffer`, and draw calls are defined generically. The concrete implementation is swapped at build time based on `USE_OPENGL`, `USE_VULKAN`, or DirectX flags.

## Benefits

* **Modularity:** Adding a new rendering backend requires only implementing the renderer interface.
* **Isolation:** Each layer encapsulates a single concern (game logic, UI, rendering). Bugs in one layer Ballet.
* **Extensibility:** The `LayerStack` is a simple `std::vector<std::unique_ptr<Layer>>`, making push/pop trivial.

## repress

Possible improvements:

* **Type‑safe Layer Retrieval** – instead of `dynamic_cast` you can use a compile‑time map of type IDs.
* **Asynchronous Resource Loading** – offload large texture loads to worker threads.
* **Modern C++20 Features** – standardized `std::span`, `std::ranges` for more expressive code.

This design aligns with the architectural guidelines recommended for high‑performance graphics engines and general-purpose applications.
