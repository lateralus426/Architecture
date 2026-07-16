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
#include<iostream>
#include "Models.h"


constexpr int keys[] =
{
    GLFW_KEY_LEFT,
    GLFW_KEY_RIGHT,
    GLFW_KEY_UP,
    GLFW_KEY_SPACE,
    GLFW_KEY_P,
    GLFW_KEY_R
};


TetrisAppLayer::TetrisAppLayer()
{
    std::println("Created new AppLayer!");

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

    m_WindowHandle = Core::Application::Get().GetWindow()->GetHandle();
	
    if (m_WindowHandle == nullptr)
	{
		std::println("Error: Window handle is null!");
		return;
	}

    //m_GameState.Init();
    //InitializeCube();
    InitializeCubeV1();
	//Init();

    // Aspect-correct projection
   
    float boardAspect = (float)BOARD_WIDTH / (float)BOARD_HEIGHT;

    float halfW = m_Camera.m_ScreenSize.x / 2, halfH = m_Camera.m_ScreenSize.y / 2;
    //if (aspect > boardAspect) {
    //    halfH = BOARD_HEIGHT / 2.0f + 1.0f;
    //    halfW = halfH * aspect;
    //}
    //else {
    //    halfW = BOARD_WIDTH / 2.0f + 1.0f;
    //    halfH = halfW / aspect;
    //}

    const float cx = BOARD_WIDTH * 0.5f;
    const float cy = BOARD_HEIGHT * 0.5f;

    glm::mat4 mvp = glm::ortho(
        cx - halfW, cx + halfW,
        cy - halfH, cy + halfH,
        -15.0f, 15.0f
    );

    //m_Camera.m_View = mvp;
}

TetrisAppLayer::~TetrisAppLayer()
{
//#if OPENGL_VERSION_MAJOR > 1 
//    glDeleteVertexArrays(1, &m_VertexArray);
//    glDeleteBuffers(1, &m_VertexBuffer);
//    glDeleteBuffers(1, &m_PosBuffer);
//    glDeleteBuffers(1, &m_ColorBuffer);
//    glDeleteBuffers(1, &m_IndexBuffer);
//
//
//   
//#endif

#if USE_OPENGL && OPENGL_VERSION_MAJOR == 1
    // No resources to release
#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 2
    if (m_VertexBuffer != 0) { glDeleteBuffers(1, &m_VertexBuffer); m_VertexBuffer = 0; }
    if (m_IndexBuffer != 0) { glDeleteBuffers(1, &m_IndexBuffer); m_IndexBuffer = 0; }
    glDeleteProgram(m_Shader);
#elif USE_OPENGL && OPENGL_VERSION_MAJOR >= 3
    if (m_VertexArray != 0) { glDeleteVertexArrays(1, &m_VertexArray); m_VertexArray = 0; }
    if (m_VertexBuffer != 0) { glDeleteBuffers(1, &m_VertexBuffer); m_VertexBuffer = 0; }
    if (m_IndexBuffer != 0) { glDeleteBuffers(1, &m_IndexBuffer); m_IndexBuffer = 0; }
    glDeleteProgram(m_Shader);
#endif
    m_Initialized = false;
}



void TetrisAppLayer::OnEvent(Core::Event &event)
{
    std::println("[Test]{}", event.ToString());

    Core::EventDispatcher dispatcher(event);



//	dispatcher.Dispatch<Core::KeyPressedEvent>([this](Core::KeyPressedEvent& e)
//		{ return false; });
    dispatcher.Dispatch<Core::MouseButtonPressedEvent>([this](Core::MouseButtonPressedEvent &e)
                                                       { return OnMouseButtonPressed(e); });
    dispatcher.Dispatch<Core::MouseMovedEvent>([this](Core::MouseMovedEvent &e)
                                               { return OnMouseMoved(e); });
    dispatcher.Dispatch<Core::WindowClosedEvent>([this](Core::WindowClosedEvent &e)
                                                 { return OnWindowClosed(e); });
}

