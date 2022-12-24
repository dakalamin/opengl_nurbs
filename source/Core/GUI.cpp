#include "Core/GUI.h"
#include "Core/Window.h"


GUI::GUI() : nurbs(5, 3)
{
	setCP(NURBS::U);
	setCP(NURBS::V);
}

void GUI::init(Window* window)
{
	setRenderer();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	io = ImGui::GetIO(); (void)io;
	setConfigFlags();
	loadFonts();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window->window, true);
	ImGui_ImplOpenGL3_Init(window->GLSL_VERSION.c_str());
}

GUI::~GUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}


void GUI::mainloop(int width, int height, float time)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	NURBSSurfaceManager();
	ImGui::Render();
	ImGui::EndFrame();

	drawNURBS(width, height, time);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void GUI::NURBSSurfaceManager()
{
	ImGui::Begin("NURBS Surface Manager");
	action = Action::NONE;

	if (ImGui::CollapsingHeader("Control Points", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushItemWidth(60);
		if (ImGui::SliderInt("Dim", (int*)&dim, 0, 1, NURBS::dim_char[dim], ImGuiSliderFlags_NoInput))
			setCP(dim);
		ImGui::PopItemWidth();
		ImGui::SameLine(0, 20);

		if (ImGui::BeginTabBar("ActionsTab", ImGuiTabBarFlags_FittingPolicyScroll))
		{
			if (ImGui::BeginTabItem("Insertion"))
			{
				action = Action::INSERT;

				ImGui::Text("Layer to insert a new line of CP to:");
				ImGui::SliderInt("# of layer", (int*)&layer[dim], 0, nurbs.dim[dim]);

				ImGui::Separator();
				ImGui::Text("CP set modes:");
				ImGui::RadioButton("Automatic", &automatic[dim], 1); ImGui::SameLine();
				ImGui::RadioButton("Manual", &automatic[dim], 0);

				if (automatic[dim])
				{
					controlPoints[dim] = nurbs.interpolateCP(dim, layer[dim]);
					ImGui::Text("*Coords will be approximated");
				}

				ImGui::BeginDisabled(automatic[dim]);
				for (size_t i = 0; i < nurbs.dim[NURBS::reverseDim(dim)]; i++)
					ImGui::DragFloat3((NURBS::dim_char[dim] + std::to_string(i)).c_str(), &controlPoints[dim][i][0], 0.1f);
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
}

void GUI::drawNURBS(int width, int height, float time)
{
	float ratio = 1.f * width/height;

	glViewport(0, 0, width, height);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.f * width/height, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glRotatef(-60.f, 1.f, 0.f, 0.f);
	glRotatef(time*15.f, 0.f, 0.f, 1.f);

	GLUnurbs* r = (GLUnurbs*)renderer;

	// Render the NURBS surface
	gluBeginSurface(r);
	gluNurbsSurface(r,
		nurbs.knots[NURBS::U].size(), nurbs.knots[NURBS::U].data(),
		nurbs.knots[NURBS::V].size(), nurbs.knots[NURBS::V].data(),
		3, 3 * nurbs.dim[NURBS::U],
		&nurbs.controlPoints[0][0],
		nurbs.getOrder(NURBS::U), nurbs.getOrder(NURBS::V),
		GL_MAP2_VERTEX_3);
	gluEndSurface(r);

	if (showPoints) drawPoints();

	glPopMatrix();
	glFlush();
}

void GUI::drawPoints()
{
	glPointSize(5.f);
	glDisable(GL_LIGHTING);

	glBegin(GL_POINTS);
	for (size_t v = 0; v < nurbs.dim[NURBS::V]; v++)
	{
		for (size_t u = 0; u < nurbs.dim[NURBS::U]; u++)
		{
			size_t coord = nurbs.dim[NURBS::U] * v + u;
			glm::vec3 cp = nurbs.controlPoints[coord];

			if (action == Action::INSERT && (
				(dim == NURBS::U && (u == layer[dim] || u == layer[dim] - 1)) ||
				(dim == NURBS::V && (v == layer[dim] || v == layer[dim] - 1))
				))
			{
				glColor3f(1.f, 1.f, 0.f);
			}
			else
				glColor3f(0.8f, 0.8f, 0.8f);

			glVertex3f(cp.x, cp.y, cp.z);
		}
	}
	if (action == Action::INSERT)
	{
		glColor3f(0.f, 1.f, 0.f);
		for (size_t i = 0; i < nurbs.dim[NURBS::reverseDim(dim)]; i++)
		{
			glm::vec3 cp = controlPoints[dim][i];
			glVertex3f(cp.x, cp.y, cp.z);
		}
	}
	glEnd();
	glEnable(GL_LIGHTING);
}


void GUI::setRenderer()
{
	renderer = gluNewNurbsRenderer();
	GLUnurbs* r = (GLUnurbs*)renderer;

	gluNurbsProperty(r, GLU_SAMPLING_TOLERANCE, 25.0);
	gluNurbsProperty(r, GLU_DISPLAY_MODE, GLU_FILL);

	GLfloat mat_diffuse[]   = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat mat_specular[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess[] = { 100.f };

	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
}


void GUI::setConfigFlags()
{
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform Windows
	// io.ConfigViewportsNoAutoMerge   = true;
	// io.ConfigViewportsNoTaskBarIcon = true;
}

void GUI::loadFonts()
{
	io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 15.0f);
	io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/sourcesans_pro/SourceSansPro-Regular.ttf", 15.0f);
}