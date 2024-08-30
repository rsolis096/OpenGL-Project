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
static int propertiesSelected = 0; //Default selected object is first object in scene
static int lightingSelected = 0; //Default selected object is first object in scene
static int skyBoxSelected = 0; //Default selected object is first object in scene
static bool showModel = false;
static bool modelSucceed = true;

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
	DirectionalLight* dirLight = myScene.m_LightController->m_DirectionalLight;

	//Left side of window 
	ImGui::BeginGroup();
	//Object List
	if (currentTab == 1) 
	{
		ImGui::BeginChild("object_list", ImVec2(150, ImGui::GetWindowHeight() * 0.5f), true);
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
		//Add Cube object to scene
		
		if (ImGui::Button(showModel ? "Cancel" : "Add Model")) {
			showModel = !showModel;
			
			//myScene.addObject(new Model());
		}
		if (showModel)
		{
			static char modelPathBuffer[128] = {0};

			ImGui::Text("Model Path:\n", "NULL");
			ImGui::InputText("##ModelPath", modelPathBuffer, sizeof(modelPathBuffer));


			if (ImGui::Button("Load Model")) {
				std::string path(modelPathBuffer);
				//Verify the path is valid for assimp (may still not load)
				ImGui::Text("Loading model...");

				if(!Model::CheckPath(path))
				{
					modelSucceed = false;
				}
				else
				{
					myScene.addObject(new Model(path));
					modelSucceed = true;
					showModel = false;
				}

				if(!modelSucceed)
					ImGui::Text("Model failed to be created, check file path.");

			}

			ImGui::SameLine(); HelpMarker(
				"The application will briefly pause when a model is loading.\n"
				"If the model fails to load, nothing will happen.\n");
		}
	}

	//Light List
	else if (currentTab == 2)
	{
		// Start the child window for the light list
		ImGui::BeginChild("light_list", ImVec2(150, ImGui::GetWindowHeight() * 0.5f), true);

		unsigned short numSpotLights = myScene.m_LightController->m_SpotLights.size();
		unsigned short numPointLights = myScene.m_LightController->m_PointLights.size();
		unsigned int totalLights = numSpotLights + numPointLights;

		// First, display the directional light if it exists
		if (dirLight != nullptr)
		{
			char label[128];
			bool isSelected = (selectedType == "dir");

			sprintf_s(label, "%s", dirLight->m_DisplayName.c_str());

			if (ImGui::Selectable(label, isSelected))
			{
				lightingSelected = 0;  // Assign a special value (like 0) for the directional light
				selectedType = "dir";
			}
		}

		// Fill the list of lights
		for (unsigned int i = 0; i < totalLights; ++i)
		{
			char label[128];
			bool isSelected = false;

			if (i < numSpotLights) // Spotlights
			{
				sprintf_s(label, "%s", myScene.m_LightController->m_SpotLights[i]->m_DisplayName.c_str());
				if (lightingSelected == i && selectedType == "spot") {
					isSelected = true;
				}

				if (ImGui::Selectable(label, isSelected))
				{
					lightingSelected = i;  // Correct index for spotlight
					selectedType = "spot";
				}
			}
			else // Point lights
			{
				unsigned int pointIndex = i - numSpotLights;
				sprintf_s(label, "%s", myScene.m_LightController->m_PointLights[pointIndex]->m_DisplayName.c_str());
				if (lightingSelected == pointIndex && selectedType == "point") {
					isSelected = true;
				}

				if (ImGui::Selectable(label, isSelected))
				{
					lightingSelected = pointIndex;  // Correct index for point light
					selectedType = "point";
				}
			}
		}

		ImGui::EndChild();


		//Add light buttons
		if (ImGui::Button("SpotLight +")) {

			if (myScene.m_LightController->m_SpotLights.size() < 8)
			{
				glm::vec3 pos = glm::vec3(0.0f, 3.0f, 0.0f);
				glm::vec3 dir = glm::vec3(-4.50f, -3.0f, 14.0f);
				dir = glm::normalize(dir);

				myScene.m_LightController->addSpotLight(pos, dir);
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

	//Skybox list
	else if (currentTab == 3)
	{
		ImGui::BeginChild("skybox_list", ImVec2(150, ImGui::GetWindowHeight() * 0.5f), true);
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
		ImGui::EndChild();
	}
	ImGui::EndGroup();

	ImGui::SameLine();

	// Right side of window 
	ImGui::BeginGroup();
	{
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us	
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			//Objects Tab
			if (ImGui::BeginTabItem("Objects"))
			{
				currentTab = 1;

				if (myScene.m_SceneObjects.size() > 0)
				{
					//Update the selected item (default selected item is the 0th object)
					Object* selectedObject = myScene.m_SceneObjects[propertiesSelected];
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

							if (ImGui::DragFloat("x##position", &vec4f[0], 0.25f, -1000.0f, 1000.0f, "%.2f", 0))
							{

								selectedObject->setPosition(glm::vec3(vec4f[0], vec4f[1], vec4f[2]));
								selectedObject->setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
								selectedObject->startFall = glfwGetTime();
							}

							if (ImGui::DragFloat("y##position", &vec4f[1], 0.25f, -1000.0f, 1000.0f, "%.2f", 0))
							{

								selectedObject->setPosition(glm::vec3(vec4f[0], vec4f[1], vec4f[2]));
								selectedObject->setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
								selectedObject->startFall = glfwGetTime();
							}

							if (ImGui::DragFloat("z##position", &vec4f[2], 0.25f, -1000.0f, 1000.0f, "%.2f", 0))
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

							if (ImGui::DragFloat("x##scale", &sX, 0.1f, 0.0f, 360.0f, "%.2f", 0))
							{

								currentScale[0] = sX;
								selectedObject->setScale(currentScale);
							}
							if (ImGui::DragFloat("y##scale", &sY, 0.1f, 0.0f, 360.0f, "%.2f", 0))
							{
								currentScale[1] = sY;
								selectedObject->setScale(currentScale);
							}
							if (ImGui::DragFloat("z##scale", &sZ, 0.1f, 0.0f, 360.0f, "%.2f", 0))
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

							if (ImGui::DragFloat("x##rotation", &rX, 0.5f, 0.0f, 360.0f, "%.2f", 0))
							{

								currentRotation.x = rX;
								selectedObject->setRotation(currentRotation);
							}
							if (ImGui::DragFloat("y##rotation", &rY, 0.5f, 0.0f, 360.0f, "%.2f", 0))
							{
								currentRotation.y = rY;
								selectedObject->setRotation(currentRotation);
							}
							if (ImGui::DragFloat("z##rotation", &rZ, 0.5f, 0.0f, 360.0f, "%.2f", 0))
							{
								currentRotation.z = rZ;
								selectedObject->setRotation(currentRotation);
							}


						}


						// Color
						{
							glm::vec3 diffuse = selectedObject->getDiffuse();
							glm::vec3 specular = selectedObject->getSpecular();
							glm::vec3 ambient = selectedObject->getAmbient();

							ImGui::Text("Object Color", "NULL");

							//Only allow Diffuse changes if it does not have a diffuse map
							if (selectedObject->m_DiffuseMap == nullptr) {
								if (ImGui::ColorEdit3("Diffuse", &diffuse[0]))
								{
									selectedObject->setDiffuse(glm::vec3(diffuse));

								}
							}
							//Only allow Specular changes if it does not have a specular map
							if (selectedObject->m_SpecularMap == nullptr)
							{
								if (ImGui::ColorEdit3("Specular", &specular[0]))
								{
									selectedObject->setSpecular(glm::vec3(specular));
								}
							}
							//Yet to see an Ambient map so, it should be always available
							if (ImGui::ColorEdit3("Ambient", &ambient[0]))
							{
								selectedObject->setAmbient(glm::vec3(ambient));

							}

						}

						ImGui::SameLine(); HelpMarker(
							"Click on the color square to open a color picker.\n"
							"Click and hold to use drag and drop.\n"
							"Right-click on the color square to show options.\n"
							"CTRL+click on individual component to input value.\n");
						ImGui::Spacing();

						// Texture
						{
							static std::string diffuseText = "";
							static std::string specularText = "";
							static char diffuseBuffer[256]; // This is where the text will be stored
							static char specularBuffer[256]; // This is where the text will be stored
							static bool showSuccessText = false;

							ImGui::Text("Texture Paths:");
							if (selectedObject->m_HasTexture && selectedObject->GetType() != Object::ObjectType::Model)
							{
								//Set textbox default values
								if (diffuseText == "")
									strcpy_s(diffuseBuffer, selectedObject->m_DiffuseMap->m_Path.c_str());
								else
									strcpy_s(diffuseBuffer, diffuseText.c_str()); //dest, src

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
								if (selectedObject->isPhysicsObject == false)
									myScene.m_PhysicsWorld->addObject(selectedObject);
								selectedObject->setPhysics();
							}
						}

						// Delete Item
						if (ImGui::Button("Delete Object"))
						{
							std::cout << "Selected To Delete " << selectedObject->m_DisplayName << "\n";
							myScene.removeObject(selectedObject);
							propertiesSelected -= 1;
							if(propertiesSelected < 0)
							{
								propertiesSelected = 0;
							}
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
								if (intensity < 0)
									intensity = 0;
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

							if (ImGui::ColorEdit3("Diffuse###SpotLightcolor 1", d))
								selectedSpotLight->setDiffuse(glm::vec3(d[0], d[1], d[2]));
							
							if (ImGui::ColorEdit3("Ambient###SpotLightcolor 2", a))
								selectedSpotLight->setAmbient(glm::vec3(a[0], a[1], a[2]));
							
							if (ImGui::ColorEdit3("Specular###SpotLightcolor 3", s))
								selectedSpotLight->setSpecular(glm::vec3(s[0], s[1], s[2]));
							

							ImGui::SameLine(); HelpMarker(
								"Click on the color square to open a color picker.\n"
								"Click and hold to use drag and drop.\n"
								"Right-click on the color square to show options.\n"
								"CTRL+click on individual component to input value.\n");
							ImGui::Spacing();
						}

						// Position
						{

							//For transformation
							glm::vec3 pos = selectedSpotLight->getLightPos();
							float vec4f[3] = { pos[0], pos[1], pos[2] };
							ImGui::Text("Current Light Position:\tx: %.2f, y: %.2f, z: %.2f", pos.x, pos.y, pos.z);

							if (ImGui::DragFloat("x##spotlight_position", &vec4f[0], 0.25f, -1000.0f, 1000.0f, "%.2f", 0))
								selectedSpotLight->setLightPos(glm::vec3(vec4f[0], vec4f[1], vec4f[2]));

							if (ImGui::DragFloat("y##spotlight_position", &vec4f[1], 0.25f, -1000.0f, 1000.0f, "%.2f", 0))
								selectedSpotLight->setLightPos(glm::vec3(vec4f[0], vec4f[1], vec4f[2]));

							if (ImGui::DragFloat("z##spotlight_position", &vec4f[2], 0.25f, -1000.0f, 1000.0f, "%.2f", 0))
								selectedSpotLight->setLightPos(glm::vec3(vec4f[0], vec4f[1], vec4f[2]));

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

						//Shadow Map Properties
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

						//Shadow Map Resolution
						{

							int shadowHeight = selectedSpotLight->getShadowHeight();
							int shadowWidth = selectedSpotLight->getShadowWidth();
							ImGui::Text("Current Shadow Map Resolution:\t %d x %d", shadowHeight, shadowWidth);

							static int newShadowHeight = shadowHeight; // Store the original value
							static int newShadowWidth = shadowWidth;   // Store the original value

							ImGui::InputInt("Shadow Height", &newShadowHeight);
							//ImGui::InputInt("Shadow Width", &newShadowWidth);

							if (ImGui::Button("Submit")) {
								// Check if the values have changed
								if (newShadowHeight != shadowHeight || newShadowWidth != shadowWidth) {
									selectedSpotLight->setShadowHeight(newShadowHeight);
									selectedSpotLight->setShadowWidth(newShadowHeight);
									myScene.m_ShadowMap->updateShadowResolution(selectedSpotLight);
								}
							}

						}
						
						// Display the depth map for spotlights
						if (selectedSpotLight->getDepthMapTexture() != 0) {
							ImGui::Text("Spotlight Depth Map:");
							GLuint colorTexture = myScene.m_ShadowMap->renderDepthMapToGUI(
								selectedSpotLight->getDepthMapTexture(),
								selectedSpotLight->getShadowHeight(),
								selectedSpotLight->getShadowWidth()
							);
							ImGui::Image((void*)colorTexture, ImVec2(256, 256));
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

						// Position
						{

							//For transformation
							glm::vec3 pos = selectedPointLight->getLightPos();
							float vec4f[3] = { pos[0], pos[1], pos[2] };
							ImGui::Text("Current Light Position:\tx: %.2f, y: %.2f, z: %.2f", pos.x, pos.y, pos.z);

							if (ImGui::DragFloat("x##pointlight_position", &vec4f[0], 0.25f, -1000.0f, 1000.0f, "%.2f", 0))
								selectedPointLight->setLightPos(glm::vec3(vec4f[0], vec4f[1], vec4f[2]));

							if (ImGui::DragFloat("y##pointlight_position", &vec4f[1], 0.25f, -1000.0f, 1000.0f, "%.2f", 0))
								selectedPointLight->setLightPos(glm::vec3(vec4f[0], vec4f[1], vec4f[2]));

							if (ImGui::DragFloat("z##pointlight_position", &vec4f[2], 0.25f, -1000.0f, 1000.0f, "%.2f", 0))
								selectedPointLight->setLightPos(glm::vec3(vec4f[0], vec4f[1], vec4f[2]));

						}

					}

				}
				if(selectedType == "dir")
				{
					ImGui::Text("Directional Light");

						// Light Color
						{
							glm::vec3 diffuse = dirLight->getDiffuse();
							glm::vec3 ambient = dirLight->getAmbient();
							glm::vec3 specular = dirLight->getSpecular();
							float intensity = dirLight->getIntensity();

							ImGui::Text("Light Color", "NULL");
							float d[3] = { diffuse[0] / intensity , diffuse[1] / intensity, diffuse[2] / intensity };
							float a[3] = { ambient[0] / intensity , ambient[1] / intensity, ambient[2] / intensity };
							float s[3] = { specular[0] / intensity , specular[1] / intensity, specular[2] / intensity };

							if (ImGui::ColorEdit3("Diffuse###DirLightcolor 1", d))
								dirLight->setDiffuse(glm::vec3(d[0], d[1], d[2]));
							
							if (ImGui::ColorEdit3("Ambient###DirLightcolor 2", a))
								dirLight->setAmbient(glm::vec3(a[0], a[1], a[2]));
							
							if (ImGui::ColorEdit3("Specular###DirLightcolor 3", s))
								dirLight->setSpecular(glm::vec3(s[0], s[1], s[2]));
							

							ImGui::SameLine(); HelpMarker(
								"Click on the color square to open a color picker.\n"
								"Click and hold to use drag and drop.\n"
								"Right-click on the color square to show options.\n"
								"CTRL+click on individual component to input value.\n");
							ImGui::Spacing();
						}

					// Resolution
					{

						int shadowHeight = dirLight->getShadowHeight();
						int shadowWidth = dirLight->getShadowWidth();
						ImGui::Text("Current Shadow Map Resolution:\t %d x %d", shadowHeight, shadowWidth);

						static int newShadowHeight = shadowHeight; // Store the original value
						static int newShadowWidth = shadowWidth;   // Store the original value

						ImGui::InputInt("Shadow Height", &newShadowHeight);
						//ImGui::InputInt("Shadow Width", &newShadowWidth);

						if (ImGui::Button("Submit")) {
							// Check if the values have changed
							if (newShadowHeight != shadowHeight || newShadowWidth != shadowWidth) {
								dirLight->setShadowHeight(newShadowHeight);
								dirLight->setShadowWidth(newShadowHeight);
								myScene.m_ShadowMap->updateShadowResolution(dirLight);
							}
						}

					}


					if(dirLight != nullptr)
					{
						ImGui::Text("Spotlight Depth Map:");
						GLuint colorTexture = myScene.m_ShadowMap->renderDepthMapToGUI(dirLight->getDepthMapTexture(),
							dirLight->getShadowWidth(),
							dirLight->getShadowHeight()
							);

						ImGui::Image((void*)colorTexture, ImVec2(256, 256));
					}

					
				}
				ImGui::EndTabItem();
			}

			//Skybox tab
			if (ImGui::BeginTabItem("SkyBox"))
			{
				currentTab = 3;
				bool check = myScene.m_SkyBox->m_InvertedTexture;
				ImGui::Text(myScene.m_SkyBox->m_CubeMapNames[skyBoxSelected].c_str());
				if(ImGui::Checkbox("Invert on Y ", &check))
				{
					myScene.m_SkyBox->m_InvertedTexture = !myScene.m_SkyBox->m_InvertedTexture;
					myScene.m_SkyBox->setCubeMapTexture(skyBoxSelected);
				}

				ImGui::SameLine(); HelpMarker(
					"It's possible the skybox may be inverted when loaded.\n"
					"If needed, invert the skybox texture.\n");
				ImGui::Spacing();

				ImGui::EndTabItem();
			}


			ImGui::EndTabBar();
		}
		ImGui::EndChild();
	}		
	ImGui::EndGroup();

}







