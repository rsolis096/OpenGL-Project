#include "UI/GUI.h"

#include "Camera.h"
#include "World/Scene.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

bool GUI::isWindowHidden = true;

GUI::GUI(GLFWwindow* windowParam, Scene& scene)
    : m_Window(windowParam),
      m_Context{ scene },
      m_SceneObjectsPanel(m_Context),
      m_LightingPanel(m_Context),
      m_SkyBoxPanel(m_Context)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init();
    GUI::isWindowHidden = true;
}

void GUI::displayWindow()
{
    if (GUI::isWindowHidden)
        return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("OpenGL Project");
    ImGui::Text("%.3f fps", m_Context.scene.m_fps);
    drawWorkspace();
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::drawWorkspace()
{
    Scene& scene = m_Context.scene;
    if (scene.m_mainCamera != nullptr)
    {
        ImGui::Text(
            "Player Position: x: %.2f, y: %.2f, z: %.2f",
            scene.m_mainCamera->m_LookFrom[0],
            scene.m_mainCamera->m_LookFrom[1],
            scene.m_mainCamera->m_LookFrom[2]);
    }
    ImGui::Text("Object Count: %zu", scene.m_Entities.size());

    ImGui::BeginGroup();
    switch (m_ActivePanel)
    {
    case ActivePanel::Objects:
        m_SceneObjectsPanel.drawSidebar();
        break;
    case ActivePanel::Lighting:
        m_LightingPanel.drawSidebar();
        break;
    case ActivePanel::SkyBox:
        m_SkyBoxPanel.drawSidebar();
        break;
    }
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::BeginChild("item_view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

    if (ImGui::BeginTabBar("##MainPanels", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Objects"))
        {
            m_ActivePanel = ActivePanel::Objects;
            m_SceneObjectsPanel.drawInspector();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Lighting"))
        {
            m_ActivePanel = ActivePanel::Lighting;
            m_LightingPanel.drawInspector();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("SkyBox"))
        {
            m_ActivePanel = ActivePanel::SkyBox;
            m_SkyBoxPanel.drawInspector();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndChild();
    ImGui::EndGroup();
}
