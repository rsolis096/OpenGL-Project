#pragma once

#include "Scene.h" 
#include "Sphere.h"
#include "Cube.h"
#include "Plane.h"
#include "PhysicsWorld.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


class GUI
{
	public:
		Scene& myScene;
		static bool isWindowHidden;
		GLFWwindow* window;
		GUI(GLFWwindow* windowParam, Scene& scene);
		void displayWindow();
private:
	void drawList();
};