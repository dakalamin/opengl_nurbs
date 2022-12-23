#include "Core/Base.h"
#include "Core/Nurbs.h"

#include <iostream>
#include <string>

// The GLFW header can detect most such headers if they are included first
// and will then not include the one from your development environment.
// To make sure there will be no header conflicts, GLFW_INCLUDE_NONE can be defined
// before the GLFW header to explicitly disable inclusion of the development environment header.
// This also allows the two headers to be included in any order.
#define GLFW_INCLUDE_GLU
#include "glfw3.h"
#include "glm/glm.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


bool showPoints = true;


void GLFWErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << '\n';
}

void GLFWWindowCloseCallback(GLFWwindow* window)
{
	std::cout << "GLFW Window Close Callback Successful\n";
}


int main(int argc, char** argv)
{
	glfwSetErrorCallback(GLFWErrorCallback);
	if (glfwInit() == GLFW_FALSE) // GLFW initialization
		return -1;

	// Hinting sctrictly before window creation!
	const char* glsl_version = "#version 120";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// using OpenGL ver 4.2 [Compatible]
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

	
	NURBS nurbs(3, 5);
	
	GLUnurbs* renderer = gluNewNurbsRenderer();
	gluNurbsProperty(renderer, GLU_SAMPLING_TOLERANCE, 25.0);
	gluNurbsProperty(renderer, GLU_DISPLAY_MODE, GLU_FILL);
	
	GLfloat mat_diffuse[]   = { 0.9f, 0.9f, 0.9f, 1.f };
	GLfloat mat_specular[]  = { 1.f,  1.f,  1.f,  1.f };
	GLfloat mat_shininess[] = { 100.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	
	
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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

	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();


		ImGui::Begin("NURBS Surface Manager");
		enum Action { NONE, INSERT, DELETE } action = NONE;

		static NURBS::Dim dim  = NURBS::U;
		static size_t place[2] = { 0, 0 };

		if (ImGui::CollapsingHeader("Control Points", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const char* dims[2] = { "U", "V" };

			action = Action::NONE;

			ImGui::PushItemWidth(60);
			ImGui::SliderInt("Dim", (int*)&dim, 0, 1, dims[dim]);
			ImGui::PopItemWidth();
			ImGui::SameLine(0, 20);
			if (ImGui::BeginTabBar("ActionsTab", ImGuiTabBarFlags_FittingPolicyScroll))
			{
				if (ImGui::BeginTabItem("Insertion"))
				{
					action = Action::INSERT;

					static int automatic[2] = { 1, 1 };
					static std::vector<glm::vec3> newCP[2] =
					{
						std::vector<glm::vec3>(nurbs.dim[NURBS::reverseDim(dim)], {0.f, 0.f, 0.f}),
						std::vector<glm::vec3>(nurbs.dim[NURBS::reverseDim(dim)], {0.f, 0.f, 0.f})
					};

					ImGui::Text("The layer to insert a new line of CP to:");
					ImGui::SliderInt("# of layer", (int*)&place[dim], 0, nurbs.dim[dim]);

					ImGui::Separator(); 
					ImGui::Text("CP set modes:");
					ImGui::RadioButton("Automatic", &automatic[dim], 1); ImGui::SameLine();
					ImGui::RadioButton("Manual",    &automatic[dim], 0);

					if (automatic[dim])
						ImGui::Text("*Coords will be approximated");

					ImGui::BeginDisabled(automatic[dim]);
					for (size_t i = 0; i < nurbs.dim[dim]; i++)
						ImGui::DragFloat3((dims[dim] + std::to_string(i)).c_str(), &newCP[dim][i][0], 0.1f);
					ImGui::EndDisabled();

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Deletion"))
				{
					action = Action::DELETE;

					ImGui::Text("This is for Dim Deletion");
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
		ImGui::End();
		ImGui::Render();


		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		float ratio = display_w / (float)display_h;

		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0, ratio, 0.1, 100.0);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -10.0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glRotatef(-60.f, 1.f, 0.f, 0.f);
		glRotatef((float)glfwGetTime() * 15.f, 0.f, 0.f, 1.f);
		
		// Render the NURBS surface
		gluBeginSurface(renderer);
		gluNurbsSurface(renderer,
			nurbs.knots[NURBS::U].size(), nurbs.knots[NURBS::U].data(),
			nurbs.knots[NURBS::V].size(), nurbs.knots[NURBS::V].data(),
			3, 3 * nurbs.dim[NURBS::U],
			&nurbs.controlPoints[0][0],
			nurbs.degree[NURBS::U] + 1, nurbs.degree[NURBS::V] + 1,
			GL_MAP2_VERTEX_3);
		gluEndSurface(renderer);

		if (showPoints) {
			glPointSize(5.f);
			glDisable(GL_LIGHTING);

			glBegin(GL_POINTS);
			for (int v = 0; v < nurbs.dim[NURBS::V]; v++)
			{
				for (int u = 0; u < nurbs.dim[NURBS::U]; u++)
				{
					size_t coord = nurbs.dim[NURBS::U] * v + u;
					glm::vec3 cp = nurbs.controlPoints[coord];

					if (action == Action::INSERT && (
					   (dim == NURBS::U && (u == place[dim] || u == place[dim] - 1)) ||
					   (dim == NURBS::V && (v == place[dim] || v == place[dim] - 1))
					   ))
					{
						glColor3f(0.0, 1.0, 0.0);
					}
					else
						glColor3f(1.0, 1.0, 0.0);

					glVertex3f(cp.x, cp.y, cp.z);
				}
			}
			glEnd();
			glEnable(GL_LIGHTING);
		}
		glPopMatrix();
		glFlush();
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());		
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}