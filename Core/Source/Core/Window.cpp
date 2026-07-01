#include "Window.h"

#include "WindowEvents.h"
#include "InputEvents.h"

#include <glad/gl.h>

#include <iostream>
#include <assert.h>

namespace Core {

	Window::Window(const WindowSpecification& specification)
		: m_Specification(specification)
	{
	}

	Window::~Window()
	{
		Destroy();
	}




	/*void Window::Create()
	{
		std::cout << "Requesting OpenGL " << OPENGL_VERSION_MAJOR << "." << OPENGL_VERSION_MINOR << " Core Profile\n";



		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

		m_Handle = glfwCreateWindow(m_Specification.Width, m_Specification.Height,
			m_Specification.Title.c_str(), nullptr, nullptr);

		if (!m_Handle)
		{
			std::cerr << "Failed to create GLFW window!\n";
			assert(false);
		}

		glfwMakeContextCurrent(m_Handle);
		gladLoadGL(glfwGetProcAddress);

		glfwSwapInterval(m_Specification.VSync ? 1 : 0);

		glfwSetWindowUserPointer(m_Handle, this);

		glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* handle)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			WindowClosedEvent event;
			window.RaiseEvent(event);
		});

		glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow* handle, int width, int height)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			WindowResizeEvent event((uint32_t)width, (uint32_t)height);
			window.RaiseEvent(event);
		});

		glfwSetKeyCallback(m_Handle, [](GLFWwindow* handle, int key, int scancode, int action, int mods)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			switch (action)
			{
				case GLFW_PRESS:
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, action == GLFW_REPEAT);
					window.RaiseEvent(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					window.RaiseEvent(event);
					break;
				}
			}
		});

		glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* handle, int button, int action, int mods)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					window.RaiseEvent(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					window.RaiseEvent(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Handle, [](GLFWwindow* handle, double xOffset, double yOffset)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			MouseScrolledEvent event(xOffset, yOffset);
			window.RaiseEvent(event);
		});

		glfwSetCursorPosCallback(m_Handle, [](GLFWwindow* handle, double x, double y)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			MouseMovedEvent event(x, y);
			window.RaiseEvent(event);
		});
	}*/
	
void Window::Create()
{
	glfwDefaultWindowHints();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);

#if OPENGL_VERSION_MAJOR > 3 || (OPENGL_VERSION_MAJOR == 3 && OPENGL_VERSION_MINOR >= 2)
	std::cout << "Requesting OpenGL "
		<< OPENGL_VERSION_MAJOR << "." << OPENGL_VERSION_MINOR
		<< " Core Profile\n";

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
#else
	std::cout << "Use legacy OpenGL path\n";
	std::cout << "Requesting OpenGL "
		<< OPENGL_VERSION_MAJOR << "." << OPENGL_VERSION_MINOR
		<< " legacy context\n";
#endif

#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	m_Handle = glfwCreateWindow(
		m_Specification.Width,
		m_Specification.Height,
		m_Specification.Title.c_str(),
		nullptr,
		nullptr
	);

	if (!m_Handle)
	{
		std::cerr << "Failed to create GLFW window!\n";
		assert(false);
	}

	glfwMakeContextCurrent(m_Handle);

	if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD!\n";
		assert(false);
	}

	std::cout << "Created context: "
		<< glGetString(GL_VERSION) << "\n";

	glfwSwapInterval(m_Specification.VSync ? 1 : 0);

	glfwSetWindowUserPointer(m_Handle, this);

	glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* handle)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));
			WindowClosedEvent event;
			window.RaiseEvent(event);
		});

	glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow* handle, int width, int height)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));
			WindowResizeEvent event((uint32_t)width, (uint32_t)height);
			window.RaiseEvent(event);
		});

	glfwSetKeyCallback(m_Handle, [](GLFWwindow* handle, int key, int scancode, int action, int mods)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			switch (action)
			{
			case GLFW_PRESS:
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, action == GLFW_REPEAT);
				window.RaiseEvent(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				window.RaiseEvent(event);
				break;
			}
			}
		});

	glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* handle, int button, int action, int mods)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				window.RaiseEvent(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				window.RaiseEvent(event);
				break;
			}
			}
		});

	glfwSetScrollCallback(m_Handle, [](GLFWwindow* handle, double xOffset, double yOffset)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));
			MouseScrolledEvent event(xOffset, yOffset);
			window.RaiseEvent(event);
		});

	glfwSetCursorPosCallback(m_Handle, [](GLFWwindow* handle, double x, double y)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));
			MouseMovedEvent event(x, y);
			window.RaiseEvent(event);
		});
}
	void Window::Destroy()
	{
		if (m_Handle)
			glfwDestroyWindow(m_Handle);

		m_Handle = nullptr;
	}

	void Window::Update()
	{
		glfwSwapBuffers(m_Handle);
	}

	void Window::RaiseEvent(Event& event)
	{
		if (m_Specification.EventCallback)
			m_Specification.EventCallback(event);
	}

	glm::vec2 Window::GetFramebufferSize() const
	{
		int width, height;
		glfwGetFramebufferSize(m_Handle, &width, &height);
		return { width, height };
	}

	glm::vec2 Window::GetMousePos() const
	{
		double x, y;
		glfwGetCursorPos(m_Handle, &x, &y);
		return { static_cast<float>(x), static_cast<float>(y) };
	}

	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(m_Handle) != 0;
	}

}