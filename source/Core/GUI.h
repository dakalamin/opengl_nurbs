#pragma once

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include <vector>
#include "glm/glm.hpp"

#include "Core/Nurbs.h"


class Window;

class GUI
{
private:
	const float FONT_SIZE = 15.f;

	ImGuiIO io;
	ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
private:
	enum Action { NONE, INSERT, DELETE } action = NONE;

	NURBS nurbs;
	NURBS::Dim dim = NURBS::U;
	std::vector<glm::vec3> controlPoints[2];

	void* renderer = nullptr;

	bool showPoints = true;
	size_t layer[2]  = { 0, 0 };
	int automatic[2] = { 1, 1 };

public:
	GUI();
	~GUI();

	void init(Window* window);

	void mainloop(int width, int height, float time);

	
private:
	void NURBSSurfaceManager();
	void drawNURBS(int width, int height, float time);
	void drawPoints();

	inline void setCP(NURBS::Dim d)
	{ controlPoints[d] = std::vector<glm::vec3>(nurbs.dim[NURBS::reverseDim(d)], {0.f, 0.f, 0.f}); }

	void setRenderer();

	void setConfigFlags();
	void loadFonts();
};