#pragma once

// LegacyRenderer.h
// Handles legacy rendering for Windows XP (DX9, OpenGL 1.1) and low-end Linux systems
// Manages fallback rendering without advanced features

#include <filesystem>
#include <optional>
#include <variant>

// Opaque handle types for cross-backend compatibility
struct OpenGLHandle {
    unsigned int id = 0;
    static constexpr unsigned int invalid = 0;
};

struct DirectX9Handle {
    // Simplified DX9 handle representation - actual implementation would need ID3D9Device9, ID3D9Texture9, etc.
    void* native_handle = nullptr;
    static constexpr void* invalid = nullptr;
};

// Backend-agnostic resource handle
struct TextureHandle {
    std::variant<OpenGLHandle, DirectX9Handle> backend;
    unsigned int width = 0;
    unsigned int height = 0;
};

namespace LegacyRenderer {
    // Legacy rendering mode enumeration
    enum class LegacyMode {
        NONE,      // Modern pipeline (default)
        DX9,       // DirectX 9 legacy mode
        GL13,      // OpenGL 1.3 legacy mode
        GL33       // OpenGL 3.3 legacy mode (if vendor supported)
    };

    // Legacy shader program handles
    struct ShaderProgram {
        unsigned int program_id = 0;
    };

    // Legacy rendering pipeline structure
    struct LegacyRenderState {
        // Legacy OpenGL 1.3: Single texture unit, fixed function matrix
        struct GL13State {
            // OpenGL 1.3-only state
            unsigned int texture_unit = 0;
            bool lighting_enabled = false;
            // Note: No VAOs, no IBOs in GL 1.3
        };

        // DirectX 9 legacy state
        struct DX9State {
            // DX9-only state - simplified representation
            unsigned int material_handle = 0;
            unsigned int texture_stage_state = 0;
            D3DLIGHT9 lights[8]; // Simplified for legacy modes
            bool _padding[3]; // Align for SIMD (optional)
        };

        std::variant<GL13State, DX9State> state;
    };

    // Legacy rendering interface
    namespace Legacy {
        // Initialize legacy rendering system
        bool Initialize(LegacyMode mode);

        // Cleanup legacy resources
        void Shutdown();

        // Check if legacy mode is active
        bool IsActive();

        // Load texture from file with legacy support
        std::optional<TextureHandle> LoadTexture(const std::filesystem::path& path);

        // Create legacy render state
        LegacyRenderState CreateRenderState();

        // Render object with legacy pipeline
        void RenderLegacyObject(
            const TextureHandle& texture,
            const LegacyRenderState& state,
            const float* model_view_matrix, // OpenGL 1.3-style matrix (16 floats)
            float x, float y, float z        // Position for sprite rendering
        );

        // Legacy shader compilation
        bool CompileLegacyShader(
            ShaderProgram& program,
            const std::string& vertex_source,
            const std::string& fragment_source,
            LegacyMode mode
        );

        // Set render state parameters
        void SetRenderState(
            const LegacyRenderState& state,
            LegacyMode mode
        );

        // Clean up legacy resources
        void DestroyTexture(TextureHandle& texture);
        void DestroyShader(ShaderProgram& program);

        // Legacy rendering capabilities detection
        struct LegacyCapabilities {
            bool supports_vbo = false;           // OpenGL VBO (GL_ARB_vertex_buffer_object)
            bool supports_texture_compression = false; // Legacy texture compression (DXT1, etc.)
            bool supports_multitexture = false;  // GL 1.3 multitexturing
            bool supports_separate_shader_objects = false;
            unsigned int max_texture_units = 0;
            unsigned int max_vertex_attributes = 0;
        };

        LegacyCapabilities QueryLegacyCapabilities(LegacyMode mode);

        // Helper for legacy-style rendering (equivalent to fixed function pipeline)
        void LegacyRenderSprite(const TextureHandle& texture, float x, float y, float scale);

        // Legacy material handling (simplified for DX9 fixed function)
        struct LegacyMaterial {
            float diffuse[4];       // RGBA diffuse color
            float specular[4];      // RGBA specular color
            float ambient[4];       // RGBA ambient color
            float emission[4];      // RGBA emission color
            float power;             // Specular power (shininess)
            unsigned int texture_id; // Legacy texture handle
            bool has_alpha = false;
        };

        // Apply legacy material to render state
        void ApplyLegacyMaterial(const LegacyMaterial& material, LegacyMode mode);

        // Legacy rendering helper functions
        namespace Helpers {
            // Legacy sprite rendering with alpha blending
            void LegacySpriteBlending(bool enable, LegacyMode mode);

            // Legacy depth testing
            void LegacyDepthTesting(bool enable, LegacyMode mode);

            // Legacy texture filtering
            void LegacyTextureFiltering(unsigned int min_filter, unsigned int mag_filter, LegacyMode mode);

            // Legacy color material
            void LegacyColorMaterial(unsigned int face, unsigned int property, LegacyMode mode);
        }
    }

    // Legacy renderer error handling
    enum class LegacyError {
        SUCCESS = 0,
        INIT_FAILURE,
        SHADER_COMPILE_FAILED,
        TEXTURE_LOAD_FAILED,
        UNSUPPORTED_MODE,
        INVALID_HANDLE,
        OUT_OF_VIDEO_MEMORY,
        DRIVER_NOT_COMPATIBLE,
        CAPABILITY_NOT_MET
    };

    // Runtime detection of legacy capabilities
    LegacyMode DetectBestLegacyMode();
    LegacyCapabilities DetectLegacyCapabilities(LegacyMode mode);

    // Legacy performance profiling
    namespace Perf {
        struct LegacyPerfMetrics {
            unsigned long long cpu_render_time_ns = 0;
            unsigned long long gpu_upload_time_ns = 0;
            unsigned long long texture_memory_kb = 0;
            unsigned int draw_calls = 0;
            unsigned int vertices_rendered = 0;
        };

        void StartTiming();
        PerfMetrics EndTiming();

        // Legacy-specific profiling
        void ProfileLegacyTextureUpload(const TextureHandle& texture);
        void ProfileLegacyDrawCall();
    }
}