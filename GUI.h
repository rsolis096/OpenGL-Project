#pragma once
#include "Scene.h" 
#include "Sphere.h"
#include "Cube.h"
#include "Plane.h"
#include "Vendors/imgui/imgui.h"
#include "Vendors/imgui/imgui_impl_glfw.h"
#include "Vendors/imgui/imgui_impl_opengl3.h"

class GUI
{
	public:
		Scene& myScene;
		static bool isWindowHidden;
		GLFWwindow* window;
		//GUI();
		GUI(GLFWwindow* windowParam, Scene& scene);
		void displayWindow();
		//void setSceneCubes(std::vector<Cube*>& objects);
private:
	void drawList();
};