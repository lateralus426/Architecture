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



	  // VAO stores the attribute layout and buffer bindings
	  glCreateVertexArrays(1, &m_VertexArray);        //  this IS the VAO

	  // VBOs the raw GPU buffers with your data
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
	  glVertexArrayAttribFormat(m_VertexArray, 0, 3, GL_FLOAT, GL_FALSE,0);
	  glVertexArrayAttribBinding(m_VertexArray, 0, 0);

	  // color
	  glEnableVertexArrayAttrib(m_VertexArray, 1);
	  glVertexArrayAttribFormat(m_VertexArray,1,3,GL_FLOAT,GL_FALSE,0);
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

	// Auto-drop (gravity)
    if (!m_GameState.m_GameOver && !m_GameState.m_Paused) {
        if ((m_CurrentTime - m_GameState.m_LastDropTime) * 1000.0f > m_GameState.m_DropInterval) {
            m_GameState.MoveDown();
            m_GameState.m_LastDropTime = m_CurrentTime;
        }
    }

    // Continuous soft drop: DOWN fires every frame while held
    //if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    //    if (!m_GameState.m_GameOver && !m_GameState.m_Paused) {
    //        m_GameState.MoveDown();
    //        m_GameState.m_LastDropTime = m_CurrentTime;
    //    }
    //}

    // One-shot keys: trigger once per press
    for (int i = 0; i <= GLFW_KEY_LAST; ++i) 
	{
            
        //bool pressed = (glfwGetKey(window, i) == GLFW_PRESS);

        //if (pressed && !keysTriggered[i]) {
        //    if (i == GLFW_KEY_LEFT) { m_GameState.MoveLeft(); }
        //    else if (i == GLFW_KEY_RIGHT) { m_GameState.MoveRight(); }
        //    else if (i == GLFW_KEY_UP) { m_GameState.Rotate(); }
        //    else if (i == GLFW_KEY_SPACE) { m_GameState.HardDrop(); }
        //    else if (i == GLFW_KEY_P) { m_GameState.m_Paused = !m_GameState.m_Paused; }
        //    else if (i == GLFW_KEY_R) {
        //        if (m_GameState.m_GameOver) {
        //            m_GameState.Init();
        //            m_GameState.m_LastDropTime = m_CurrentTime;
        //        }
        //    }
        //    keysTriggered[i] = true;
        //}

        //if (!pressed) {
        //    keysTriggered[i] = false; // reset so next press can fire
        //}
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

	m_Angle += m_Time*0.001f;



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
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
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


void TetrisAppLayer::AddLineQuad(std::vector<Vertex>& verts, int gx1, int gy1, int gx2, int gy2, float r, float g, float b) 
{
	float x1 = EdgeX(gx1);
	float x2 = EdgeX(gx2);
	float y1 = EdgeY(gy1);
	float y2 = EdgeY(gy2);
	float thick = 0.02f;

	if (y1 == y2) 
	{
		// Horizontal line
		verts.push_back({ x1, y1 - thick, 0.0f, r, g, b });
		verts.push_back({ x2, y1 - thick, 0.0f, r, g, b });
		verts.push_back({ x2, y1 + thick, 0.0f, r, g, b });
		verts.push_back({ x1, y1 - thick, 0.0f, r, g, b });
		verts.push_back({ x2, y1 + thick, 0.0f, r, g, b });
		verts.push_back({ x1, y1 + thick, 0.0f, r, g, b });
	}
	else 
	{
		// Vertical line
		verts.push_back({ x1 - thick, y1, 0.0f, r, g, b });
		verts.push_back({ x1 + thick, y1, 0.0f, r, g, b });
		verts.push_back({ x1 + thick, y2, 0.0f, r, g, b });
		verts.push_back({ x1 - thick, y1, 0.0f, r, g, b });
		verts.push_back({ x1 + thick, y2, 0.0f, r, g, b });
		verts.push_back({ x1 - thick, y2, 0.0f, r, g, b });
	}
}

// Helper: push a quad for given grid coords
void TetrisAppLayer::PushQuad(std::vector<Vertex>& verts, int x, int y, float r, float g, float b) 
{
	float wx = WorldX(x);
	float wy = WorldY(y);
	float inset = 0.05f;

	verts.push_back({ wx - CELL_SIZE / 2 + inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b });
	verts.push_back({ wx + CELL_SIZE / 2 - inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b });
	verts.push_back({ wx + CELL_SIZE / 2 - inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b });
	verts.push_back({ wx - CELL_SIZE / 2 + inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b });
	verts.push_back({ wx + CELL_SIZE / 2 - inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b });
	verts.push_back({ wx - CELL_SIZE / 2 + inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b });
}	


void TetrisAppLayer::RenderNextPiece(std::vector<Vertex>& verts, PieceType type, const std::vector<std::pair<int, int>>& cells)
{
    // type is valid (I=0 through L=6), no EMPTY variant
    int idx = static_cast<int>(type);
    float r = PIECE_COLORS[idx][0];
    float g = PIECE_COLORS[idx][1];
    float b = PIECE_COLORS[idx][2];

    // Preview box dimensions
    int boxLeft = BOARD_WIDTH + 2;
    int boxRight = BOARD_WIDTH + 5;
    int boxTop = 6;   // top row
    int boxBottom = 16;  // bottom row

    // Background for preview box (drawn first)
    for (int y = boxTop; y < boxBottom; ++y) {
        for (int x = boxLeft; x < boxRight; ++x) {
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
    for (auto& [dr, dc] : cells) 
    {
        minR = std::min(minR, dr); maxR = std::max(maxR, dr);
        minC = std::min(minC, dc); maxC = std::max(maxC, dc);
    }

    // Center position in preview box
    int centerX = (boxLeft + boxRight) / 2;
    int centerY = (boxTop + boxBottom) / 2;
    int offsetX = centerX - (minC + maxC) / 2;
    int offsetY = centerY - (minR + maxR) / 2;

    // Draw the piece
    for (auto& [dr, dc] : cells) {
        int rx = dc + offsetX;
        int ry = dr + offsetY;
        PushQuad(verts, rx, ry, r, g, b);
    }
}

void TetrisAppLayer::Render(const GameState& state, int winW, int winH)
{
    std::vector<Vertex> vertices;
    vertices.reserve(BOARD_WIDTH * BOARD_HEIGHT * VERTS_PER_QUAD * 3);

    auto addQuad = [&](int x, int y, float r, float g, float b) {
        float wx = WorldX(x);
        float wy = WorldY(y);
        float inset = 0.05f;

        vertices.push_back({ wx - CELL_SIZE / 2 + inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b });
        vertices.push_back({ wx + CELL_SIZE / 2 - inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b });
        vertices.push_back({ wx + CELL_SIZE / 2 - inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b });

        vertices.push_back({ wx - CELL_SIZE / 2 + inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b });
        vertices.push_back({ wx + CELL_SIZE / 2 - inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b });
        vertices.push_back({ wx - CELL_SIZE / 2 + inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b });
        };

    // 1) Background (dark pit)
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            addQuad(x, y, 0.06f, 0.06f, 0.1f);
        }
    }

    // 2) Grid lines (on top of background)
    for (int x = 0; x <= BOARD_WIDTH; ++x) {
        AddLineQuad(vertices, x, 0, x, BOARD_HEIGHT, 0.2f, 0.2f, 0.28f);
    }
    for (int y = 0; y <= BOARD_HEIGHT; ++y) {
        AddLineQuad(vertices, 0, y, BOARD_WIDTH, y, 0.2f, 0.2f, 0.28f);
    }

    // 3) Placed pieces from grid
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            if (state.m_Grid[y][x] > 0) {
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
    if (!state.m_GameOver) {
        int ghostRow = -1;
        for (int testRow = state.m_CurrentRow; testRow < BOARD_HEIGHT; ++testRow) {
            bool canDrop = true;
            for (auto& [dr, dc] : state.m_CurrentCells) {
                int r = testRow + dr;
                int c = m_GameState.m_CurrentCol + dc;
                if (c < 0 || c >= BOARD_WIDTH) { canDrop = false; break; }
                if (r >= BOARD_HEIGHT) { canDrop = false; break; }
                if (r >= 0 && state.m_Grid[r][c] != 0) { canDrop = false; break; }
            }
            
            if (!canDrop)
            {
                ghostRow = testRow - 1;
                break;
            }
            if (testRow == BOARD_HEIGHT - 1) ghostRow = testRow;
        }

        if (ghostRow >= 0 && ghostRow != state.m_CurrentRow)
        {
            int idx = static_cast<int>(state.m_CurrentType);
            for (auto& [dr, dc] : state.m_CurrentCells) 
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
        for (auto& [dr, dc] : state.m_CurrentCells)
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
    //glBindVertexArray(vao);
    //glBindVertexArray(m_VertexArray);
    //glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    ////glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
    //glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    //glBindVertexArray(0);
#else
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}



void TetrisAppLayer::Init() {
    //#ifdef GL3_PLUS_SUPPORT
    //        glGenVertexArrays(1, &vao);
    //        glGenBuffers(1, &vbo);
    //
    //        glBindVertexArray(vao);
    //        glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //
    //        glEnableVertexAttribArray(0);
    //        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    //        glEnableVertexAttribArray(1);
    //        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    //
    //        glBindVertexArray(0);
    //#else
    //        glGenBuffers(1, &vbo);
    //#endif
}