#pragma once
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
