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
	ImGui::Text("Player Position: x: %.2f, y: %.2f, z: %.2f", 
		myScene.mainCamera->cameraPos[0],
		myScene.mainCamera->cameraPos[1],
		myScene.mainCamera->cameraPos[2]);
	ImGui::Text("Object Count: %d", myScene.m_SceneObjects.size());

	//Left Side of window
	static int selected = 0; //Default selected object is first object in scene

	//Current Tab from left to right
	static int currentTab = 1;

	ImGui::BeginGroup();
	if (currentTab == 1)
	{
		ImGui::BeginChild("left pane top", ImVec2(150, -ImGui::GetFrameHeightWithSpacing()), true);
		for (int i = 0; i < myScene.m_SceneObjects.size(); i++)
		{
			// FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
			char label[128];
			sprintf_s(label, myScene.m_SceneObjects[i]->m_DisplayName.c_str(), i);
			if (ImGui::Selectable(label, false))
			{
				selected = i;
			}
		}
		ImGui::EndChild();
		//Add Cube object to scene
		if (ImGui::Button("+")) {
			myScene.addObject(new Cube());
		}
	}
	else if (currentTab == 2)
	{
		ImGui::BeginChild("left pane top", ImVec2(150, -ImGui::GetFrameHeightWithSpacing()), true);
		for (int i = 0; i < myScene.m_LightController->m_SpotLights.size(); i++)
		{
			// FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
			char label[128];
			sprintf_s(label, myScene.m_LightController->m_SpotLights[i]->m_DisplayName.c_str(), i);
			if (ImGui::Selectable(label, false))
			{
				selected = i;
			}
		}
		ImGui::EndChild();
		//Add Cube object to scene
		if (ImGui::Button("+")) {
			//myScene.addObject(new Cube());
			//Add spotlight code
		}
	}
	ImGui::EndGroup();


	


	ImGui::SameLine();
	// Right side of window
	ImGui::BeginGroup();
	{
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us	
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			//Properties Tab
			if (ImGui::BeginTabItem("Properties"))
			{
				//Update the selected item (default selected item is the 0th object)
				Object* selectedObject = myScene.m_SceneObjects[selected];
				currentTab = 1;
				//Display the Current selected object name
				if (selectedObject != nullptr)
					ImGui::Text("Selected Item: %s", selectedObject->m_DisplayName.c_str());
				else
					ImGui::Text("Selected Item: %s", "NULL");

				ImGui::Spacing();
				if (selectedObject != nullptr)
				{
					// Position
					{
						//For transformation
						static float vec4f[4] = { 0.00f, 0.00f, 0.00f, 0.00f };
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
							selectedObject->startFall = glfwGetTime();
						}
					}
					
					// Scaling
					{
						//For Scale
						static float vec3f[4] = { 1.00f, 1.00f, 1.00f };
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
					}

					// Rotation
					{
						ImGui::Spacing();
						ImGui::Text("Rotation");
						float rX = selectedObject->m_Rotation[0];
						float rY = selectedObject->m_Rotation[2];
						float rZ = selectedObject->m_Rotation[1];
						glm::vec3 currentRotation = glm::vec3(rX, rY, rZ);

						if (ImGui::DragFloat("x", &rX, 1.0f, 0.0f, 360.0f, "%.2f", 0))
						{
							
							currentRotation[0] = rX;
							selectedObject->setRotation(currentRotation, glm::vec3(1.0f, 0.0f, 0.0f), 0);
						}
						if (ImGui::DragFloat("z", &rZ, 1.0f, 0.0f, 360.0f, "%.2f", 0))
						{
							currentRotation[1] = rZ;
							selectedObject->setRotation(currentRotation, glm::vec3(0.0f, 1.0f, 0.0f), 1);
						}
						if (ImGui::DragFloat("y", &rY, 1.0f, 0.0f, 360.0f, "%.2f", 0))
						{
							currentRotation[2] = rY;
							selectedObject->setRotation(currentRotation, glm::vec3(0.0f, 0.0f, 1.0f), 2);
						}

					}

					// Color
					{
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
						ImGui::Spacing();

					}

					// Texture
					{
						static std::string diffuseText = "";
						static std::string specularText = "";
						static char diffuseBuffer[256]; // This is where the text will be stored
						static char specularBuffer[256]; // This is where the text will be stored
						static bool showSuccessText = false;

						ImGui::Text("Texture Paths:");
						if (selectedObject->m_hasTexture)
						{

							//Set textbox default values
							if (diffuseText == "")
								strcpy_s(diffuseBuffer, selectedObject->m_DiffuseMap->m_Path.c_str());
							else
								strcpy_s(diffuseBuffer, diffuseText.c_str());

							if (specularText == "")
								strcpy_s(specularBuffer, selectedObject->m_SpecularMap->m_Path.c_str());
							else
								strcpy_s(specularBuffer, specularText.c_str());
						}
						else
						{
							//Set textbox default values
							if (diffuseText != "")
								strcpy_s(diffuseBuffer, diffuseText.c_str());
							else
								strcpy_s(diffuseBuffer, "");

							if (specularText != "")
								strcpy_s(specularBuffer, specularText.c_str());
							else
								strcpy_s(specularBuffer, "");
						}

						ImGui::Spacing();
						//Get changes from textboxes
						ImGui::Text("Diffuse Path:\n", "NULL");
						if (ImGui::InputText("##DiffusePath", diffuseBuffer, sizeof(diffuseBuffer)))
						{
							diffuseText = diffuseBuffer;
							showSuccessText = false;
						}

						ImGui::Spacing();
						ImGui::Text("Specular Path:\n", "NULL");
						if (ImGui::InputText("##SpecularPath", specularBuffer, sizeof(specularBuffer)))
						{
							specularText = specularBuffer;
							showSuccessText = false;
						}

						if (ImGui::Button("Set Texture##hasTextureTrue")) {
							//[0]: diffuse
							//[1]: specular					
							//[2]: ambient
							std::vector<std::string> newPaths = {
								string(diffuseBuffer, 256),
								string(specularBuffer, 256)
							};
						}	

						ImGui::SameLine(); HelpMarker(
							"If only one texture is desired, insert the\n"
							"the same file path in both input boxes.\n");

					}

					// Physics
					{
						ImGui::Spacing();
						if (ImGui::Checkbox("Allow Physics ", &selectedObject->enablePhysics))
						{
							if(selectedObject->isPhysicsObject == false)
								myScene.m_PhysicsWorld->addObject(selectedObject);
							selectedObject->setPhysics();
						}
					}
				}	
				ImGui::EndTabItem();
			}
			//Lighting Tab
			if (ImGui::BeginTabItem("Lighting"))
			{
				currentTab = 2;
				glm::vec3 diffuse = myScene.m_LightController->m_SpotLights[0]->getDiffuse();
				glm::vec3 ambient = myScene.m_LightController->m_SpotLights[0]->getAmbient();
				glm::vec3 specular = myScene.m_LightController->m_SpotLights[0]->getSpecular();
				float intensity = myScene.m_LightController->m_SpotLights[0]->getIntensity();
				// Intensity
				{
					ImGui::Spacing();
					ImGui::Text("SpotLight");
					if (ImGui::DragFloat("Intensity", &intensity, 0.5f, 1.0f, 100.0f, "%.2f", 0))
					{
						if (intensity <= 0)
							intensity = 1;
						myScene.m_LightController->m_SpotLights[0]->setIntensity(intensity);
						myScene.m_LightController->m_SpotLights[0]->setDiffuse(diffuse);
						myScene.m_LightController->m_SpotLights[0]->setDiffuse(ambient);
						myScene.m_LightController->m_SpotLights[0]->setDiffuse(specular);
					}
				}

				// Light Color
				{
					ImGui::Text("Light Color", "NULL");
					float d[3] = { diffuse[0] / intensity , diffuse[1] / intensity, diffuse[2] / intensity };
					float a[3] = { ambient[0] / intensity , ambient[1] / intensity, ambient[2] / intensity };
					float s[3] = { specular[0] / intensity , specular[1] / intensity, specular[2] / intensity };

					bool change = false;
					if (ImGui::ColorEdit3("Diffuse###Lightcolor 1", d))
					{
						myScene.m_LightController->m_SpotLights[0]->setDiffuse(glm::vec3(d[0], d[1], d[2]));
						change = true;
					}
					if (ImGui::ColorEdit3("Ambient###Lightcolor 2", a))
					{
						myScene.m_LightController->m_SpotLights[0]->setAmbient(glm::vec3(a[0], a[1], a[2]));
						change = true;
					}
					if (ImGui::ColorEdit3("Specular###Lightcolor 3", s))
					{
						myScene.m_LightController->m_SpotLights[0]->setSpecular(glm::vec3(s[0], s[1], s[2]));
						change = true;
					}

					ImGui::SameLine(); HelpMarker(
						"Click on the color square to open a color picker.\n"
						"Click and hold to use drag and drop.\n"
						"Right-click on the color square to show options.\n"
						"CTRL+click on individual component to input value.\n");
					ImGui::Spacing();
				}

				//ShadowMap
				{
					ImGui::Spacing();
					ImGui::Text("Shadows");
					float near_plane = myScene.m_ShadowMap->getNearPlane();
					if (ImGui::InputFloat("Near Plane", &near_plane, 0.1f, 1.0f, "%.3f", 0))
						myScene.m_ShadowMap->setNearPlane(near_plane);
					float far_plane = myScene.m_ShadowMap->getFarPlane();
					if (ImGui::InputFloat("Far Plane", &far_plane, 0.1f, 1.0f, "%.3f", 0))
						myScene.m_ShadowMap->setFarPlane(far_plane);
				}

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
	}		
	ImGui::EndGroup();

}