void TetrisAppLayer::OnUpdate(float ts)
{
    m_Time += ts;
    m_CurrentTime += ts;
    m_Angle += ts * m_Speed;

    m_Angle >= 360.0f ? m_Angle = 0.0f : m_Angle;

    //std::println("------------------------------Start Grid --------------------------------------");
	//for ( int i = 0; i < BOARD_HEIGHT; ++i )
	//{
	//	for (int j = 0; j < BOARD_WIDTH; ++j)
	//	{
	//		int cell = m_GameState.m_Grid[i][j];
	//		std::cout << cell << " ";
	//	}
	//	std::cout << "\n";
	//}
    //
    //std::println("------------------------------End Grid --------------------------------------");


    if (glfwGetKey(m_WindowHandle, GLFW_KEY_1) == GLFW_PRESS)
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

    // Continuous soft drop: DOWN fires every frame while held
    if (glfwGetKey(m_WindowHandle, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (!m_GameState.m_GameOver && !m_GameState.m_Paused) {
            m_GameState.MoveDown();
            m_GameState.m_LastDropTime = m_CurrentTime;
        }
    }
    
    // One-shot keys: trigger once per press
	for (const auto& i : keys)
    {

        bool pressed = glfwGetKey(m_WindowHandle, i) == GLFW_PRESS;

      
        if (pressed && !m_KeysTriggered[i])
        {
            if (i == GLFW_KEY_LEFT) { 
                m_GameState.MoveLeft(); 
            }
            else if (i == GLFW_KEY_RIGHT) 
            { 
                m_GameState.MoveRight(); 
            }
            else if (i == GLFW_KEY_UP) 
            { 
                m_GameState.Rotate(); 
            }
            else if (i == GLFW_KEY_SPACE) 
            { 
                m_GameState.HardDrop(); 
            }
            else if (i == GLFW_KEY_P) 
            { 
                m_GameState.m_Paused = !m_GameState.m_Paused; 
            }
            else if (i == GLFW_KEY_R) 
            {
                if (m_GameState.m_GameOver) 
                {
                    m_GameState.Init();
                    m_GameState.m_LastDropTime = m_CurrentTime;
                }
            }
            m_KeysTriggered[i] = true;
        }

        if (!pressed) {
            m_KeysTriggered[i] = false; // reset so next press can fire
        }
    }
}

void TetrisAppLayer::OnRender()
{
    RenderCubeVertices();
    // 
	//Render(m_GameState, m_Camera.m_ScreenSize.x, m_Camera.m_ScreenSize.y);
}

bool TetrisAppLayer::OnMouseButtonPressed(Core::MouseButtonPressedEvent &event)
{
    glm::vec2 framebufferSize = glm::vec2(m_Camera.m_ScreenSize);
    float aspectRatio = framebufferSize.x / framebufferSize.y;
    glm::vec2 normalizedMousePos = (m_MousePosition / framebufferSize) * 2.0f - 1.0f;
    normalizedMousePos.x *= aspectRatio;
    normalizedMousePos.y *= -1.0f;
    normalizedMousePos.y += 0.7f;

    m_FlamePosition = -normalizedMousePos;

    return false;
}

bool TetrisAppLayer::OnMouseMoved(Core::MouseMovedEvent &event)
{
    m_MousePosition = {static_cast<float>(event.GetX()), static_cast<float>(event.GetY())};

    return false;
}

bool TetrisAppLayer::OnWindowClosed(Core::WindowClosedEvent &event)
{
    std::println("Window Closed!");

    return false;
}

void TetrisAppLayer::AddLineQuad(std::vector<Vertex> &verts, int gx1, int gy1, int gx2, int gy2, float r, float g, float b)
{
    float x1 = EdgeX(gx1);
    float x2 = EdgeX(gx2);
    float y1 = EdgeY(gy1);
    float y2 = EdgeY(gy2);
    float thick = 0.02f;

    if (y1 == y2)
    {
        // Horizontal line
        verts.push_back({x1, y1 - thick, 0.0f, r, g, b});
        verts.push_back({x2, y1 - thick, 0.0f, r, g, b});
        verts.push_back({x2, y1 + thick, 0.0f, r, g, b});
        verts.push_back({x1, y1 - thick, 0.0f, r, g, b});
        verts.push_back({x2, y1 + thick, 0.0f, r, g, b});
        verts.push_back({x1, y1 + thick, 0.0f, r, g, b});
    }
    else
    {
        // Vertical line
        verts.push_back({x1 - thick, y1, 0.0f, r, g, b});
        verts.push_back({x1 + thick, y1, 0.0f, r, g, b});
        verts.push_back({x1 + thick, y2, 0.0f, r, g, b});
        verts.push_back({x1 - thick, y1, 0.0f, r, g, b});
        verts.push_back({x1 + thick, y2, 0.0f, r, g, b});
        verts.push_back({x1 - thick, y2, 0.0f, r, g, b});
    }
}

// Helper: push a quad for given grid coords
void TetrisAppLayer::PushQuad(std::vector<Vertex> &verts, int x, int y, float r, float g, float b)
{
    float wx = WorldX(x);
    float wy = WorldY(y);
    float inset = 0.05f;

    verts.push_back({wx - CELL_SIZE / 2 + inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b});
    verts.push_back({wx + CELL_SIZE / 2 - inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b});
    verts.push_back({wx + CELL_SIZE / 2 - inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b});
    verts.push_back({wx - CELL_SIZE / 2 + inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b});
    verts.push_back({wx + CELL_SIZE / 2 - inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b});
    verts.push_back({wx - CELL_SIZE / 2 + inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b});
}

void TetrisAppLayer::RenderNextPiece(std::vector<Vertex> &verts, PieceType type, const std::vector<std::pair<int, int>> &cells)
{
    // type is valid (I=0 through L=6), no EMPTY variant
    int idx = static_cast<int>(type);
    float r = PIECE_COLORS[idx][0];
    float g = PIECE_COLORS[idx][1];
    float b = PIECE_COLORS[idx][2];

    // Preview box dimensions
    int boxLeft = BOARD_WIDTH + 2;
    int boxRight = BOARD_WIDTH + 5;
    int boxTop = 6;     // top row
    int boxBottom = 16; // bottom row

    // Background for preview box (drawn first)
    for (int y = boxTop; y < boxBottom; ++y)
    {
        for (int x = boxLeft; x < boxRight; ++x)
        {
            PushQuad(verts, x, y, 0.08f, 0.08f, 0.14f);
        }
    }

    // Border for preview box
    AddLineQuad(verts, boxLeft, boxTop, boxLeft, boxBottom, 0.35f, 0.35f, 0.5f);
    AddLineQuad(verts, boxRight, boxTop, boxRight, boxBottom, 0.35f, 0.35f, 0.5f);
    AddLineQuad(verts, boxLeft, boxTop, boxRight, boxTop, 0.35f, 0.35f, 0.5f);
    AddLineQuad(verts, boxLeft, boxBottom, boxRight, boxBottom, 0.35f, 0.35f, 0.5f);

    // Center the piece in the preview box
    int minR = 100, maxR = -1, minC = 100, maxC = -1;
    for (auto &[dr, dc] : cells)
    {
        minR = std::min(minR, dr);
        maxR = std::max(maxR, dr);
        minC = std::min(minC, dc);
        maxC = std::max(maxC, dc);
    }

    // Center position in preview box
    int centerX = (boxLeft + boxRight) / 2;
    int centerY = (boxTop + boxBottom) / 2;
    int offsetX = centerX - (minC + maxC) / 2;
    int offsetY = centerY - (minR + maxR) / 2;

    // Draw the piece
    for (auto &[dr, dc] : cells)
    {
        int rx = dc + offsetX;
        int ry = dr + offsetY;
        PushQuad(verts, rx, ry, r, g, b);
    }
}

void TetrisAppLayer::Render(const GameState &state, int winW, int winH)
{
    std::vector<Vertex> vertices;
    vertices.reserve(BOARD_WIDTH * BOARD_HEIGHT * VERTS_PER_QUAD * 3);

    auto addQuad = [&](int x, int y, float r, float g, float b)
    {
        float wx = WorldX(x);
        float wy = WorldY(y);
        float inset = 0.05f;

        vertices.push_back({wx - CELL_SIZE / 2 + inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b});
        vertices.push_back({wx + CELL_SIZE / 2 - inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b});
        vertices.push_back({wx + CELL_SIZE / 2 - inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b});

        vertices.push_back({wx - CELL_SIZE / 2 + inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b});
        vertices.push_back({wx + CELL_SIZE / 2 - inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b});
        vertices.push_back({wx - CELL_SIZE / 2 + inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b});
    };

    // 1) Background (dark pit)
    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            addQuad(x, y, 0.06f, 0.06f, 0.1f);
        }
    }

    // 2) Grid lines (on top of background)
    for (int x = 0; x <= BOARD_WIDTH; ++x)
    {
        AddLineQuad(vertices, x, 0, x, BOARD_HEIGHT, 0.2f, 0.2f, 0.28f);
    }
    for (int y = 0; y <= BOARD_HEIGHT; ++y)
    {
        AddLineQuad(vertices, 0, y, BOARD_WIDTH, y, 0.2f, 0.2f, 0.28f);
    }

    // 3) Placed pieces from grid
    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            if (state.m_Grid[y][x] > 0)
            {
                int idx = state.m_Grid[y][x] - 1;
                addQuad(x, y,
                        PIECE_COLORS[idx][0],
                        PIECE_COLORS[idx][1],
                        PIECE_COLORS[idx][2]);
            }
        }
    }

    // 4) Outer border (matches grid edges exactly)
    AddLineQuad(vertices, 0, 0, 0, BOARD_HEIGHT, 0.45f, 0.45f, 0.6f);
    AddLineQuad(vertices, BOARD_WIDTH, 0, BOARD_WIDTH, BOARD_HEIGHT, 0.45f, 0.45f, 0.6f);
    AddLineQuad(vertices, 0, 0, BOARD_WIDTH, 0, 0.45f, 0.45f, 0.6f);
    AddLineQuad(vertices, 0, BOARD_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT, 0.45f, 0.45f, 0.6f);

    // 5) Ghost piece (read-only computation)
    if (!state.m_GameOver)
    {
        int ghostRow = -1;
        for (int testRow = state.m_CurrentRow; testRow < BOARD_HEIGHT; ++testRow)
        {
            bool canDrop = true;
            for (auto &[dr, dc] : state.m_CurrentCells)
            {
                int r = testRow + dr;
                int c = m_GameState.m_CurrentCol + dc;
                if (c < 0 || c >= BOARD_WIDTH)
                {
                    canDrop = false;
                    break;
                }
                if (r >= BOARD_HEIGHT)
                {
                    canDrop = false;
                    break;
                }
                if (r >= 0 && state.m_Grid[r][c] != 0)
                {
                    canDrop = false;
                    break;
                }
            }

            if (!canDrop)
            {
                ghostRow = testRow - 1;
                break;
            }
            if (testRow == BOARD_HEIGHT - 1)
                ghostRow = testRow;
        }

        if (ghostRow >= 0 && ghostRow != state.m_CurrentRow)
        {
            int idx = static_cast<int>(state.m_CurrentType);
            for (auto &[dr, dc] : state.m_CurrentCells)
            {
                int r = ghostRow + dr;
                int c = m_GameState.m_CurrentCol + dc;
                if (r >= 0 && r < BOARD_HEIGHT && c >= 0 && c < BOARD_WIDTH)
                {
                    addQuad(c, r,
                            PIECE_COLORS[idx][0] * 0.3f,
                            PIECE_COLORS[idx][1] * 0.3f,
                            PIECE_COLORS[idx][2] * 0.3f);
                }
            }
        }
    }

    // 6) Current piece (on top)
    if (!state.m_GameOver)
    {
        int idx = static_cast<int>(state.m_CurrentType);
        for (auto &[dr, dc] : state.m_CurrentCells)
        {
            int r = state.m_CurrentRow + dr;
            int c = state.m_CurrentCol + dc;
            if (r >= 0 && r < BOARD_HEIGHT && c >= 0 && c < BOARD_WIDTH)
            {
                addQuad(c, r,
                        PIECE_COLORS[idx][0],
                        PIECE_COLORS[idx][1],
                        PIECE_COLORS[idx][2]);
            }
        }
    }

    // 7) Next piece preview (right side of screen)
    RenderNextPiece(vertices, m_GameState.m_NextType, m_GameState.m_NextCells);

