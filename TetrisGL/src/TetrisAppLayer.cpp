#include "TetrisAppLayer.h"

#include "Core/Application.h"

#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/Shader.h"

#include <glm/glm.hpp>

#include <print>


#include "Core/Layer.h"
#include "Core/InputEvents.h"
#include "Core/WindowEvents.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

TetrisAppLayer::TetrisAppLayer()
{
	std::println("Created new AppLayer!");

	// Create shaders
	m_Shader = Renderer::CreateGraphicsShader("Shaders/vertex.glsl", "Shaders/fragment.glsl");

	// Create geometry
	glCreateVertexArrays(1, &m_VertexArray);  // vertex array object (VAO)
	glCreateBuffers(1, &m_VertexBuffer);

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
	};

	//Vertex vertices[] = {
	//	{ {-1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },  // Bottom-left
	//	{ { 3.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },  // Bottom-right
	//	{ {-1.0f,  3.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }   // Top-left
	//};

	float vertices[] = {
		// Front face
		0.5,  0.5,  0.5,
		-0.5,  0.5,  0.5,
		-0.5, -0.5,  0.5,
		0.5, -0.5,  0.5,

		// Back face
		0.5,  0.5, -0.5,
		-0.5,  0.5, -0.5,
		-0.5, -0.5, -0.5,
		0.5, -0.5, -0.5,
	};

	float vertex_colors[] = {
		1.0, 0.4, 0.6,
		1.0, 0.9, 0.2,
		0.7, 0.3, 0.8,
		0.5, 0.3, 1.0,

		0.2, 0.6, 1.0,
		0.6, 1.0, 0.4,
		0.6, 0.8, 0.8,
		0.4, 0.8, 0.8,
	};

	  unsigned short triangle_indices[] = {
		  // Front
		  0, 1, 2,
		  2, 3, 0,

		  // Right
		  0, 3, 7,
		  7, 4, 0,

		  // Bottom
		  2, 6, 7,
		  7, 3, 2,

		  // Left
		  1, 5, 6,
		  6, 2, 1,

		  // Back
		  4, 7, 6,
		  6, 5, 4,

		  // Top
		  5, 1, 0,
		  0, 4, 5,
	  };



	  // VAO � stores the attribute layout and buffer bindings
	  glCreateVertexArrays(1, &m_VertexArray);        //  this IS the VAO

	  // VBOs � the raw GPU buffers with your data
	  GLuint posBuffer, colorBuffer, indexBuffer;
	  glCreateBuffers(1, &posBuffer);                 //  VBO #1: positions
	  glCreateBuffers(1, &colorBuffer);               //  VBO #2: colors
	  glCreateBuffers(1, &indexBuffer);               //  EBO: triangle indices

	  // Upload data into the VBOs (no binding needed with DSA)
	  glNamedBufferData(posBuffer, sizeof(vertices), vertices, GL_STATIC_DRAW);
	  glNamedBufferData(colorBuffer, sizeof(vertex_colors), vertex_colors, GL_STATIC_DRAW);
	  glNamedBufferData(indexBuffer, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);

	  // Wire the VBOs into the VAO
	  glVertexArrayVertexBuffer(m_VertexArray, 0, posBuffer, 0, 3 * sizeof(float));    //  VAO learns about VBO #1
	  glVertexArrayVertexBuffer(m_VertexArray, 1, colorBuffer, 0, 3 * sizeof(float));  //  VAO learns about VBO #2
	  glVertexArrayElementBuffer(m_VertexArray, indexBuffer);                          //  VAO learns about EBO

	  //glVertexArrayAttribBinding(m_VertexArray, 0, 0);
	 // glVertexArrayAttribBinding(m_VertexArray, 1, 1);

	  // position
	  glEnableVertexArrayAttrib(m_VertexArray, 0);
	  glVertexArrayAttribFormat(
		  m_VertexArray,
		  0,
		  3,
		  GL_FLOAT,
		  GL_FALSE,
		  0);
	  glVertexArrayAttribBinding(m_VertexArray, 0, 0);

	  // color
	  glEnableVertexArrayAttrib(m_VertexArray, 1);
	  glVertexArrayAttribFormat(
		  m_VertexArray,
		  1,
		  3,
		  GL_FLOAT,
		  GL_FALSE,
		  0);
	  glVertexArrayAttribBinding(m_VertexArray, 1, 1);


}

