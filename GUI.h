#pragma once
#include "Object.h"
#include <GLFW/glfw3.h> //Window functions
#include "Vendors/imgui/imgui.h"
#include "Vendors/imgui/imgui_impl_glfw.h"
#include "Vendors/imgui/imgui_impl_opengl3.h"

class GUI
{
	public:
		std::vector<Object>& m_ScenePrimitives;
		static bool isWindowHidden;
		GLFWwindow* window;
		//GUI();
		GUI(GLFWwindow* windowParam, std::vector<Object>& scenePrimitives);
		void displayWindow();
		void setScenePrimitives(std::vector<Object>& scenePrimitives);
private:
	void drawList();
};