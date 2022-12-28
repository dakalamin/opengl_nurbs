#pragma once

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include <string>
#include <vector>
#include "glm/glm.hpp"

#include "Core/Nurbs.h"


class Window;

namespace Color
{
	static glm::vec4 BACKGROUND   = { 0.45f, 0.55f, 0.60f, 1.0f };

	static glm::vec4 POINT_COMMON = { 0.7f,  0.7f,  0.7f,  1.0f };
	static glm::vec4 POINT_ACCENT = { 1.0f,  1.0f,  1.0f,  1.0f };
	static glm::vec4 POINT_NEARBY = { 0.8f,  0.8f,  0.0f,  1.0f };
	static glm::vec4 POINT_INSERT = { 0.0f,  0.8f,  0.0f,  1.0f };
	static glm::vec4 POINT_DELETE = { 0.8f,  0.0f,  0.0f,  1.0f };

	inline void set3(void(*function)(float, float, float), glm::vec3 color)
	{ function(color.x, color.y, color.z); }
	inline void set4(void(*function)(float, float, float, float), glm::vec4 color)
	{ function(color.x, color.y, color.z, color.w); }
}

class GUI
{
private:
	const float FONT_SIZE = 15.f;
	const float dragV  = 0.05f;

	ImGuiIO io;
private:
	enum Action { NONE, INSERT, DELETE } action = NONE;

	NURBS nurbs;
	NURBS::Dim dim = NURBS::U;
	glm::vec3* cpFocused = nullptr;
	std::vector<glm::vec3> controlPoints[2];

	void* renderer = nullptr;

	bool showPoints  = true;
	size_t layer[2][2] = { {0, 0}, {0, 0} };
	int automatic[2]   = { 1, 1 };

public:
	inline GUI() : nurbs(5, 3) { setCP(); };
	~GUI();

	void init(Window* window);
	void mainloop(int width, int height, float time);
	
private:
	void NURBSSurfaceManager();
	void drawNURBS(int width, int height, float time);
	void drawPoint(glm::vec3 cp);
	void drawPoints();

	void drawDegreeSlider(NURBS::Dim d);
	void drawKnotsClamp(NURBS::Dim d);
	void drawKnotsList(NURBS::Dim d);
	void drawCoordsTop();

	void setCP();
	inline void setCP(NURBS::Dim d)
	{ controlPoints[d] = std::vector<glm::vec3>(nurbs.dim[NURBS::reverseDim(d)], {0.f, 0.f, 0.f}); }

	void setRenderer();

	void setConfigFlags();
	void loadFonts();

	inline std::string concat(std::string line, NURBS::Dim d)
	{ return line + NURBS::dim_char[d]; }
	inline std::string concat(size_t i, NURBS::Dim d)
	{ return NURBS::dim_char[d] + std::to_string(i); }
};