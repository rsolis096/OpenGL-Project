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

GUI::GUI(GLFWwindow* windowParam, Scene& scene) : window(windowParam), myScene(scene)
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

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void GUI::drawList()
{
	//For transformation
	static float vec4f[4] = { 0.00f, 0.00f, 0.00f, 0.00f };
	//For Scale
	static float vec3f[4] = { 1.00f, 1.00f, 1.00f};

	ImGui::Text("Object Count: %d", myScene.sceneObjects.size());
	//Add Cube object to scene
	if (ImGui::Button("+")) {
		myScene.addObject(new Cube());
		myScene.sceneObjects[myScene.objectCount - 1]->m_Type = "Cube" + to_string(myScene.objectCount);
	}
	//Left Side of window
	static int selected = 0; //Default selected object is first object in scene
	{
		ImGui::BeginChild("left pane", ImVec2(150, 0), true);
		for (int i = 0; i < myScene.sceneObjects.size(); i++)
		{
			// FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
			char label[128];
			sprintf_s(label, myScene.sceneObjects[i]->m_Type.c_str(), i);
			if (ImGui::Selectable(label, false))
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
			ImGui::Text("Selected Item: %s", selectedObject->m_Type.c_str());
		else
			ImGui::Text("Selected Item: %s", "NULL");

		ImGui::Separator();
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			//Properties Tab
			if (ImGui::BeginTabItem("Properties"))
			{
				if (selectedObject != nullptr)
				{
					//Position
					glm::vec3 pos = selectedObject->m_Position;
					ImGui::Text("Position:\tx: %.2f, y: %.2f, z: %.2f", pos.x, pos.y, pos.z);
					ImGui::InputFloat3("Position", vec4f);
					if (ImGui::Button("Set")) {
						pos[0] = vec4f[0];
						pos[1] = vec4f[1];
						pos[2] = vec4f[2];
						selectedObject->setPosition(pos);
						vec4f[0] = 0.00f;
						vec4f[1] = 0.00f;
						vec4f[2] = 0.00f;
					}
					
					//Scaling
					glm::vec3 sc(1.0f, 1.0f, 1.0f);
					ImGui::Text("Scale:\tx: %.2f, y: %.2f, z: %.2f", sc.x, sc.y, sc.z);
					ImGui::InputFloat3("Scale", vec3f);
					if (ImGui::Button("Set1")) {
						sc[0] = vec3f[0];
						sc[1] = vec3f[1];
						sc[2] = vec3f[2];
						selectedObject->setScale(sc);
						vec3f[0] = 1.00f;
						vec3f[1] = 1.00f;
						vec3f[1] = 1.00f;
					}
				}	
				ImGui::EndTabItem();
			}
			//Texture Tab
			static std::string text = " ";
			if (ImGui::BeginTabItem("Texture"))
			{
				char buffer[256]; // This is where the text will be stored
				ImGui::Text("Texture Path");

				if (selectedObject != nullptr)
				{
					if (selectedObject->m_DiffuseMap != nullptr)
					{
						if(text == " ")
							strcpy_s(buffer, selectedObject->m_DiffuseMap->m_Path.c_str());
						else
							strcpy_s(buffer, text.c_str());

						if (ImGui::InputText("Texture Path: ", buffer, sizeof(buffer)))
							text = buffer;
						if (ImGui::Button("Set Texture")) {

							selectedObject->m_hasTexture = true;
							selectedObject->m_DiffuseMap->updateTexture(text.c_str(), false);
							//This should be changed to allow for a new specular map
							selectedObject->m_SpecularMap->updateTexture(text.c_str(), false);

						}
					}


				}
			
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
		ImGui::EndGroup();
	}
}






