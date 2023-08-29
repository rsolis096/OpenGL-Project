#pragma once

#include "SceneManager.h" //Window functions
#include "Vendors/imgui/imgui.h"
#include "Vendors/imgui/imgui_impl_glfw.h"
#include "Vendors/imgui/imgui_impl_opengl3.h"

class GUI
{
	public:
		SceneManager& myScene;
		static bool isWindowHidden;
		GLFWwindow* window;
		//GUI();
		GUI(GLFWwindow* windowParam, SceneManager& scene);
		void displayWindow();
		//void setScenePrimitives(std::vector<Primitive*>& objects);
private:
	void drawList();
};