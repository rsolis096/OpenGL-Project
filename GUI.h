#pragma once

#include "Primitive.h"
#include <GLFW/glfw3.h> //Window functions
#include "Vendors/imgui/imgui.h"
#include "Vendors/imgui/imgui_impl_glfw.h"
#include "Vendors/imgui/imgui_impl_opengl3.h"

class GUI
{
	public:
		std::vector<Primitive*>& m_scenePrimitives;
		static bool isWindowHidden;
		GLFWwindow* window;
		//GUI();
		GUI(GLFWwindow* windowParam, std::vector<Primitive*>& objects);
		void displayWindow();
		void setScenePrimitives(std::vector<Primitive*>& objects);
private:
	void drawList();
};