#if USE_OPENGL && OPENGL_VERSION_MAJOR == 1
    
        // Update projection for aspect ratio
	const float aspect = m_Camera.m_AspectRatio;
            //static_cast<float>(winW) / static_cast<float>(winH);
        const float viewHeight = 24.0f;
        const float viewWidth = viewHeight * aspect;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-viewWidth * 0.5f, viewWidth * 0.5f,
            -viewHeight * 0.5f, viewHeight * 0.5f, -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Draw using immediate mode
        glBegin(GL_TRIANGLES);
        for (const Vertex& v    : vertices) {
            //const Vertex& v = buf.vertices[idx];
            glColor3f(v.r, v.g, v.b);
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 2
	// TODO: Use shader program not done 
    //if (m_Shader) {
    //    m_Shader->use();
    //}
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    //    buf.indices.size() * sizeof(GLuint),
    //    buf.indices.data(), GL_DYNAMIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(Vertex),reinterpret_cast<const void*>(offsetof(Vertex, x)));

    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,sizeof(Vertex),reinterpret_cast<const void*>(offsetof(Vertex, r)));

    //glDrawElements(GL_TRIANGLES,
    //    static_cast<GLsizei>(buf.indices.size()),
    //    GL_UNSIGNED_INT, nullptr);


    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 3
    glBindVertexArray(m_VertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(), GL_DYNAMIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    //    buf.indices.size() * sizeof(GLuint),
    //    buf.indices.data(), GL_DYNAMIC_DRAW);

    // Setup attribute pointers (VAO remembers these)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<const void*>(offsetof(Vertex, x)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<const void*>(offsetof(Vertex, r)));
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    
    //glDrawElements(GL_TRIANGLES,
    //    static_cast<GLsizei>(buf.indices.size()),
    //    GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);

#elif USE_OPENGL && OPENGL_VERSION_MAJOR >= 4
    //if (shader_) shader_->use();

    // DSA - no binding required
    glNamedBufferData(m_VertexBuffer,
        vertices.size() * sizeof(Vertex),
        vertices.data(), GL_DYNAMIC_DRAW);
    //glNamedBufferData(ibo_,
    //    buf.indices.size() * sizeof(GLuint),
    //    buf.indices.data(), GL_DYNAMIC_DRAW);

    // Bind vertex buffer to VAO
    glVertexArrayVertexBuffer(m_VertexArray, 0, m_VertexBuffer, 0, sizeof(Vertex));
    // Bind index buffer to VAO
    //glVertexArrayElementBuffer(m_VertexArray, m_IndexBuffer);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
   // glDrawElements(GL_TRIANGLES,
   //     static_cast<GLsizei>(buf.indices.size()),
   //     GL_UNSIGNED_INT, nullptr);
#endif
}

