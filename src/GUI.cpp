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

//Left Side of window
static int propertiesSelected; //Default selected object is first object in scene
static int lightingSelected; //Default selected object is first object in scene
static int skyBoxSelected; //Default selected object is first object in scene

void GUI::drawList()
{

	ImGui::Text("Player Position: x: %.2f, y: %.2f, z: %.2f", 
		myScene.mainCamera->cameraPos[0],
		myScene.mainCamera->cameraPos[1],
		myScene.mainCamera->cameraPos[2]);
	ImGui::Text("Object Count: %d", myScene.m_SceneObjects.size());

	//Current Tab from left to right
	static int currentTab = 1;
	static string selectedType = "none";

	//Left side of window 
	ImGui::BeginGroup();
	//Object List
	if (currentTab == 1) 
	{
		ImGui::BeginChild("object list", ImVec2(150, -ImGui::GetFrameHeightWithSpacing()), true);
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
		if (ImGui::Button("Add Cube")) {
			myScene.addObject(new Cube());
		}
		//Add Cube object to scene
		if (ImGui::Button("Add Sphere")) {
			myScene.addObject(new Sphere());
		}
		//Add Cube object to scene
		if (ImGui::Button("Add Plane")) {
			myScene.addObject(new Plane());
		}
	}
	//Light List
	else if (currentTab == 2)
	{
		//Fill the light source list
		ImGui::BeginChild("light list", ImVec2(150, 150), true);
		//Fill Side Panel with spotlights
		{
			unsigned short numSpotLights = myScene.m_LightController->m_SpotLights.size();
			unsigned short numPointLights = myScene.m_LightController->m_PointLights.size();
			unsigned int totalLights = numSpotLights + numPointLights;
			unsigned int lightCounter = 0;

			//Fill list of lights
			while (lightCounter < totalLights)
			{
				unsigned short s = 0;

				char label[128];
				bool isSelected = (lightingSelected == s);

				//Fill list with spotlights
				for (s; s < numSpotLights; s++, lightCounter++)
				{
					sprintf_s(label, myScene.m_LightController->m_SpotLights[s]->m_DisplayName.c_str());
					if (ImGui::Selectable(label, isSelected))
					{
						lightingSelected = s;
						selectedType = "spot";
					}
				}

				//Fill list with pointlights
				for (s; s < totalLights; s++, lightCounter++)
				{
					sprintf_s(label, myScene.m_LightController->m_PointLights[s - numSpotLights]->m_DisplayName.c_str());
					if (ImGui::Selectable(label, isSelected))
					{
						lightingSelected = s - numSpotLights;
						selectedType = "point";
					}
				}

			}		
		}
		ImGui::EndChild();

		//Add light buttons
		if (ImGui::Button("SpotLight +")) {

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

		if (ImGui::Button("PointLight +")) {

			if (myScene.m_LightController->m_PointLights.size() < 8)
			{
				myScene.m_LightController->addPointLight(glm::vec3(0.0f, 3.0f, 0.0f));
			}
			else
				std::cout << "Max PointLight Limit" << std::endl;
		}

	}
	ImGui::EndGroup();

	ImGui::SameLine();

	// Right side of window (light/Object properties)
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
						ImGui::Spacing();
						ImGui::Text("Scaling");
						float sX = selectedObject->m_Scale[0];
						float sY = selectedObject->m_Scale[1];
						float sZ = selectedObject->m_Scale[2];
						glm::vec3 currentScale = glm::vec3(sX, sY, sZ);

						if (ImGui::DragFloat("x##scale", &sX, 1.0f, 0.0f, 360.0f, "%.2f", 0))
						{

							currentScale[0] = sX;
							selectedObject->setScale(currentScale);
						}
						if (ImGui::DragFloat("y##scale", &sY, 1.0f, 0.0f, 360.0f, "%.2f", 0))
						{
							currentScale[1] = sY;
							selectedObject->setScale(currentScale);
						}
						if (ImGui::DragFloat("z##scale", &sZ, 1.0f, 0.0f, 360.0f, "%.2f", 0))
						{
							currentScale[2] = sZ;
							selectedObject->setScale(currentScale);
						}


					}

					// Rotation
					{
						ImGui::Spacing();
						ImGui::Text("Rotation");

						float rX = selectedObject->m_Rotation.x;
						float rY = selectedObject->m_Rotation.y;
						float rZ = selectedObject->m_Rotation.z;

						glm::vec3 currentRotation = glm::vec3(rX, rY, rZ);

						if (ImGui::DragFloat("x##rotation", &rX, 1.0f, 0.0f, 360.0f, "%.2f", 0))
						{
							
							currentRotation.x = rX;
							selectedObject->setRotation(currentRotation);
						}
						if (ImGui::DragFloat("y##rotation", &rY, 1.0f, 0.0f, 360.0f, "%.2f", 0))
						{
							currentRotation.y = rY;
							selectedObject->setRotation(currentRotation);
						}
						if (ImGui::DragFloat("z##rotation", &rZ, 1.0f, 0.0f, 360.0f, "%.2f", 0))
						{
							currentRotation.z = rZ;
							selectedObject->setRotation(currentRotation);
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
						if (selectedObject->m_HasTexture)
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
			if (ImGui::BeginTabItem("Lighting")){
				currentTab = 2;
				if (selectedType == "spot")
				{
					SpotLight* selectedSpotLight = nullptr;
					if (myScene.m_LightController->m_SpotLights.size() > 0)
					{
						selectedSpotLight = myScene.m_LightController->m_SpotLights[lightingSelected];
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
							static float dir[3] = { lightDir[0], lightDir[1], lightDir[2] };
							ImGui::Text("Direction");
							if (ImGui::DragFloat("##Direction1", &dir[0], 0.1f, -100.0f, 100.0f, "%.2f", 0))
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
					}
				}
				if (selectedType == "point")
				{
					PointLight* selectedPointLight = nullptr;
					if (myScene.m_LightController->m_PointLights.size() > 0)
					{
						selectedPointLight = myScene.m_LightController->m_PointLights[lightingSelected];
						glm::vec3 diffuse = selectedPointLight->getDiffuse();
						glm::vec3 ambient = selectedPointLight->getAmbient();
						glm::vec3 specular = selectedPointLight->getSpecular();

						// Light Color
						{
							ImGui::Text("Light Color", "NULL");
							float d[3] = { diffuse[0], diffuse[1], diffuse[2] };
							float a[3] = { ambient[0], ambient[1], ambient[2] };
							float s[3] = { specular[0], specular[1], specular[2] };

							bool change = false;
							if (ImGui::ColorEdit3("Diffuse###Lightcolor 1", d))
							{
								selectedPointLight->setDiffuse(glm::vec3(d[0], d[1], d[2]));
								change = true;
							}
							if (ImGui::ColorEdit3("Ambient###Lightcolor 2", a))
							{
								selectedPointLight->setAmbient(glm::vec3(a[0], a[1], a[2]));
								change = true;
							}
							if (ImGui::ColorEdit3("Specular###Lightcolor 3", s))
							{
								selectedPointLight->setSpecular(glm::vec3(s[0], s[1], s[2]));
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
							glm::vec3 lightPosition = selectedPointLight->getLightPos();
							float pos[3] = { lightPosition[0], lightPosition[1], lightPosition[2] };
							ImGui::Text("Position");
							if (ImGui::InputFloat3("##Position", pos, "%.3f", 0))
							{
								selectedPointLight->setLightPos(glm::vec3(pos[0], pos[1], pos[2]));
							}
						}

					}

				}
				ImGui::EndTabItem();
			}

			//Skybox tab
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






