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
	static int propertiesSelected; //Default selected object is first object in scene
	static int lightingSelected; //Default selected object is first object in scene
	static int skyBoxSelected; //Default selected object is first object in scene

	//Current Tab from left to right
	static int currentTab = 1;

	ImGui::BeginGroup();
	if (currentTab == 1)
	{
		ImGui::BeginChild("left pane top", ImVec2(150, -ImGui::GetFrameHeightWithSpacing()), true);
		for (int i = 0; i < myScene.m_SceneObjects.size(); i++)
		{
			char label[128];
			bool isSelected = (propertiesSelected == i);

			sprintf_s(label, myScene.m_SceneObjects[i]->m_DisplayName.c_str(), i);
			if (ImGui::Selectable(label, isSelected))
			{
				propertiesSelected = i;
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
			char label[128];
			bool isSelected = (lightingSelected == i);

			sprintf_s(label, myScene.m_LightController->m_SpotLights[i]->m_DisplayName.c_str(), i);
			if (ImGui::Selectable(label, isSelected))
			{
				lightingSelected = i;
			}
		}
		ImGui::EndChild();
		//Add Cube object to scene
		if (ImGui::Button("+")) {

			if (myScene.m_LightController->m_SpotLights.size() < 8)
			{
				glm::vec3 first = glm::vec3(10.0f, 3.0f, 0.0f);
				glm::vec3 second = glm::vec3(-4.50f, -3.0f, 14.0f);
				second = glm::normalize(first - second);
				myScene.m_LightController->addSpotLight(first, second);
			}
			else
				std::cout << "Max SpotLight Limit" << std::endl;
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
				Object* selectedObject = myScene.m_SceneObjects[propertiesSelected];
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
						glm::vec3 pos = selectedObject->m_Position;
						float vec4f[3] = { pos[0], pos[1], pos[2] };
						ImGui::Text("Current Object Position:\tx: %.2f, y: %.2f, z: %.2f", pos.x, pos.y, pos.z);
						if (ImGui::InputFloat3("##Position", vec4f))
						 {			
							selectedObject->setPosition(glm::vec3(vec4f[0], vec4f[1], vec4f[2]));
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

							std::vector<std::string> newPaths = {
								string(diffuseBuffer, 256),
								string(specularBuffer, 256)
							};
							cout << newPaths[0] << endl;
							cout << newPaths[1] << endl;
							selectedObject->updateTexture(newPaths);
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
				SpotLight* selectedSpotLight = myScene.m_LightController->m_SpotLights[lightingSelected];
				currentTab = 2;

				glm::vec3 diffuse = selectedSpotLight->getDiffuse();
				glm::vec3 ambient = selectedSpotLight->getAmbient();
				glm::vec3 specular = selectedSpotLight->getSpecular();
				float intensity = selectedSpotLight->getIntensity();

				// Intensity
				{
					ImGui::Spacing();
					ImGui::Text("SpotLight");
					if (ImGui::DragFloat("Intensity", &intensity, 0.5f, 1.0f, 100.0f, "%.2f", 0))
					{
						if (intensity <= 0)
							intensity = 1;
						selectedSpotLight->setIntensity(intensity);
						selectedSpotLight->setDiffuse(diffuse);
						selectedSpotLight->setDiffuse(ambient);
						selectedSpotLight->setDiffuse(specular);
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
						selectedSpotLight->setDiffuse(glm::vec3(d[0], d[1], d[2]));
						change = true;
					}
					if (ImGui::ColorEdit3("Ambient###Lightcolor 2", a))
					{
						selectedSpotLight->setAmbient(glm::vec3(a[0], a[1], a[2]));
						change = true;
					}
					if (ImGui::ColorEdit3("Specular###Lightcolor 3", s))
					{
						selectedSpotLight->setSpecular(glm::vec3(s[0], s[1], s[2]));
						change = true;
					}

					ImGui::SameLine(); HelpMarker(
						"Click on the color square to open a color picker.\n"
						"Click and hold to use drag and drop.\n"
						"Right-click on the color square to show options.\n"
						"CTRL+click on individual component to input value.\n");
					ImGui::Spacing();
				}

				// Light Position
				{
					ImGui::Spacing();
					glm::vec3 lightPosition = selectedSpotLight->getLightPos();
					float pos[3] = { lightPosition[0], lightPosition[1], lightPosition[2] };
					ImGui::Text("Position");
					if (ImGui::InputFloat3("##Position", pos, "%.3f", 0))
					{
						selectedSpotLight->setLightPos(glm::vec3(pos[0], pos[1], pos[2]));
					}
				}
				
				// Light Direction (Needs Work)
				{
					ImGui::Spacing();
					glm::vec3 lightDir = selectedSpotLight->getLightDirection();
					//Direction
					static float dir[3] = {lightDir[0], lightDir[1], lightDir[2]};
					ImGui::Text("Direction");
					if(ImGui::DragFloat("##Direction1", &dir[0], 0.1f, -100.0f, 100.0f, "%.2f",0))
					{
						selectedSpotLight->setLightDirection(
							glm::vec3(
								dir[0],
								dir[1],
								dir[2])
						);
					}

					if (ImGui::DragFloat("##Direction2", &dir[1], 0.1f, -100.0f, 100.0f, "%.2f", 0))
					{
						selectedSpotLight->setLightDirection(
							glm::vec3(
								dir[0],
								dir[1],
								dir[2])
						);
					}

					if (ImGui::DragFloat("##Direction3", &dir[2], 0.1f, -100.0f, 100.0f, "%.2f", 0))
					{
						selectedSpotLight->setLightDirection(
							glm::vec3(
								dir[0],
								dir[1],
								dir[2])
						);
					}


					/* Light Rotation (Better for direction but no implemented yet)
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
					*/
				}

				//ShadowMap
				{
					ImGui::Spacing();
					ImGui::Text("Shadows");
					float near_plane = selectedSpotLight->getNearPlane();
					if (ImGui::InputFloat("Near Plane", &near_plane, 0.1f, 1.0f, "%.3f", 0))
						selectedSpotLight->setNearPlane(near_plane);
					float far_plane = selectedSpotLight->getFarPlane();
					if (ImGui::InputFloat("Far Plane", &far_plane, 0.1f, 1.0f, "%.3f", 0))
						selectedSpotLight->setFarPlane(far_plane);
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("SkyBox"))
			{
				currentTab = 3;
				for (int i = 0; i < myScene.m_SkyBox->m_CubeMapNames.size(); i++)
				{
					char label[128];
					bool isSelected = (skyBoxSelected == i);

					sprintf_s(label, myScene.m_SkyBox->m_CubeMapNames[i].c_str(), i);
					if (ImGui::Selectable(label, isSelected))
					{
						skyBoxSelected = i;
						myScene.m_SkyBox->setCubeMapTexture(skyBoxSelected);
					}
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
	}		
	ImGui::EndGroup();

}






