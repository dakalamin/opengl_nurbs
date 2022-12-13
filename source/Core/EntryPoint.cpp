#include "Core/Base.h"

#include "glad.h"
#include "glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


void GLFWErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << '\n';
}

int main(int argc, char** argv)
{
	glfwSetErrorCallback(GLFWErrorCallback);
	if (!glfwInit())
		return 1;

	const char* glsl_version = "#version 420";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);	// using OpenGL ver 4.2[Core]
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#if defined(CW_PLATFORM_IOS) || defined(CW_PLATFORM_MACOS)  // (in Base/PlatformDetector.h)
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	#ifdef CW_DEBUG  // (in premake5.lua) 
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	#endif

	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "CourseWork App", NULL, NULL);
	if (window == NULL)
		return 1;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync


	if (!gladLoadGL())
		return 1;

	std::cout << "[OpenGL Info]\n";
	std::cout << "      Vendor: " << glGetString(GL_VENDOR)   << '\n';
	std::cout << "    Renderer: " << glGetString(GL_RENDERER) << '\n';
	std::cout << "     Version: " << glGetString(GL_VERSION)  << '\n';


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	// io.ConfigViewportsNoAutoMerge   = true;
	// io.ConfigViewportsNoTaskBarIcon = true;

	io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 15.0f);
	io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/sourcesans_pro/SourceSansPro-Regular.ttf", 15.0f);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark(); //ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	auto& colors = ImGui::GetStyle().Colors;
	{
		colors[ImGuiCol_WindowBg]       = ImVec4{ 0.1f,  0.1f,  0.1f,  1.0f };

		// Headers
		colors[ImGuiCol_Header]         = ImVec4{ 0.2f,  0.2f,  0.2f,  1.0f };
		colors[ImGuiCol_HeaderHovered]  = ImVec4{ 0.3f,  0.3f,  0.3f,  1.0f };
		colors[ImGuiCol_HeaderActive]   = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button]         = ImVec4{ 0.2f,  0.2f,  0.2f,  1.0f };
		colors[ImGuiCol_ButtonHovered]  = ImVec4{ 0.3f,  0.3f,  0.3f,  1.0f };
		colors[ImGuiCol_ButtonActive]   = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg]        = ImVec4{ 0.2f,  0.2f,  0.2f,  1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f,  0.3f,  0.3f,  1.0f };
		colors[ImGuiCol_FrameBgActive]  = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab]                = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
		colors[ImGuiCol_TabHovered]         = ImVec4{ 0.38f, 0.38f, 0.38f, 1.0f };
		colors[ImGuiCol_TabActive]          = ImVec4{ 0.28f, 0.28f, 0.28f, 1.0f };
		colors[ImGuiCol_TabUnfocused]       = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f,  0.2f,  0.2f,  1.0f };

		// Title
		colors[ImGuiCol_TitleBg]            = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
		colors[ImGuiCol_TitleBgActive]      = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed]   = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
	}

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);


	bool show_demo_window    = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);

	bool opt_padding = false;
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		
		static bool* p_open = (bool*)false;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking; // | ImGuiWindowFlags_MenuBar;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("MainDockingSpace", p_open, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		/*
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control
				ImGui::MenuItem("Padding", NULL, &opt_padding);
				ImGui::Separator();

				if (ImGui::MenuItem("Flag: NoSplit", "",
					(dockspace_flags & ImGuiDockNodeFlags_NoSplit)  != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
				if (ImGui::MenuItem("Flag: NoResize", "",
					(dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
				if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "",
					(dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
				if (ImGui::MenuItem("Flag: AutoHideTabBar", "",
					(dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar)         != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
				if (ImGui::MenuItem("Flag: PassthruCentralNode", "",
					(dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)    != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
				ImGui::Separator();

				if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
					*p_open = false;
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		*/
		ImGui::End();


		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("New"))
				{
					if (ImGui::MenuItem("Project", "CTRL+SHIFT+N")) {}
					if (ImGui::MenuItem("File",    "CTRL+N"))       {}
					if (ImGui::MenuItem("Repository"))  {}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Open"))
				{
					if (ImGui::MenuItem("Project", "CTRL+SHIFT+O")) {}
					if (ImGui::MenuItem("File",    "CTRL+O"))       {}
					if (ImGui::MenuItem("Folder"))   {}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Recent")) {}
				if (ImGui::MenuItem("Start Window"))    {}
				ImGui::Separator();
				if (ImGui::MenuItem("Save",    "CTRL+S"))       {}
				if (ImGui::MenuItem("Save As", "CTRL+SHIFT+S")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", "ALT+F4")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Cut",       "CTRL+X")) {}
				if (ImGui::MenuItem("Copy",      "CTRL+C")) {}
				if (ImGui::MenuItem("Paste",     "CTRL+V")) {}
				if (ImGui::MenuItem("Duplicate", "CTRL+D")) {}
				if (ImGui::MenuItem("Delete",    "DEL"))    {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}


		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();


	return 0;
}