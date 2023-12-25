#include "GUI.h"

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}


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
		ImGui::Text("%.3f fps", myScene.fps);
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

	ImGui::Text("Player Position: x: %.2f, y: %.2f, z: %.2f", 
		myScene.mainCamera->cameraPos[0],
		myScene.mainCamera->cameraPos[1],
		myScene.mainCamera->cameraPos[2]);
	ImGui::Text("Object Count: %d", myScene.m_SceneObjects.size());
	//Add Cube object to scene
	if (ImGui::Button("+")) {
		myScene.addObject(new Cube());
	}
	//Left Side of window
	static int selected = 0; //Default selected object is first object in scene
	{
		ImGui::BeginChild("left pane", ImVec2(150, 0), true);
		for (int i = 0; i < myScene.m_SceneObjects.size(); i++)
		{
			// FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
			char label[128];
			sprintf_s(label, myScene.m_SceneObjects[i]->m_ObjectID.c_str(), i);
			if (ImGui::Selectable(label, false))
			{
				selected = i;
			}
		}
		ImGui::EndChild();
	}


	//******************************************
	// EDIT OBJECT PROPERTIES
	// *****************************************
	
	//Update the selected item (default selected item is the 0th object)
	Object* selectedObject = myScene.m_SceneObjects[selected];
	ImGui::SameLine();
	// Right side of window
	{
		ImGui::BeginGroup();
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
		
		if(selectedObject != nullptr)
			ImGui::Text("Selected Item: %s", selectedObject->m_ObjectID.c_str());
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
					// Position
					glm::vec3 pos = selectedObject->m_Position;
					ImGui::Text("Current Position:\tx: %.2f, y: %.2f, z: %.2f", pos.x, pos.y, pos.z);
					ImGui::InputFloat3("##Position", vec4f);
					if (ImGui::Button("Set##SetPosition")) {
						pos[0] = vec4f[0];
						pos[1] = vec4f[1];
						pos[2] = vec4f[2];
						selectedObject->setPosition(pos);
						vec4f[0] = 0.00f;
						vec4f[1] = 0.00f;
						vec4f[2] = 0.00f;
						selectedObject->setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
						selectedObject->startFall = myScene.worldTime;
					}
					
					// Scaling
					glm::vec3 currentScale = selectedObject->m_Scale;
					ImGui::Text("Current Scale:\tx: %.2f, y: %.2f, z: %.2f", 
						currentScale.x, currentScale.y, currentScale.z);
					ImGui::InputFloat3("##Scale", vec3f);
					if (ImGui::Button("Set##SetScale")) {
						currentScale[0] = vec3f[0];
						currentScale[1] = vec3f[1];
						currentScale[2] = vec3f[2];
						selectedObject->setScale(currentScale);
						vec3f[0] = 1.00f;
						vec3f[1] = 1.00f;
						vec3f[2] = 1.00f;
					}

					// Color
					static float col1[3] = { 1.0f, 0.0f, 0.2f };
					ImGui::Text("Object Color", "NULL");
					if (ImGui::ColorEdit3("###color 1", col1))
					{
						selectedObject->m_Diffuse = glm::vec3(col1[0], col1[1], col1[2]);

					}
					ImGui::SameLine(); HelpMarker(
						"Click on the color square to open a color picker.\n"
						"Click and hold to use drag and drop.\n"
						"Right-click on the color square to show options.\n"
						"CTRL+click on individual component to input value.\n");

					// Physics
					if (ImGui::Checkbox("Allow Physics", &selectedObject->enablePhysics))
					{
						selectedObject->setPhysics();
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
							//[0]: diffuse
							//[1]: specular					
							//[2]: ambient
							std::vector<std::string> newPaths = {
								text,
								text
							};
							selectedObject->updateTexture(newPaths);
						}
					}
					else
					{
						strcpy_s(buffer, "");
						if (ImGui::InputText("Texture Path: ", buffer, sizeof(buffer)))
							text = buffer;
						if (ImGui::Button("Set Texture")) {
							//[0]: diffuse
							//[1]: specular					
							//[2]: ambient
							std::vector<std::string> newPaths = {
								text,
								text
							};
							selectedObject->updateTexture(newPaths);
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






