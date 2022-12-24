#pragma once

#include <iostream>
#include <string>

// The GLFW header can detect most such headers if they are included first
// and will then not include the one from your development environment.
// To make sure there will be no header conflicts, GLFW_INCLUDE_NONE can be defined
// before the GLFW header to explicitly disable inclusion of the development environment header.
// This also allows the two headers to be included in any order.
#define GLFW_INCLUDE_GLU
#include "glfw3.h"
#include "Core/GUI.h"


class Window
{
private:
	GUI gui;
	static bool initialized;

public:
	GLFWwindow* window;

	static const int DEFAULT_WIDTH = 1280;
	static const int DEFAULT_HEIGHT = 720;
	const std::string GLSL_VERSION = "#version 130";

private:
	inline static void CallbackError(int error, const char* description)
	{ std::cerr << "GLFW Error " << error << ": " << description << '\n'; }

	inline static void CallbackClose(GLFWwindow* window)
	{ std::cout << "GLFW Window Close Callback Successful\n"; }

public:
	Window(std::string title, int width=DEFAULT_WIDTH, int height=DEFAULT_HEIGHT);
	~Window();

	void mainloop();
};