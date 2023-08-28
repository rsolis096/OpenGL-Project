#include "GUI.h"

bool GUI::isWindowHidden = false;

int selectedItemIndex = -1;
static float vec3a[3] = { 0.00f, 0.00f, 0.00f };

/*
GUI::GUI() : window(nullptr)
{
	GUI::isWindowHidden = true;
}
*/

GUI::GUI(GLFWwindow* windowParam, std::vector<Object*>& objects) : window(windowParam), m_scenePrimitives(objects)
{
	// Setup ImGUI context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); //Input/Output allowing user and gui interactions

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	GUI::isWindowHidden = true;

	
}

void GUI::displayWindow()
{
	if (!isWindowHidden)
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Rendering
		// (Your code clears your framebuffer, renders your other stuff etc.)
		ImGui::Begin("OpenGL Project");
		ImGui::Text("Hello World");
		drawList();

		//ImGui::ColorEdit3("color", &colorSlider[0]);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void GUI::drawList()
{
	// Create an array of labels for the ListBox
	std::vector<const char*> itemLabels;
	for (auto& obj : m_scenePrimitives) {
		itemLabels.push_back(obj->m_type.c_str());
	}

	// Display the ListBox
	ImGui::ListBox("Objects\n", &selectedItemIndex, itemLabels.data(), static_cast<int>(itemLabels.size()));

	// Display detailed information about the selected item (if any)
	if (selectedItemIndex >= 0 && selectedItemIndex < m_scenePrimitives.size()) {
		auto& selectedObject = m_scenePrimitives[selectedItemIndex];
		ImGui::Text("Name: %s", selectedObject->m_type.c_str());
		glm::vec3 pos = selectedObject->m_Position;
		ImGui::Text("Position:\tx: %.2f, y: %.2f, z: %.2f", pos.x, pos.y, pos.z);
		// Show a button to set values

		static float f0 = 0.001f;
		//ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");
		static float vec4f[4] = { 0.00f, 0.00f, 0.00f, 0.00f };
		ImGui::InputFloat3("input float3", vec4f);

		// Show a button to set values
		if (ImGui::Button("Set")) {
			pos[0] = vec4f[0];
			pos[1] = vec4f[1];
			pos[2] = vec4f[2];
			selectedObject->setPosition(pos);
		}
	}
	if (ImGui::Button("+")) {
		m_scenePrimitives.push_back(new Sphere(m_scenePrimitives[0]->m_Shader));
	}
}

void GUI::setScenePrimitives(std::vector<Object*>& objects)
{
	m_scenePrimitives = objects;
}





