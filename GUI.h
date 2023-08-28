#pragma once
#include "Sphere.h"
#include "Cube.h"
#include <GLFW/glfw3.h> //Window functions
#include "Vendors/imgui/imgui.h"
#include "Vendors/imgui/imgui_impl_glfw.h"
#include "Vendors/imgui/imgui_impl_opengl3.h"

class GUI
{
	public:
		std::vector<Object*>& m_scenePrimitives;
		static bool isWindowHidden;
		GLFWwindow* window;
		//GUI();
		GUI(GLFWwindow* windowParam, std::vector<Object*>& objects);
		void displayWindow();
		void setScenePrimitives(std::vector<Object*>& objects);
private:
	void drawList();
};