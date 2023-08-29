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

GUI::GUI(GLFWwindow* windowParam, SceneManager& scene) : window(windowParam), myScene(scene)
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
	//For transformation
	static float vec4f[4] = { 0.00f, 0.00f, 0.00f, 0.00f };


	// Create an array of labels for the ListBox
	ImGui::Text("Object Count: %d", myScene.objectCount);
	//Left Side of windo
	static int selected = 0;
	{
		ImGui::BeginChild("left pane", ImVec2(150, 0), true);
		for (int i = 0; i < myScene.sceneObjects.size(); i++)
		{
			// FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
			char label[128];
			sprintf_s(label, myScene.sceneObjects[i]->m_type.c_str(), i);
			if (ImGui::Selectable(label, selected == i))
			{
				selected = i;
			}
		}
		ImGui::EndChild();
	}
	//Update the selected item (default selected item is the 0th object)
	Object* selectedObject = myScene.sceneObjects[selected];
	ImGui::SameLine();
	// Right side of window
	{
		ImGui::BeginGroup();
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
		if(selectedObject != nullptr)
			ImGui::Text("Selected Item: %s", selectedObject->m_type.c_str());
		else
			ImGui::Text("Selected Item: %s", "NULL");

		ImGui::Separator();
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Position"))
			{
				if (selectedObject != nullptr)
				{
					glm::vec3 pos = selectedObject->m_Position;
					ImGui::Text("Position:\tx: %.2f, y: %.2f, z: %.2f", pos.x, pos.y, pos.z);
					ImGui::InputFloat3("input float3", vec4f);
					if (ImGui::Button("Set")) {
						pos[0] = vec4f[0];
						pos[1] = vec4f[1];
						pos[2] = vec4f[2];
						selectedObject->setPosition(pos);
						vec4f[0] = 0.00f;
						vec4f[1] = 0.00f;
						vec4f[2] = 0.00f;
						vec4f[4] = 0.00f;
					}
				}	
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Details"))
			{
				ImGui::Text("ID: 0123456789");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
		if (ImGui::Button("Revert")) {}
		ImGui::SameLine();
		if (ImGui::Button("Save")) {}
		ImGui::EndGroup();
	}

	/*
	// Display detailed information about the selected item (if any)
	if (selectedItemIndex >= 0 && selectedItemIndex < myScene.sceneObjects.size()) {
		auto& selectedObject = myScene.sceneObjects[selectedItemIndex];
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
	}*/

	if (ImGui::Button("+")) {
		myScene.addPrimitive(new Primitive("Sphere"));
	}

	/*	
	ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);


		// Left
		static int selected = 0;
		{
			ImGui::BeginChild("left pane", ImVec2(150, 0), true);
			for (int i = 0; i < 100; i++)
			{
				// FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
				char label[128];
				sprintf_s(label, "MyObject %d", i);
				if (ImGui::Selectable(label, selected == i))
					selected = i;
			}
			ImGui::EndChild();
		}
		ImGui::SameLine();

		// Right
		{
			ImGui::BeginGroup();
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::Text("MyObject: %d", selected);
			ImGui::Separator();
			if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Description"))
				{
					ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Details"))
				{
					ImGui::Text("ID: 0123456789");
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::EndChild();
			if (ImGui::Button("Revert")) {}
			ImGui::SameLine();
			if (ImGui::Button("Save")) {}
			ImGui::EndGroup();
		}*/
	
}






