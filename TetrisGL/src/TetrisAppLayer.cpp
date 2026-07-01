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

#include "Models.h"

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

    m_Shader = Renderer::CreateGraphicsShader(vertexShader, fragmentShader);
    std::println("Finished compiling the shaders\n");

    // Create geometry
    glCreateVertexArrays(1, &m_VertexArray); // vertex array object (VAO)
    glCreateBuffers(1, &m_VertexBuffer);

    m_Camera.m_Position = glm::vec3(0.0f, 0.0f, -5.0f);
    m_Camera.m_Direction = glm::vec3(0.0f, 0.0f, 1.0f);
    m_Camera.m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_Camera.m_Fov = 45.0f;
    glm::vec2 framebufferSize = Core::Application::Get().GetFramebufferSize();
    float aspect = framebufferSize.x / framebufferSize.y;
    m_Camera.m_AspectRatio = aspect;
    m_Camera.m_NearPlane = 0.1f;
    m_Camera.m_FarPlane = 100.0f;
    m_Camera.ScreenSize = glm::ivec2(framebufferSize.x, framebufferSize.y);

    glm::mat4 view = glm::lookAt(m_Camera.m_Position, m_Camera.m_Position + m_Camera.m_Direction, m_Camera.m_Up);
    glm::mat4 projection = glm::perspective(glm::radians(m_Camera.m_Fov), m_Camera.m_AspectRatio, m_Camera.m_NearPlane, m_Camera.m_FarPlane);

    m_Camera.m_View = view;
    m_Camera.m_Projection = projection;

    //InitializeCube();
    InitializeCubeV1();
}

TetrisAppLayer::~TetrisAppLayer()
{
    glDeleteVertexArrays(1, &m_VertexArray);
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_PosBuffer);
    glDeleteBuffers(1, &m_ColorBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);


    glDeleteProgram(m_Shader);
}



void TetrisAppLayer::OnEvent(Core::Event &event)
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

void TetrisAppLayer::OnUpdate(float ts)
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

    // Continuous soft drop: DOWN fires every frame while held
    // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    //    if (!m_GameState.m_GameOver && !m_GameState.m_Paused) {
    //        m_GameState.MoveDown();
    //        m_GameState.m_LastDropTime = m_CurrentTime;
    //    }
    //}

    // One-shot keys: trigger once per press
    for (int i = 0; i <= GLFW_KEY_LAST; ++i)
    {

        // bool pressed = (glfwGetKey(window, i) == GLFW_PRESS);

        // if (pressed && !keysTriggered[i]) {
        //     if (i == GLFW_KEY_LEFT) { m_GameState.MoveLeft(); }
        //     else if (i == GLFW_KEY_RIGHT) { m_GameState.MoveRight(); }
        //     else if (i == GLFW_KEY_UP) { m_GameState.Rotate(); }
        //     else if (i == GLFW_KEY_SPACE) { m_GameState.HardDrop(); }
        //     else if (i == GLFW_KEY_P) { m_GameState.m_Paused = !m_GameState.m_Paused; }
        //     else if (i == GLFW_KEY_R) {
        //         if (m_GameState.m_GameOver) {
        //             m_GameState.Init();
        //             m_GameState.m_LastDropTime = m_CurrentTime;
        //         }
        //     }
        //     keysTriggered[i] = true;
        // }

        // if (!pressed) {
        //     keysTriggered[i] = false; // reset so next press can fire
        // }
    }
}

