#pragma once

#include "UI/LightingPanel.h"
#include "UI/SceneObjectsPanel.h"
#include "UI/SkyBoxPanel.h"
#include "UI/UIContext.h"

// Forward Declarations
class Scene;
struct GLFWwindow;

class GUI
{
public:
	static bool isWindowHidden;
	GUI(GLFWwindow* windowParam, Scene& scene);
	void displayWindow();

private:
	enum class ActivePanel
	{
		Objects,
		Lighting,
		SkyBox
	};

	void drawWorkspace();

	GLFWwindow* m_Window;
	UIContext m_Context;
	SceneObjectsPanel m_SceneObjectsPanel;
	LightingPanel m_LightingPanel;
	SkyBoxPanel m_SkyBoxPanel;
	ActivePanel m_ActivePanel = ActivePanel::Objects;
};
