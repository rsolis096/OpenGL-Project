#include "UI/LightingPanel.h"

#include "UI/UIContext.h"
#include "Lighting/DirectionalLight.h"
#include "Lighting/Light.h"
#include "Lighting/LightController.h"
#include "Lighting/PointLight.h"
#include "Lighting/ShadowMap.h"
#include "Lighting/SpotLight.h"
#include "World/Scene.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>

#include <algorithm>
#include <cstdint>
#include <iostream>

namespace
{
    void drawHelpMarker(const char* description)
    {
        ImGui::TextDisabled("(?)");
        if (!ImGui::BeginItemTooltip())
            return;

        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(description);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    void drawLightColors(Light& light, const char* idSuffix)
    {
        float intensity = light.getIntensity();
        glm::vec3 diffuse = light.getDiffuse();
        glm::vec3 ambient = light.getAmbient();
        glm::vec3 specular = light.getSpecular();

        const std::string intensityLabel = std::string("Intensity##") + idSuffix;
        if (ImGui::DragFloat(intensityLabel.c_str(), &intensity, 0.5f, 0.0f, 100.0f, "%.2f"))
            light.setIntensity(intensity);

        const std::string diffuseLabel = std::string("Diffuse##") + idSuffix;
        const std::string ambientLabel = std::string("Ambient##") + idSuffix;
        const std::string specularLabel = std::string("Specular##") + idSuffix;
        if (ImGui::ColorEdit3(diffuseLabel.c_str(), &diffuse.x))
            light.setDiffuse(diffuse);
        if (ImGui::ColorEdit3(ambientLabel.c_str(), &ambient.x))
            light.setAmbient(ambient);
        if (ImGui::ColorEdit3(specularLabel.c_str(), &specular.x))
            light.setSpecular(specular);
    }

    template <typename Light>
    void drawPosition(Light& light, const char* idSuffix)
    {
        glm::vec3 position = light.getLightPos();
        const std::string label = std::string("Position##") + idSuffix;
        if (ImGui::DragFloat3(label.c_str(), &position.x, 0.25f, -1000.0f, 1000.0f, "%.2f"))
            light.setLightPos(position);
    }

    template <typename Light>
    void drawShadowProperties(Light& light, const char* idSuffix)
    {
        float nearPlane = light.getNearPlane();
        float farPlane = light.getFarPlane();
        float fov = light.getShadowFOV();
        float bias = light.getShadowBias();
        bool updateShadowPass = light.getShadowPassUpdate();

        std::string label = std::string("Near Plane##") + idSuffix;
        if (ImGui::InputFloat(label.c_str(), &nearPlane, 0.1f, 1.0f, "%.3f"))
            light.setNearPlane(nearPlane);
        label = std::string("Far Plane##") + idSuffix;
        if (ImGui::InputFloat(label.c_str(), &farPlane, 0.1f, 1.0f, "%.3f"))
            light.setFarPlane(farPlane);
        label = std::string("Shadow FOV##") + idSuffix;
        if (ImGui::InputFloat(label.c_str(), &fov, 0.1f, 1.0f, "%.3f"))
            light.setShadowFOV(fov);
        label = std::string("Shadow Bias##") + idSuffix;
        if (ImGui::InputFloat(label.c_str(), &bias, 0.01f, 1.0f, "%.3f"))
            light.setShadowBias(bias);
        label = std::string("Enable Shadow Pass##") + idSuffix;
        if (ImGui::Checkbox(label.c_str(), &updateShadowPass))
            light.setShadowPassUpdate(updateShadowPass);
    }
}

LightingPanel::LightingPanel(UIContext& context)
    : m_Context(context)
{
}

void LightingPanel::drawSidebar()
{
    LightController* controller = m_Context.scene.m_LightController;
    if (controller == nullptr)
    {
        ImGui::TextUnformatted("No light controller.");
        return;
    }

    ImGui::BeginChild("light_list", ImVec2(150, ImGui::GetWindowHeight() * 0.5f), true);

    if (controller->m_DirectionalLight != nullptr &&
        ImGui::Selectable(controller->m_DirectionalLight->getDisplayName().c_str(), m_SelectionType == SelectionType::Directional))
    {
        m_SelectionType = SelectionType::Directional;
        m_SelectedIndex = 0;
    }

    for (unsigned int index = 0; index < controller->m_SpotLights.size(); ++index)
    {
        SpotLight* light = controller->m_SpotLights[index];
        if (light != nullptr && ImGui::Selectable(light->getDisplayName().c_str(),
            m_SelectionType == SelectionType::Spot && m_SelectedIndex == index))
        {
            m_SelectionType = SelectionType::Spot;
            m_SelectedIndex = index;
        }
    }

    for (unsigned int index = 0; index < controller->m_PointLights.size(); ++index)
    {
        PointLight* light = controller->m_PointLights[index];
        if (light != nullptr && ImGui::Selectable(light->getDisplayName().c_str(),
            m_SelectionType == SelectionType::Point && m_SelectedIndex == index))
        {
            m_SelectionType = SelectionType::Point;
            m_SelectedIndex = index;
        }
    }

    ImGui::EndChild();

    if (ImGui::Button("SpotLight +"))
    {
        if (controller->m_SpotLights.size() < 8)
        {
            const glm::vec3 position(0.0f, 3.0f, 0.0f);
            const glm::vec3 direction = glm::normalize(glm::vec3(-4.5f, -3.0f, 14.0f));
            controller->addSpotLight(position, direction);
        }
        else
            std::cout << "Max SpotLight Limit\n";
    }

    if (ImGui::Button("PointLight +"))
    {
        if (controller->m_PointLights.size() < 8)
            controller->addPointLight(glm::vec3(0.0f, 3.0f, 0.0f));
        else
            std::cout << "Max PointLight Limit\n";
    }

    if (ImGui::Button("Directional Light +"))
    {
        if (controller->m_DirectionalLight == nullptr)
            controller->addDirectionalLight(glm::vec3(0.0f));
        else
            std::cout << "Directional Light already exists!\n";
    }
}

void LightingPanel::drawSpotLight(SpotLight& light)
{
    ImGui::TextUnformatted("SpotLight");
    drawLightColors(light, "spot");
    ImGui::SameLine();
    drawHelpMarker("Edit the light's base colors and intensity.");
    drawPosition(light, "spot");

    float pitch = light.getPitch();
    float yaw = light.getYaw();
    if (ImGui::DragFloat("Pitch##spot", &pitch, 0.1f, -89.9f, 89.9f, "%.2f"))
        light.setPitch(pitch);
    if (ImGui::DragFloat("Yaw##spot", &yaw, 0.1f, -180.0f, 180.0f, "%.2f"))
        light.setYaw(yaw);

    ImGui::Spacing();
    ImGui::TextUnformatted("Shadows");
    drawShadowProperties(light, "spot");

    int resolution = light.getShadowHeight();
    if (ImGui::InputInt("Shadow Resolution##spot", &resolution))
    {
        resolution = std::max(resolution, 1);
        light.setShadowHeight(resolution);
        light.setShadowWidth(resolution);
        if (m_Context.scene.m_shadowMap)
            m_Context.scene.m_shadowMap->updateShadowResolution(&light);
    }

    ImGui::Checkbox("Render Debug Depth Map", &m_RenderDebugDepthMap);
    if (m_RenderDebugDepthMap && light.getDepthMapTexture() != 0 && m_Context.scene.m_shadowMap)
    {
        const GLuint texture = m_Context.scene.m_shadowMap->renderDepthMapToGUI(
            light.getDepthMapTexture(), light.getShadowHeight(), light.getShadowWidth());
        ImGui::Image(reinterpret_cast<void*>(static_cast<std::intptr_t>(texture)), ImVec2(256, 256));
    }
}

void LightingPanel::drawPointLight(PointLight& light)
{
    ImGui::TextUnformatted("PointLight");
    drawLightColors(light, "point");
    ImGui::SameLine();
    drawHelpMarker("Edit the light's base colors and intensity.");
    drawPosition(light, "point");

    ImGui::Spacing();
    ImGui::TextUnformatted("Shadows");
    drawShadowProperties(light, "point");
}

void LightingPanel::drawDirectionalLight(DirectionalLight& light)
{
    ImGui::TextUnformatted("Directional Light");
    drawLightColors(light, "directional");

    float nearPlane = light.getNearPlane();
    float farPlane = light.getFarPlane();
    if (ImGui::DragFloat("Near Plane##directional", &nearPlane, 0.1f, 0.1f, 1000.0f, "%.2f"))
        light.setNearPlane(nearPlane);
    if (ImGui::DragFloat("Far Plane##directional", &farPlane, 0.1f, 0.1f, 1000.0f, "%.2f"))
        light.setFarPlane(farPlane);

    if (ImGui::DragFloat3("Position##directional", &light.m_LightPosition.x, 0.25f, -1000.0f, 1000.0f, "%.2f"))
    {
        // The directional light currently exposes its position directly.
    }

    float pitch = light.getPitch();
    float yaw = light.getYaw();
    float radius = light.getRadius();
    if (ImGui::DragFloat("Pitch##directional", &pitch, 0.1f, -89.9f, 89.9f, "%.2f"))
        light.setPitch(pitch);
    if (ImGui::DragFloat("Yaw##directional", &yaw, 0.1f, -180.0f, 180.0f, "%.2f"))
        light.setYaw(yaw);
    if (ImGui::DragFloat("Radius##directional", &radius, 0.1f, 10.0f, 100.0f, "%.2f"))
        light.setRadius(radius);

    int resolution = light.getShadowHeight();
    if (ImGui::InputInt("Shadow Resolution##directional", &resolution))
    {
        resolution = std::max(resolution, 1);
        light.setShadowHeight(resolution);
        light.setShadowWidth(resolution);
        if (m_Context.scene.m_shadowMap)
            m_Context.scene.m_shadowMap->updateShadowResolution(&light);
    }

    if (m_Context.scene.m_shadowMap && light.getDepthMapTexture() != 0)
    {
        ImGui::TextUnformatted("Directional Depth Map:");
        const GLuint texture = m_Context.scene.m_shadowMap->renderDepthMapToGUI(
            light.getDepthMapTexture(), light.getShadowWidth(), light.getShadowHeight());
        ImGui::Image(reinterpret_cast<void*>(static_cast<std::intptr_t>(texture)), ImVec2(256, 256));
    }

    bool showShadowArea = light.getShadowArea();
    if (ImGui::Checkbox("Show Shadow Area", &showShadowArea))
        light.showShadowArea();

    if (ImGui::Button("Delete Directional Light"))
    {
        m_Context.scene.m_LightController->removeDirectionalLight();
        m_SelectionType = SelectionType::None;
        m_SelectedIndex = 0;
    }
}

void LightingPanel::drawInspector()
{
    LightController* controller = m_Context.scene.m_LightController;
    if (controller == nullptr)
    {
        ImGui::TextUnformatted("No light controller.");
        return;
    }

    switch (m_SelectionType)
    {
    case SelectionType::Directional:
        if (controller->m_DirectionalLight)
            drawDirectionalLight(*controller->m_DirectionalLight);
        else
            m_SelectionType = SelectionType::None;
        break;
    case SelectionType::Spot:
        if (m_SelectedIndex < controller->m_SpotLights.size() && controller->m_SpotLights[m_SelectedIndex])
            drawSpotLight(*controller->m_SpotLights[m_SelectedIndex]);
        else
            m_SelectionType = SelectionType::None;
        break;
    case SelectionType::Point:
        if (m_SelectedIndex < controller->m_PointLights.size() && controller->m_PointLights[m_SelectedIndex])
            drawPointLight(*controller->m_PointLights[m_SelectedIndex]);
        else
            m_SelectionType = SelectionType::None;
        break;
    case SelectionType::None:
        ImGui::TextUnformatted("Select a light to edit its properties.");
        break;
    }
}