void TetrisAppLayer::Init()
{

   
    if (m_Initialized) 
        return;

#if USE_OPENGL && OPENGL_VERSION_MAJOR == 1
        // Fixed-function pipeline - no buffer objects needed
        glEnable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-12.0f, 12.0f, -12.0f, 12.0f, -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 2
        glEnable(GL_DEPTH_TEST);
        glGenBuffers(1, &m_VertexBuffer);
        glGenBuffers(1, &m_IndexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 3
        glEnable(GL_DEPTH_TEST);

        glGenVertexArrays(1, &m_VertexArray);
        glBindVertexArray(m_VertexArray);

        glGenBuffers(1, &m_VertexBuffer);
        glGenBuffers(1, &m_IndexBuffer);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

#elif USE_OPENGL && OPENGL_VERSION_MAJOR >= 4
        glEnable(GL_DEPTH_TEST);

        glCreateVertexArrays(1, &m_VertexArray);
        glCreateBuffers(1, &m_VertexBuffer);
        glCreateBuffers(1, &m_IndexBuffer);

        // Setup vertex format - interleaved buffer
        // Attribute 0: position
        glEnableVertexArrayAttrib(m_VertexArray, 0);
        glVertexArrayAttribFormat(m_VertexArray, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, x));
        glVertexArrayAttribBinding(m_VertexArray, 0, 0);

        // Attribute 1: color
        glEnableVertexArrayAttrib(m_VertexArray, 1);
        glVertexArrayAttribFormat(m_VertexArray, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, r));
        glVertexArrayAttribBinding(m_VertexArray, 1, 0);