TetrisAppLayer::~TetrisAppLayer()
{
	glDeleteVertexArrays(1, &m_VertexArray);
	glDeleteBuffers(1, &m_VertexBuffer);

	glDeleteProgram(m_Shader);
}

void TetrisAppLayer::OnEvent(Core::Event& event)
{
	std::println("{}", event.ToString());

	Core::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<Core::MouseButtonPressedEvent>([this](Core::MouseButtonPressedEvent& e) { return OnMouseButtonPressed(e); });
	dispatcher.Dispatch<Core::MouseMovedEvent>([this](Core::MouseMovedEvent& e) { return OnMouseMoved(e); });
	dispatcher.Dispatch<Core::WindowClosedEvent>([this](Core::WindowClosedEvent& e) { return OnWindowClosed(e); });
}

void TetrisAppLayer::OnUpdate(float ts)
{
	m_Time += ts;

	if (glfwGetKey(Core::Application::Get().GetWindow()->GetHandle(), GLFW_KEY_1) == GLFW_PRESS)
	{
		std::println("Chanign Layer not Impement!");
		//TransitionTo<VoidLayer>();
	}
}
/*
void TetrisAppLayer::OnRender()
{
	glUseProgram(m_Shader);

	// Uniforms
	//glUniform1f(0, m_Time);

	glm::vec2 framebufferSize = Core::Application::Get().GetFramebufferSize();
	//glUniform2f(1, framebufferSize.x, framebufferSize.y);

	//glUniform2f(2, m_FlamePosition.x, m_FlamePosition.y);

	//glViewport(0, 0, static_cast<GLsizei>(framebufferSize.x), static_cast<GLsizei>(framebufferSize.y));

	// Render
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindVertexArray(m_VertexArray);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	float aspect = framebufferSize.x / framebufferSize.y;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.5f));
	//glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f);
	glm::mat4 mvp = projection * view * model;


	//glm::mat4 model = glm::mat4(1.0f);
	//glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));

	//float aspect = framebufferSize.x / framebufferSize.y;
	//glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

	//glm::mat4 mvp = projection * view * model;

	//view = glm::mat4(1.0f);

	glUseProgram(m_Shader);
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, static_cast<GLsizei>(framebufferSize.x), static_cast<GLsizei>(framebufferSize.y));
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(m_VertexArray);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);


}*/


void TetrisAppLayer::OnRender()
{
    glm::vec2 framebufferSize = Core::Application::Get().GetFramebufferSize();
    float aspect = framebufferSize.x / framebufferSize.y;

	m_Angle += m_Time*0.01f;



	m_Angle >= 360.0f ? m_Angle = 0.0f : m_Angle;
	//glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::degrees(m_Angle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    glm::mat4 mvp = projection * view * model;

    glUseProgram(m_Shader);
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, (GLsizei)framebufferSize.x, (GLsizei)framebufferSize.y);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(m_VertexArray);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
}

bool TetrisAppLayer::OnMouseButtonPressed(Core::MouseButtonPressedEvent& event)
{
	glm::vec2 framebufferSize = Core::Application::Get().GetFramebufferSize();
	float aspectRatio = framebufferSize.x / framebufferSize.y;
	glm::vec2 normalizedMousePos = (m_MousePosition / framebufferSize) * 2.0f - 1.0f;
	normalizedMousePos.x *= aspectRatio;
	normalizedMousePos.y *= -1.0f;
	normalizedMousePos.y += 0.7f;

	m_FlamePosition = -normalizedMousePos;

	return false;
}

bool TetrisAppLayer::OnMouseMoved(Core::MouseMovedEvent& event)
{
	m_MousePosition = { static_cast<float>(event.GetX()), static_cast<float>(event.GetY()) };

	return false;
}

bool TetrisAppLayer::OnWindowClosed(Core::WindowClosedEvent& event)
{
	std::println("Window Closed!");

	return false;
}
