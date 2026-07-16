#pragma once


#include <cstdint>
#include <filesystem>



#if OPENGL_VERSION_MAJOR == 1 // need to remove this check and use GLAD for all versions
// In OpenGL 1.x, vogliamo che GLFW includa i vecchi header standard
#include <Windows.h>
#include <GLFW/glfw3.h> // Questo includerà internamente <GL/gl.h> in modo sicuro
#include <GL/glu.h>
#else
// In OpenGL Moderno, usiamo GLAD e diciamo a GLFW di NON includere nulla
//#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#endif


namespace Renderer {

	struct Texture
	{
		GLuint Handle = 0;
		uint32_t Width = 0;
		uint32_t Height = 0;
	};

	struct Framebuffer
	{
		GLuint Handle = 0;
		Texture ColorAttachment;
	};

	Texture CreateTexture(int width, int height);
	Texture LoadTexture(const std::filesystem::path& path);
	Framebuffer CreateFramebufferWithTexture(const Texture texture);
	bool AttachTextureToFramebuffer(Framebuffer& framebuffer, const Texture texture);
	void BlitFramebufferToSwapchain(const Framebuffer framebuffer);
}