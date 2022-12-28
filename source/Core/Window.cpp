#include "Core/Window.h"
#include <stdexcept>

bool Window::initialized = false;


Window::Window(std::string title, int width, int height)
{
	glfwSetErrorCallback(CallbackError);

	if (!initialized && !(initialized = glfwInit()))
		throw std::runtime_error("Window::Window: Failed to initialize GLFW");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// using OpenGL ver 3.2 [Compatible]
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error("Window::Window: Failed to create a window");
	}

	glfwSetWindowCloseCallback(window, CallbackClose);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	gui.init(this);
}

Window::~Window() { glfwTerminate(); }


void Window::mainloop()
{
	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		glfwPollEvents();

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		gui.mainloop(1280, 720, glfwGetTime());

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
}