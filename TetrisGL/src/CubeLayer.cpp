#include "CubeLayer.h"

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

#include "Models.h"

CubeLayer::CubeLayer()
{
    std::println("Created new CubeLayer!");

    m_GameState.Init();

    // Create shaders
#ifdef GL3_PLUS_SUPPORT
    std::string vertexShader = "shaders/modern_vs.glsl";
    std::string fragmentShader = "shaders/modern_fs.glsl";
#else
    std::string vertexShader = "shaders/legacy_vs.glsl";
    std::string fragmentShader = "shaders/legacy_fs.glsl";
#endif

#if OPENGL_VERSION_MAJOR > 1    
    m_Shader = Renderer::CreateGraphicsShader(vertexShader, fragmentShader);
    std::println("Finished compiling the shaders\n");

    // Create geometry
    glCreateVertexArrays(1, &m_VertexArray); // vertex array object (VAO)
    glCreateBuffers(1, &m_VertexBuffer);
#endif
    m_Camera.m_Position = glm::vec3(0.0f, 0.0f, -5.0f);
    m_Camera.m_Direction = glm::vec3(0.0f, 0.0f, 1.0f);
    m_Camera.m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_Camera.m_Fov = 45.0f;
    glm::vec2 framebufferSize = Core::Application::Get().GetFramebufferSize();
    float aspect = framebufferSize.x / framebufferSize.y;
    m_Camera.m_AspectRatio = aspect;
    m_Camera.m_NearPlane = 0.1f;
    m_Camera.m_FarPlane = 100.0f;
    m_Camera.m_ScreenSize = glm::ivec2(framebufferSize.x, framebufferSize.y);

    glm::mat4 view = glm::lookAt(m_Camera.m_Position, m_Camera.m_Position + m_Camera.m_Direction, m_Camera.m_Up);
    glm::mat4 projection = glm::perspective(glm::radians(m_Camera.m_Fov), m_Camera.m_AspectRatio, m_Camera.m_NearPlane, m_Camera.m_FarPlane);

    m_Camera.m_View = view;
    m_Camera.m_Projection = projection;

    InitializeCube();
}

CubeLayer::~CubeLayer()
{
#if OPENGL_VERSION_MAJOR > 1
    glDeleteVertexArrays(1, &m_VertexArray);
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_PosBuffer);
    glDeleteBuffers(1, &m_ColorBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);


    glDeleteProgram(m_Shader);
#endif
}



void CubeLayer::OnEvent(Core::Event &event)
{
    std::println("{}", event.ToString());

    Core::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Core::MouseButtonPressedEvent>([this](Core::MouseButtonPressedEvent &e)
                                                       { return OnMouseButtonPressed(e); });
    dispatcher.Dispatch<Core::MouseMovedEvent>([this](Core::MouseMovedEvent &e)
                                               { return OnMouseMoved(e); });
    dispatcher.Dispatch<Core::WindowClosedEvent>([this](Core::WindowClosedEvent &e)
                                                 { return OnWindowClosed(e); });
}

void CubeLayer::OnUpdate(float ts)
{
    m_Time += ts;

    m_Angle += ts * m_Speed;

    m_Angle >= 360.0f ? m_Angle = 0.0f : m_Angle;

    if (glfwGetKey(Core::Application::Get().GetWindow()->GetHandle(), GLFW_KEY_1) == GLFW_PRESS)
    {
        std::println("Chanign Layer not Impement!");
        // TransitionTo<VoidLayer>();
    }

    // Auto-drop (gravity)
    if (!m_GameState.m_GameOver && !m_GameState.m_Paused)
    {
        if ((m_CurrentTime - m_GameState.m_LastDropTime) * 1000.0f > m_GameState.m_DropInterval)
        {
            m_GameState.MoveDown();
            m_GameState.m_LastDropTime = m_CurrentTime;
        }
    }


}

void CubeLayer::OnRender()
{
    RenderCubeVertices();
}

bool CubeLayer::OnMouseButtonPressed(Core::MouseButtonPressedEvent &event)
{
    glm::vec2 framebufferSize = glm::vec2(m_Camera.m_ScreenSize);
    float aspectRatio = framebufferSize.x / framebufferSize.y;
    glm::vec2 normalizedMousePos = (m_MousePosition / framebufferSize) * 2.0f - 1.0f;
    normalizedMousePos.x *= aspectRatio;
    normalizedMousePos.y *= -1.0f;
    normalizedMousePos.y += 0.7f;

    return false;
}

bool CubeLayer::OnMouseMoved(Core::MouseMovedEvent &event)
{
    m_MousePosition = {static_cast<float>(event.GetX()), static_cast<float>(event.GetY())};

    return false;
}

bool CubeLayer::OnWindowClosed(Core::WindowClosedEvent &event)
{
    std::println("Window Closed!");

    return false;
}



void CubeLayer::UpdateCube()
{
  
}


