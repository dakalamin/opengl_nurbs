#include "Core/Base.h"

// The GLFW header can detect most such headers if they are included first
// and will then not include the one from your development environment.
// To make sure there will be no header conflicts, GLFW_INCLUDE_NONE can be defined
// before the GLFW header to explicitly disable inclusion of the development environment header.
// This also allows the two headers to be included in any order.
#define GLFW_INCLUDE_GLU
#include "glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


void GLFWErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << '\n';
}

void GLFWWindowCloseCallback(GLFWwindow* window)
{
	std::cout << "GLFW Window Close Callback Successful\n";
	glfwDestroyWindow(window);
}


int main(int argc, char** argv)
{
	glfwSetErrorCallback(GLFWErrorCallback);
	if (glfwInit() == GLFW_FALSE) // GLFW initialization
		return -1;

	// Hinting sctrictly before window creation!
	const char* glsl_version = "#version 420";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);	// using OpenGL ver 4.2 [Compatible]
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	#ifdef CW_DEBUG  // (in premake.lua) 
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	#endif
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "CourseWork App", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwSetWindowCloseCallback(window, GLFWWindowCloseCallback);
	
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync


	std::cout << "[OpenGL Info]\n";
	std::cout << "      Vendor: " << glGetString(GL_VENDOR) << '\n';
	std::cout << "    Renderer: " << glGetString(GL_RENDERER) << '\n';
	std::cout << "     Version: " << glGetString(GL_VERSION) << '\n';


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform Windows
	// io.ConfigViewportsNoAutoMerge   = true;
	// io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Load Fonts
	io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 15.0f);
	io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/sourcesans_pro/SourceSansPro-Regular.ttf", 15.0f);


	bool show_demo_window    = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
				

		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_another_window);   // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		counter += ImGui::Button("Button");                     // Buttons return true when clicked (most widgets return true when edited/activated)
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f/ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		if (show_another_window)
		{
			// Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello from another window!");
			
			show_another_window = !ImGui::Button("Close Me");
			ImGui::End();
		}

		ImGui::Render();


		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		float ratio = display_w / (float)display_h;

		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef((float)glfwGetTime() * 10.f, 0.f, 0.f, 1.f);
		
		glBegin(GL_TRIANGLES);
		glColor3f(  1.0f,  0.0f, 0.f);
		glVertex3f(-0.6f, -0.4f, 0.f);
		glColor3f(  0.0f,  1.0f, 0.f);
		glVertex3f( 0.6f, -0.4f, 0.f);
		glColor3f(  0.0f,  0.0f, 1.f);
		glVertex3f( 0.0f,  0.6f, 0.f);
		glEnd();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());		
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}