#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// Forward Declarations
class Scene;
struct GLFWwindow;

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