#endif

        m_Initialized = true;
}

void TetrisAppLayer::InitializeCube()
{
     float vertices[] = {
        // Front face
        0.5,
        0.5,
        0.5,
        -0.5,
        0.5,
        0.5,
        -0.5,
        -0.5,
        0.5,
        0.5,
        -0.5,
        0.5,

        // Back face
        0.5,
        0.5,
        -0.5,
        -0.5,
        0.5,
        -0.5,
        -0.5,
        -0.5,
        -0.5,
        0.5,
        -0.5,
        -0.5,
    };

    float vertex_colors[] = {
        1.0,
        0.4,
        0.6,
        1.0,
        0.9,
        0.2,
        0.7,
        0.3,
        0.8,
        0.5,
        0.3,
        1.0,

        0.2,
        0.6,
        1.0,
        0.6,
        1.0,
        0.4,
        0.6,
        0.8,
        0.8,
        0.4,
        0.8,
        0.8,
    };

    unsigned short triangle_indices[] = {
        // Front
        0,
        1,
        2,
        2,
        3,
        0,

        // Right
        0,
        3,
        7,
        7,
        4,
        0,

        // Bottom
        2,
        6,
        7,
        7,
        3,
        2,

        // Left
        1,
        5,
        6,
        6,
        2,
        1,

        // Back
        4,
        7,
        6,
        6,
        5,
        4,

        // Top
        5,
        1,
        0,
        0,
        4,
        5,
    };
/*
#if USE_OPENGL && OPENGL_VERSION_MAJOR == 1 
    auto SetupCube_GL1 = []()
    {
        glEnable(GL_DEPTH_TEST);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 1024.0 / 768.0, 0.1, 100.0);

        glMatrixMode(GL_MODELVIEW);
    };
#elif USE_OPENGL && OPENGL_VERSION_MAJOR 2
    auto setupCube_GL20 = [&](){
        glEnable(GL_DEPTH_TEST);

        //gProgram = CreateProgram(vs_20, fs_20); // assume helper exists

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

    };
#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 3
    auto SetupCube_GL33 = [&]()
        {
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
        };
#endif

#if USE_OPENGL && OPENGL_VERSION_MAJOR == 2 
    setupCube_GL20();
#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 3 //3.3
    SetupCube_GL33();


#else */
#if USE_OPENGL && OPENGL_VERSION_MAJOR > 1
    // VAO stores the attribute layout and buffer bindings
    glCreateVertexArrays(1, &m_VertexArray); //  this IS the VAO

    // VBOs the raw GPU buffers with your data
    
    glCreateBuffers(1, &m_PosBuffer);   //  VBO #1: positions
    glCreateBuffers(1, &m_ColorBuffer); //  VBO #2: colors
    glCreateBuffers(1, &m_IndexBuffer); //  EBO: triangle indices

    // Upload data into the VBOs (no binding needed with DSA)
    glNamedBufferData(m_PosBuffer, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glNamedBufferData(m_ColorBuffer, sizeof(vertex_colors), vertex_colors, GL_STATIC_DRAW);
    glNamedBufferData(m_IndexBuffer, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);

    // Wire the VBOs into the VAO
    glVertexArrayVertexBuffer(m_VertexArray, 0, m_PosBuffer, 0, 3 * sizeof(float));   //  VAO learns about VBO #1
    glVertexArrayVertexBuffer(m_VertexArray, 1, m_ColorBuffer, 0, 3 * sizeof(float)); //  VAO learns about VBO #2
    glVertexArrayElementBuffer(m_VertexArray, m_IndexBuffer);                         //  VAO learns about EBO

    // glVertexArrayAttribBinding(m_VertexArray, 0, 0);
    // glVertexArrayAttribBinding(m_VertexArray, 1, 1);

    // position
    glEnableVertexArrayAttrib(m_VertexArray, 0);
    glVertexArrayAttribFormat(m_VertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_VertexArray, 0, 0);

    // color
    glEnableVertexArrayAttrib(m_VertexArray, 1);
    glVertexArrayAttribFormat(m_VertexArray, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_VertexArray, 1, 1);
#endif
}
void TetrisAppLayer::UpdateCube()
{
  
}

void TetrisAppLayer::InitializeCubeV1()
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

    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const double top = m_Camera.m_NearPlane * tan(m_Camera.m_FOV * 3.14159265 / 360.0);
    const double bottom = -top;
    const double right = top * m_Camera.m_Aspect;
    const double left = -right;

    glFrustum(left, right, bottom, top, m_Camera.m_NearPlane, m_Camera.m_FarPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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


void TetrisAppLayer::RenderCubeVertices()
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


static constexpr float worldX(int gx) noexcept {
    return gx - BOARD_WIDTH * 0.5f + CELL_SIZE * 0.5f;
}

static constexpr float worldY(int gy) noexcept {
    return -gy + BOARD_HEIGHT * 0.5f - 0.5f;
}

static constexpr float edgeX(int gx) noexcept {
    return gx - BOARD_WIDTH * 0.5f;
}

static constexpr float edgeY(int gy) noexcept {
    return BOARD_HEIGHT * 0.5f - gy;
}



static void pushQuad(VertexBuffer& buf, float cx, float cy,
    float halfW, float halfH,
    float r, float g, float b) noexcept {
    const GLuint base = static_cast<GLuint>(buf.vertices.size());

    buf.vertices.push_back({ cx - halfW, cy - halfH, 0.0f, r, g, b });
    buf.vertices.push_back({ cx + halfW, cy - halfH, 0.0f, r, g, b });
    buf.vertices.push_back({ cx + halfW, cy + halfH, 0.0f, r, g, b });
    buf.vertices.push_back({ cx - halfW, cy + halfH, 0.0f, r, g, b });

    buf.indices.push_back(base);
    buf.indices.push_back(base + 1);
    buf.indices.push_back(base + 2);
    buf.indices.push_back(base);
    buf.indices.push_back(base + 2);
    buf.indices.push_back(base + 3);
}


static void pushCellQuad(VertexBuffer& buf, int gx, int gy,
    float r, float g, float b) noexcept {
    const float halfSize = (CELL_SIZE - QUAD_INSET * 2.0f) * 0.5f;
    pushQuad(buf, worldX(gx), worldY(gy), halfSize, halfSize, r, g, b);
}

static void pushLineQuad(VertexBuffer& buf,
    int gx1, int gy1, int gx2, int gy2,
    float r, float g, float b) noexcept {
    const float x1 = edgeX(gx1);
    const float x2 = edgeX(gx2);
    const float y1 = edgeY(gy1);
    const float y2 = edgeY(gy2);

    if (std::abs(y1 - y2) < 0.001f) {
        // Horizontal line
        const float cx = (x1 + x2) * 0.5f;
        const float halfW = std::abs(x2 - x1) * 0.5f;
        pushQuad(buf, cx, y1, halfW, LINE_THICKNESS, r, g, b);
    }
    else {
        // Vertical line
        const float cy = (y1 + y2) * 0.5f;
        const float halfH = std::abs(y2 - y1) * 0.5f;
        pushQuad(buf, x1, cy, LINE_THICKNESS, halfH, r, g, b);
    }
}


static void pushBoxBorder(VertexBuffer& buf, int left, int top,
    int right, int bottom,
    float r, float g, float b) noexcept {
    pushLineQuad(buf, left, top, left, bottom, r, g, b);
    pushLineQuad(buf, right, top, right, bottom, r, g, b);
    pushLineQuad(buf, left, top, right, top, r, g, b);
    pushLineQuad(buf, left, bottom, right, bottom, r, g, b);
}

static void pushBoxBackground(VertexBuffer& buf, int left, int top,
    int right, int bottom,
    float r, float g, float b) noexcept {
    for (int y = top; y < bottom; ++y) {
        for (int x = left; x < right; ++x) {
            pushCellQuad(buf, x, y, r, g, b);
        }
    }
}




static void buildNextPiecePreview(const GameState& state, VertexBuffer& buf)
{
    constexpr int boxLeft = BOARD_WIDTH + 2;
    constexpr int boxRight = BOARD_WIDTH + 5;
    constexpr int boxTop = 6;
    constexpr int boxBottom = 16;
    constexpr float previewBgR = 0.08f, previewBgG = 0.08f, previewBgB = 0.14f;
    constexpr float borderR = 0.35f, borderG = 0.35f, borderB = 0.5f;

    // Background
    pushBoxBackground(buf, boxLeft, boxTop, boxRight, boxBottom,
        previewBgR, previewBgG, previewBgB);

    // Border
    pushBoxBorder(buf, boxLeft, boxTop, boxRight, boxBottom,
        borderR, borderG, borderB);

    // Calculate piece bounds for centering
    int minR = std::numeric_limits<int>::max();
    int maxR = std::numeric_limits<int>::min();
    int minC = std::numeric_limits<int>::max();
    int maxC = std::numeric_limits<int>::min();

    for (const auto& [dr, dc] : state.m_NextCells) {
        minR = std::min(minR, dr);
        maxR = std::max(maxR, dr);
        minC = std::min(minC, dc);
        maxC = std::max(maxC, dc);
    }

    const int centerX = (boxLeft + boxRight) / 2;
    const int centerY = (boxTop + boxBottom) / 2;
    const int offsetX = centerX - (minC + maxC) / 2;
    const int offsetY = centerY - (minR + maxR) / 2;

    // Draw piece
    const int idx = static_cast<int>(state.m_NextType);
    for (const auto& [dr, dc] : state.m_NextCells) {
        pushCellQuad(buf, dc + offsetX, dr + offsetY,
            PIECE_COLORS[idx][0],
            PIECE_COLORS[idx][1],
            PIECE_COLORS[idx][2]);
    }
}


static int computeGhostRow(const GameState& state) noexcept {
    for (int testRow = state.m_CurrentRow; testRow < BOARD_HEIGHT; ++testRow) {
        for (const auto& [dr, dc] : state.m_CurrentCells) {
            const int r = testRow + dr;
            const int c = state.m_CurrentCol + dc;
            if (c < 0 || c >= BOARD_WIDTH || r >= BOARD_HEIGHT) {
                return testRow - 1;
            }
            if (r >= 0 && state.m_Grid[r][c] != 0) {
                return testRow - 1;
            }
        }
    }
    return BOARD_HEIGHT - 1;
}

VertexBuffer buildFrameGeometry(const GameState& state) {
    VertexBuffer buf;
    // Pre-allocate for worst case
    constexpr size_t maxQuads = BOARD_WIDTH * BOARD_HEIGHT * 3 + 100;
    buf.reserve(maxQuads);

    // 1. Board background
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            pushCellQuad(buf, x, y, 0.06f, 0.06f, 0.1f);
        }
    }

    // 2. Grid lines
    for (int x = 0; x <= BOARD_WIDTH; ++x) {
        pushLineQuad(buf, x, 0, x, BOARD_HEIGHT, 0.2f, 0.2f, 0.28f);
    }
    for (int y = 0; y <= BOARD_HEIGHT; ++y) {
        pushLineQuad(buf, 0, y, BOARD_WIDTH, y, 0.2f, 0.2f, 0.28f);
    }

    // 3. Placed pieces
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            if (state.m_Grid[y][x] > 0) {
                const int idx = state.m_Grid[y][x] - 1;
                pushCellQuad(buf, x, y,
                    PIECE_COLORS[idx][0],
                    PIECE_COLORS[idx][1],
                    PIECE_COLORS[idx][2]);
            }
        }
    }

    // 4. Board border
    constexpr float borderR = 0.45f, borderG = 0.45f, borderB = 0.6f;
    pushLineQuad(buf, 0, 0, 0, BOARD_HEIGHT, borderR, borderG, borderB);
    pushLineQuad(buf, BOARD_WIDTH, 0, BOARD_WIDTH, BOARD_HEIGHT, borderR, borderG, borderB);
    pushLineQuad(buf, 0, 0, BOARD_WIDTH, 0, borderR, borderG, borderB);
    pushLineQuad(buf, 0, BOARD_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT, borderR, borderG, borderB);

    // 5. Ghost piece
    if (!state.m_GameOver) {
        const int ghostRow = computeGhostRow(state);
        if (ghostRow >= 0 && ghostRow != state.m_CurrentRow) {
            const int idx = static_cast<int>(state.m_CurrentType);
            for (const auto& [dr, dc] : state.m_CurrentCells) {
                const int r = ghostRow + dr;
                const int c = state.m_CurrentCol + dc;
                if (r >= 0 && r < BOARD_HEIGHT && c >= 0 && c < BOARD_WIDTH) {
                    pushCellQuad(buf, c, r,
                        PIECE_COLORS[idx][0] * 0.3f,
                        PIECE_COLORS[idx][1] * 0.3f,
                        PIECE_COLORS[idx][2] * 0.3f);
                }
            }
        }

        // 6. Current piece
        const int idx = static_cast<int>(state.m_CurrentType);
        for (const auto& [dr, dc] : state.m_CurrentCells) {
            const int r = state.m_CurrentRow + dr;
            const int c = state.m_CurrentCol + dc;
            if (r >= 0 && r < BOARD_HEIGHT && c >= 0 && c < BOARD_WIDTH) {
                pushCellQuad(buf, c, r,
                    PIECE_COLORS[idx][0],
                    PIECE_COLORS[idx][1],
                    PIECE_COLORS[idx][2]);
            }
        }
    }

    // 7. Next piece preview
    buildNextPiecePreview(state, buf);

    return buf;
}