void TetrisAppLayer::OnRender()
{
    // glm::vec2 framebufferSize = Core::Application::Get().GetFramebufferSize();
    // float aspect = framebufferSize.x / framebufferSize.y;

    //glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
	//
    //
    //glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(m_Angle), glm::vec3(0.0f, 1.0f, 0.0f));
    //model = glm::translate(model, glm::vec3(10.0f, 1.0f, -5.0f));
    //// glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    //glm::mat4 mvp = m_Camera.m_Projection * m_Camera.m_View * model;

    //glm::mat4 model(1.0f);
    //model = glm::rotate(model, glm::radians(m_Angle), glm::vec3(0.0f, 1.0f, 0.0f));
    //model[3] = glm::vec4(-5.0f, 5.0f, 10.0f, 1.0f); // translation column
    ////model[0][3] = 2.0f; // column 0, row 1
    ////model[1][3] = 5.0f; // column 2, row 3
    ////model = (model, 2, glm::vec4(5.0f, 6.0f, 7.0f, 8.0f)); // third row
    //glm::mat4 mvp = m_Camera.m_Projection * m_Camera.m_View * model;
    //glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));
    //
    //
    //glUseProgram(m_Shader);
    //glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));
    //
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glViewport(0, 0, m_Camera.ScreenSize.x, m_Camera.ScreenSize.y);
    //glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CCW);
    //glCullFace(GL_BACK);
    //
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glBindVertexArray(m_VertexArray);
    //// glDrawArrays(GL_TRIANGLES, 0, 3);
    //
    //// glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);

    RenderCubeVertices();
}

bool TetrisAppLayer::OnMouseButtonPressed(Core::MouseButtonPressedEvent &event)
{
    glm::vec2 framebufferSize = glm::vec2(m_Camera.ScreenSize);
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

    // Draw
#ifdef GL3_PLUS_SUPPORT
    // glBindVertexArray(vao);
    // glBindVertexArray(m_VertexArray);
    // glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    ////glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
    // glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    // glBindVertexArray(0);
#else
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(3 * sizeof(float)));

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

void TetrisAppLayer::Init()
{
    // #ifdef GL3_PLUS_SUPPORT
    //         glGenVertexArrays(1, &vao);
    //         glGenBuffers(1, &vbo);
    //
    //         glBindVertexArray(vao);
    //         glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //
    //         glEnableVertexAttribArray(0);
    //         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    //         glEnableVertexAttribArray(1);
    //         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    ///
    //        glBindVertexArray(0);
    // #else
    //        glGenBuffers(1, &vbo);
    // #endif
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

#elif USE_OPENGL && OPENGL_VERSION_MAJOR >= 4

#else */
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
//#endif
}
void TetrisAppLayer::UpdateCube()
{
  
}

/*void TetrisAppLayer::RenderCubeVertices()
{
#if USE_OPENGL && OPENGL_VERSION_MAJOR == 1 
    auto RenderCube_GL1 = []()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -5.0f);
        glRotatef(m_Angle, 0.0f, 1.0f, 0.0f);

        glBegin(GL_TRIANGLES);
        for (int i = 0; i < 36; ++i)
        {
            unsigned short idx = triangle_indices[i];

            glColor3f(
                vertex_colors[idx * 3 + 0],
                vertex_colors[idx * 3 + 1],
                vertex_colors[idx * 3 + 2]
            );

            glVertex3f(
                vertices[idx * 3 + 0],
                vertices[idx * 3 + 1],
                vertices[idx * 3 + 2]
            );
        }
        glEnd();
    }
#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 2
    auto RenderCube_GL20 = [&](const float* mvp)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
            glUseProgram(m_Shader);
    
            GLint mvpLoc = glGetUniformLocation(m_Shader, "uMVP");
            GLint posLoc = glGetAttribLocation(m_Shader, "aPos");
            GLint colorLoc = glGetAttribLocation(m_Shader, "aColor");
    
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp);
    
            glBindBuffer(GL_ARRAY_BUFFER, gPosBuffer);
            glEnableVertexAttribArray(posLoc);
            glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
            glBindBuffer(GL_ARRAY_BUFFER, gColorBuffer);
            glEnableVertexAttribArray(colorLoc);
            glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
    
            glDisableVertexAttribArray(posLoc);
            glDisableVertexAttribArray(colorLoc);
        };
#elif USE_OPENGL && OPENGL_VERSION_MAJOR == 3
    auto 
#endif
}*/



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