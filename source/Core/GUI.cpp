#include "Core/GUI.h"
#include "Core/Window.h"


void GUI::init(Window* window)
{
	setRenderer();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	io = ImGui::GetIO(); (void)io;
	setConfigFlags(); loadFonts();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window->window, true);
	ImGui_ImplOpenGL3_Init(window->GLSL_VERSION.c_str());

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = style.ScrollbarRounding = style.TabRounding = 0.f;
}

GUI::~GUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}


void GUI::setCP()
{
	setCP(NURBS::U);
	setCP(NURBS::V);
}


void GUI::mainloop(int width, int height, float time)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	NURBSSurfaceManager();
	ImGui::Render();

	ImGui::EndFrame();

	drawNURBS(width, height, time);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void GUI::NURBSSurfaceManager()
{
	ImGui::SetNextWindowSize( {256, 680} );
	ImGui::SetNextWindowPos( {1024, 20} );
	ImGui::Begin("NURBS Surface Manager", nullptr,
		ImGuiWindowFlags_HorizontalScrollbar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoMove
	);

	if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Degrees:"); ImGui::Spacing();
		drawDegreeSlider(NURBS::U);
		drawDegreeSlider(NURBS::V);
		ImGui::Separator(); ImGui::Spacing();

		ImGui::Text("Knots:"); ImGui::Spacing();
		drawKnotsClamp(NURBS::U);
		drawKnotsClamp(NURBS::V);
		ImGui::Spacing();

		if (ImGui::TreeNode("Show values (not editable)"))
		{
			drawKnotsList(NURBS::U);
			drawKnotsList(NURBS::V);
			ImGui::TreePop();
		}
		ImGui::Spacing();
	}

	ImGui::Spacing(); ImGui::Separator();
	ImGui::Text("CONTROL POINTS"); ImGui::Spacing();
	if (ImGui::CollapsingHeader("Coordinates"))
	{
		ImGui::Spacing();
		drawCoordsTop();

		for (size_t i = 0; i < nurbs.controlPoints.size(); i++)
		{
			glm::vec3 &cp = nurbs.controlPoints[i];

			ImGui::DragFloat3(concat(i, dim).c_str(), & cp[0], dragV);
			if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
				cpFocused = &cp;
		}
		ImGui::Spacing();
	}
	action = Action::NONE;
	if (ImGui::CollapsingHeader("Actions"))
	{
		ImGui::Spacing(); ImGui::PushItemWidth(60);
		ImGui::SliderInt("Dim", (int*)&dim, 0, 1, NURBS::dim_char[dim], ImGuiSliderFlags_NoInput);
		ImGui::PopItemWidth();
		ImGui::SameLine(0, 20);

		if (ImGui::BeginTabBar("ActionsTab", ImGuiTabBarFlags_FittingPolicyScroll))
		{
			if (ImGui::BeginTabItem("Insertion"))
			{
				ImGui::Spacing();

				bool dimIsMax = nurbs.dim[dim] >= NURBS::MAX_DIM;
				if (dimIsMax)
					ImGui::Text("Max %s dimension reached!", NURBS::dim_char[dim]);
				else
					action = Action::INSERT;

				ImGui::BeginDisabled(dimIsMax);
				ImGui::Text("Layer to insert a new line to CPs:");
				ImGui::SliderInt("# of layer", (int*)&layer[0][dim], 0, nurbs.dim[dim]);

				ImGui::Separator(); ImGui::Spacing();
				ImGui::Text("CP set modes:");

				ImGui::RadioButton("Automatic", &automatic[dim], 1); ImGui::SameLine();
				ImGui::RadioButton("Manual",    &automatic[dim], 0);

				if (automatic[dim])
				{
					ImGui::Text("*Coords are approximated");
					controlPoints[dim] = nurbs.interpolateCP(dim, layer[0][dim]);
				}
				ImGui::EndDisabled(); /* dimIsMax */

				ImGui::Spacing();

				ImGui::BeginDisabled(automatic[dim] || dimIsMax);
				drawCoordsTop();
				for (size_t i = 0; i < controlPoints[dim].size(); i++)
					ImGui::DragFloat3(concat(i, dim).c_str(), &controlPoints[dim][i][0], dragV);
				ImGui::EndDisabled(); /* automatic[dim] || dimIsMax */

				ImGui::BeginDisabled(dimIsMax);
				ImGui::Spacing();
				if (ImGui::Button("Insert", ImVec2(165, 0)))
				{
					nurbs.insertDim(dim, layer[0][dim], controlPoints[dim]);
					if (layer[0][dim]) layer[0][dim]++;
				}
				ImGui::EndDisabled(); /* dimIsMax */
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Deletion"))
			{
				ImGui::Spacing();

				bool dimIsMin = nurbs.dim[dim] <= NURBS::MIN_DIM;
				if (dimIsMin)
					ImGui::Text("Min %s dimension reached!", NURBS::dim_char[dim]);
				else
					action = Action::DELETE;

				ImGui::BeginDisabled(dimIsMin);
				ImGui::Text("Layer to delete a line from CPs:");
				ImGui::SliderInt("# of layer", (int*)&layer[1][dim], 0, nurbs.dim[dim]-1);

				ImGui::Spacing();
				if (ImGui::Button("Delete", ImVec2(165, 0)))
				{
					nurbs.removeDim(dim, layer[1][dim]);
					if (layer[1][dim]) layer[1][dim]--;
				}
				ImGui::EndDisabled(); /* dimIsMin */
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}


	ImGui::Spacing(); ImGui::Separator();
	ImGui::Text("MISCELLANEOUS"); ImGui::Spacing();
	if (ImGui::CollapsingHeader("Appearance"))
	{
		ImGui::Text("Background Color");
		ImGui::ColorEdit3("##BG Color", &Color::BACKGROUND[0]);

		ImGui::Spacing(); ImGui::Text("Point Colors");
		ImGui::ColorEdit3("Common", &Color::POINT_COMMON[0]);
		ImGui::ColorEdit3("Accent", &Color::POINT_ACCENT[0]);
		ImGui::ColorEdit3("Nearby", &Color::POINT_NEARBY[0]);
		ImGui::ColorEdit3("Insert", &Color::POINT_INSERT[0]);
		ImGui::ColorEdit3("Delete", &Color::POINT_DELETE[0]);

		ImGui::Spacing();
		ImGui::Checkbox("Show Points?", &showPoints);
	}

	ImGui::End();
}

void GUI::drawNURBS(int width, int height, float time)
{
	float ratio = 1.f * width/height;

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glViewport(0, 0, width, height);
	Color::set4(glClearColor, Color::BACKGROUND);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.f * width/height, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.4, 0.0, -10.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glRotatef(-60.f, 1.f, 0.f, 0.f);
	glRotatef(time*6.f, 0.f, 0.f, 1.f);

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

void GUI::drawPoint(glm::vec3 cp) { glVertex3f(cp.x, cp.y, cp.z); }

void GUI::drawPoints()
{
	glPointSize(5.f);

	glDisable(GL_LIGHTING);
	glBegin(GL_POINTS);

	if (action == Action::INSERT)
	{
		Color::set4(glColor4f, Color::POINT_INSERT);
		for (size_t i = 0; i < nurbs.dim[NURBS::reverseDim(dim)]; i++)
			drawPoint(controlPoints[dim][i]);
	}

	for (size_t i = 0; i < nurbs.controlPoints.size(); i++)
	{
		glm::vec4 color = Color::POINT_COMMON;

		size_t c = nurbs.index2uv(i, dim);
		switch (action)
		{
		case Action::INSERT:
			if (c == layer[0][dim] || c == layer[0][dim] - 1)
				color = Color::POINT_NEARBY;
			break;
		case Action::DELETE:
			if (c == layer[1][dim])
				color = Color::POINT_DELETE;
			break;
		}
		Color::set4(glColor4f, color);
		drawPoint(nurbs.controlPoints[i]);
	}
	glEnd();

	glPointSize(8.f);
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_POINTS);
	if (cpFocused != nullptr)
	{
		Color::set4(glColor4f, Color::POINT_ACCENT);
		drawPoint(*cpFocused);

		cpFocused = nullptr;
	}
	glEnd();
}

void GUI::drawDegreeSlider(NURBS::Dim d)
{
	if (ImGui::SliderInt(NURBS::dim_char[d], (int*)&nurbs.degree[d], nurbs.MIN_DEGREE, nurbs.getMaxDegree(d)))
		nurbs.setDegree(d, nurbs.degree[d]);
}

void GUI::drawKnotsClamp(NURBS::Dim d)
{
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Clamp %s at:",  NURBS::dim_char[d]); ImGui::SameLine(75);

	bool toggled = false;
	if (ImGui::Checkbox(concat("##Clamp start", d).c_str(), &nurbs.clampKnots[d][NURBS::START]))
		toggled = true;
	ImGui::SameLine(); ImGui::Text("Start"); ImGui::SameLine();

	if (ImGui::Checkbox(concat("##Clamp end", d).c_str(), &nurbs.clampKnots[d][NURBS::END]))
		toggled = true;
	ImGui::SameLine(); ImGui::Text("End");

	if (toggled) nurbs.setKnots(d);
}

void GUI::drawKnotsList(NURBS::Dim d)
{
	float indent = -14.f;

	ImGui::Indent(indent);
	ImGui::AlignTextToFramePadding();
	ImGui::Text(NURBS::dim_char[d]); ImGui::SameLine(30);

	size_t knots = nurbs.knots[d].size();
	if (ImGui::BeginTable(concat("##Knots", d).c_str(), knots,
		ImGuiTableFlags_ScrollX, { 0.0f, FONT_SIZE * 2.5f } ))
	{
		for (size_t i = 0; i < knots; i++)
		{
			ImGui::TableNextColumn();
			ImGui::Text("%.2f", nurbs.knots[d][i]);
		}
		ImGui::EndTable();
	}
	ImGui::Unindent(indent);
}

void GUI::drawCoordsTop()
{
	float spacing = 50.f;

	ImGui::Indent(20);
	ImGui::Text("X"); ImGui::SameLine(0, spacing);
	ImGui::Text("Y"); ImGui::SameLine(0, spacing);
	ImGui::Text("Z");
	ImGui::Unindent();
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
	io.Fonts->
		AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", FONT_SIZE);
	io.FontDefault = io.Fonts->
		AddFontFromFileTTF("assets/fonts/sourcesans_pro/SourceSansPro-Regular.ttf", FONT_SIZE);
}