void CubeLayer::InitializeCube()
{
    float vertices[] = {
        // Front face
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        // Back face
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
    };

    float vertex_colors[] = {
        1.0f, 0.4f, 0.6f,
        1.0f, 0.9f, 0.2f,
        0.7f, 0.3f, 0.8f,
        0.5f, 0.3f, 1.0f,
        0.2f, 0.6f, 1.0f,
        0.6f, 1.0f, 0.4f,
        0.6f, 0.8f, 0.8f,
        0.4f, 0.8f, 0.8f,
    };

    unsigned short triangle_indices[] = {
        0, 1, 2,  2, 3, 0,   // Front
        0, 3, 7,  7, 4, 0,   // Right
        2, 6, 7,  7, 3, 2,   // Bottom
        1, 5, 6,  6, 2, 1,   // Left
        4, 7, 6,  6, 5, 4,   // Back
        5, 1, 0,  0, 4, 5,   // Top
    };

#if USE_OPENGL && OPENGL_VERSION_MAJOR == 1
    // GL 1.x — fixed-function, no buffers needed
    glEnable(GL_DEPTH_TEST);
    // Projection is set once here; modelview is rebuilt each frame in RenderCubeVertices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1024.0 / 768.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 2
    glEnable(GL_DEPTH_TEST);

    glGenBuffers(1, &m_PosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_PosBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_ColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_colors), vertex_colors, GL_STATIC_DRAW);

    glGenBuffers(1, &m_IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 3
    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &m_VertexArray);
    glBindVertexArray(m_VertexArray);

    glGenBuffers(1, &m_PosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_PosBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &m_ColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_colors), vertex_colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &m_IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

#elif USE_OPENGL && OPENGL_VERSION_MAJOR >= 4
    // GL 4.5+ — Direct State Access (DSA), no bind-to-modify
    glEnable(GL_DEPTH_TEST);

    glCreateVertexArrays(1, &m_VertexArray);

    glCreateBuffers(1, &m_PosBuffer);
    glCreateBuffers(1, &m_ColorBuffer);
    glCreateBuffers(1, &m_IndexBuffer);

    glNamedBufferData(m_PosBuffer, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glNamedBufferData(m_ColorBuffer, sizeof(vertex_colors), vertex_colors, GL_STATIC_DRAW);
    glNamedBufferData(m_IndexBuffer, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);

    // Bind buffers into VAO binding slots
    glVertexArrayVertexBuffer(m_VertexArray, 0, m_PosBuffer, 0, 3 * sizeof(float));
    glVertexArrayVertexBuffer(m_VertexArray, 1, m_ColorBuffer, 0, 3 * sizeof(float));
    glVertexArrayElementBuffer(m_VertexArray, m_IndexBuffer);

    // Attribute 0: position (from binding slot 0)
    glEnableVertexArrayAttrib(m_VertexArray, 0);
    glVertexArrayAttribFormat(m_VertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_VertexArray, 0, 0);

    // Attribute 1: color (from binding slot 1)
    glEnableVertexArrayAttrib(m_VertexArray, 1);
    glVertexArrayAttribFormat(m_VertexArray, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_VertexArray, 1, 1);
#endif
}


void CubeLayer::RenderCubeVertices()
{
    glm::mat4 mvp = m_Camera.m_Projection * m_Camera.m_View *
    glm::rotate(glm::mat4(1.0f), glm::radians(m_Angle), glm::vec3(0.0f, 1.0f, 0.0f));

#if USE_OPENGL && OPENGL_VERSION_MAJOR == 1
    // Fixed-function: no shader, no VAO — feed vertices inline
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(m_Angle, 0.0f, 1.0f, 0.0f);

    // Index + vertex data stored as static locals (GL1 has no GPU buffers)
    static float vertices[] = {
         0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,
    };
    static float vertex_colors[] = {
        1.0f,0.4f,0.6f, 1.0f,0.9f,0.2f, 0.7f,0.3f,0.8f, 0.5f,0.3f,1.0f,
        0.2f,0.6f,1.0f, 0.6f,1.0f,0.4f, 0.6f,0.8f,0.8f, 0.4f,0.8f,0.8f,
    };
    static unsigned short triangle_indices[] = {
        0,1,2, 2,3,0,  0,3,7, 7,4,0,  2,6,7, 7,3,2,
        1,5,6, 6,2,1,  4,7,6, 6,5,4,  5,1,0, 0,4,5,
    };

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 36; ++i)
    {
        unsigned short idx = triangle_indices[i];
        glColor3f(vertex_colors[idx * 3 + 0], vertex_colors[idx * 3 + 1], vertex_colors[idx * 3 + 2]);
        glVertex3f(vertices[idx * 3 + 0], vertices[idx * 3 + 1], vertices[idx * 3 + 2]);
    }
    glEnd();

#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 2
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_Shader);

    GLint mvpLoc = glGetUniformLocation(m_Shader, "uMVP");
    GLint posLoc = glGetAttribLocation(m_Shader, "aPos");
    GLint colorLoc = glGetAttribLocation(m_Shader, "aColor");

    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindBuffer(GL_ARRAY_BUFFER, m_PosBuffer);
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, m_ColorBuffer);
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

    glDisableVertexAttribArray(posLoc);
    glDisableVertexAttribArray(colorLoc);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 3
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_Shader);

    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindVertexArray(m_VertexArray);   // VAO already has attrib pointers + EBO baked in
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
    glBindVertexArray(0);

#elif USE_OPENGL && OPENGL_VERSION_MAJOR >= 4
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   //
   //glEnable(GL_DEPTH_TEST);
   //glEnable(GL_CULL_FACE);
   //glFrontFace(GL_CCW);
   //glCullFace(GL_BACK);
    glUseProgram(m_Shader);

    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindVertexArray(m_VertexArray);   // DSA VAO — same bind-and-draw as GL3
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
    glBindVertexArray(0);
#endif
}