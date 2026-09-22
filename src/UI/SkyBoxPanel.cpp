#include "UI/SkyBoxPanel.h"

#include "UI/UIContext.h"
#include "Objects/SkyBox.h"
#include "World/Scene.h"

#include <imgui/imgui.h>

#include <algorithm>

SkyBoxPanel::SkyBoxPanel(UIContext& context)
    : m_Context(context)
{
}

void SkyBoxPanel::drawSidebar()
{
    SkyBox* skyBox = m_Context.scene.m_skyBox;
    if (skyBox == nullptr)
    {
        ImGui::TextUnformatted("No skybox configured.");
        return;
    }

    const auto& names = skyBox->m_CubeMapNames;
    if (!names.empty())
        m_SelectedIndex = std::max(0, std::min(m_SelectedIndex, static_cast<int>(names.size()) - 1));

    ImGui::BeginChild("skybox_list", ImVec2(150, ImGui::GetWindowHeight() * 0.5f), true);
    for (int index = 0; index < static_cast<int>(names.size()); ++index)
    {
        if (ImGui::Selectable(names[index].c_str(), m_SelectedIndex == index))
        {
            m_SelectedIndex = index;
            skyBox->setCubeMapTexture(m_SelectedIndex);
        }
    }
    ImGui::EndChild();
}

void SkyBoxPanel::drawInspector()
{
    SkyBox* skyBox = m_Context.scene.m_skyBox;
    if (skyBox == nullptr || skyBox->m_CubeMapNames.empty())
    {
        ImGui::TextUnformatted("No skybox configured.");
        return;
    }

    m_SelectedIndex = std::max(0, std::min(m_SelectedIndex, static_cast<int>(skyBox->m_CubeMapNames.size()) - 1));
    ImGui::TextUnformatted(skyBox->m_CubeMapNames[m_SelectedIndex].c_str());

    bool inverted = skyBox->m_InvertedTexture;
    if (ImGui::Checkbox("Invert on Y", &inverted))
    {
        skyBox->m_InvertedTexture = inverted;
        skyBox->setCubeMapTexture(m_SelectedIndex);
    }

    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::TextUnformatted("Invert the skybox vertically if its source orientation requires it.");
        ImGui::EndTooltip();
    }
}
