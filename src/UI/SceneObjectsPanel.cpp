#include "UI/SceneObjectsPanel.h"

#include "UI/UIContext.h"
#include "Objects/Cube.h"
#include "Objects/Model.h"
#include "Objects/Object.h"
#include "Objects/Plane.h"
#include "Objects/Sphere.h"
#include "World/Scene.h"

#include <imgui/imgui.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

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

    void copyPath(std::array<char, 256>& destination, const std::string& source)
    {
        strncpy_s(destination.data(), destination.size(), source.c_str(), _TRUNCATE);
    }
}

SceneObjectsPanel::SceneObjectsPanel(UIContext& context)
    : m_Context(context)
{
}

Object* SceneObjectsPanel::selectedObject()
{
    auto& objects = m_Context.scene.m_Entities;
    if (objects.empty())
    {
        m_SelectedIndex = 0;
        return nullptr;
    }

    m_SelectedIndex = std::max(0, std::min(m_SelectedIndex, static_cast<int>(objects.size()) - 1));
    return objects[m_SelectedIndex].get();
}

void SceneObjectsPanel::syncTexturePaths(Object* object)
{
    if (object == m_TexturePathObject)
        return;

    m_TexturePathObject = object;
    m_DiffusePath.fill('\0');
    m_SpecularPath.fill('\0');

    if (object == nullptr)
        return;

    if (object->m_Material.m_DiffuseMap)
        copyPath(m_DiffusePath, object->m_Material.m_DiffuseMap->m_Path);
    if (object->m_Material.m_SpecularMap)
        copyPath(m_SpecularPath, object->m_Material.m_SpecularMap->m_Path);
}

void SceneObjectsPanel::drawModelDialog()
{
    if (!m_ShowModelDialog)
        return;

    ImGui::Text("Model Path:");
    ImGui::InputText("##ModelPath", m_ModelPath.data(), m_ModelPath.size());

    if (ImGui::Button("Load Model"))
    {
        const std::string path(m_ModelPath.data());
        if (Model::CheckPath(path) == nullptr)
        {
            m_ModelLoadFailed = true;
        }
        else
        {
            m_Context.scene.createEntity<Model>(path);
            m_ModelLoadFailed = false;
            m_ShowModelDialog = false;
            m_SelectedIndex = static_cast<int>(m_Context.scene.m_Entities.size()) - 1;
        }
    }

    ImGui::SameLine();
    drawHelpMarker(
        "The application will briefly pause while a model loads.\n"
        "If loading fails, verify the model path.");

    if (m_ModelLoadFailed)
        ImGui::TextUnformatted("Model failed to be created; check the file path.");
}

void SceneObjectsPanel::drawSidebar()
{
    auto& scene = m_Context.scene;
    ImGui::BeginChild("object_list", ImVec2(150, ImGui::GetWindowHeight() * 0.5f), true);

    for (int index = 0; index < static_cast<int>(scene.m_Entities.size()); ++index)
    {
        Object* object = scene.m_Entities[index].get();
        if (object != nullptr && ImGui::Selectable(object->m_EntityInfo.displayName.c_str(), m_SelectedIndex == index))
            m_SelectedIndex = index;
    }

    ImGui::EndChild();

    if (ImGui::Button("Add Cube"))
        scene.createEntity<Cube>();
    if (ImGui::Button("Add Sphere"))
        scene.createEntity<Sphere>();
    if (ImGui::Button("Add Plane"))
        scene.createEntity<Plane>();

    if (ImGui::Button(m_ShowModelDialog ? "Cancel" : "Add Model"))
    {
        m_ShowModelDialog = !m_ShowModelDialog;
        m_ModelLoadFailed = false;
    }

    drawModelDialog();
}

void SceneObjectsPanel::drawInspector()
{
    Object* object = selectedObject();
    syncTexturePaths(object);

    if (object == nullptr)
    {
        ImGui::TextUnformatted("No scene object selected.");
        return;
    }

    ImGui::Text("Selected Item: %s", object->m_EntityInfo.displayName.c_str());
    ImGui::Spacing();

    glm::vec3 position = object->m_Transform.m_Position;
    ImGui::Text("Current Object Position: x: %.2f, y: %.2f, z: %.2f", position.x, position.y, position.z);
    if (ImGui::DragFloat3("Position", &position.x, 0.25f, -1000.0f, 1000.0f, "%.2f"))
        object->m_Transform.setPosition(position);

    glm::vec3 scale = object->m_Transform.m_Scale;
    if (ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.0f, 360.0f, "%.2f"))
        object->m_Transform.setScale(scale);

    glm::vec3 rotation = object->m_Transform.m_Rotation;
    if (ImGui::DragFloat3("Rotation", &rotation.x, 0.5f, 0.0f, 360.0f, "%.2f"))
        object->m_Transform.setRotation(rotation);

    ImGui::Spacing();
    ImGui::TextUnformatted("Object Color");

    glm::vec3 diffuse = object->m_Material.getDiffuse();
    if (!object->m_Material.m_DiffuseMap && ImGui::ColorEdit3("Diffuse", &diffuse.x))
        object->m_Material.setDiffuse(diffuse);

    glm::vec3 specular = object->m_Material.getSpecular();
    if (!object->m_Material.m_SpecularMap && ImGui::ColorEdit3("Specular", &specular.x))
        object->m_Material.setSpecular(specular);

    glm::vec3 ambient = object->m_Material.getAmbient();
    if (ImGui::ColorEdit3("Ambient", &ambient.x))
        object->m_Material.setAmbient(ambient);

    ImGui::SameLine();
    drawHelpMarker(
        "Click the color square to open a picker.\n"
        "CTRL+click a component to enter a value.");

    ImGui::Spacing();
    ImGui::TextUnformatted("Texture Paths:");
    ImGui::TextUnformatted("Diffuse Path:");
    ImGui::InputText("##DiffusePath", m_DiffusePath.data(), m_DiffusePath.size());
    ImGui::TextUnformatted("Specular Path:");
    ImGui::InputText("##SpecularPath", m_SpecularPath.data(), m_SpecularPath.size());

    if (ImGui::Button("Set Texture"))
    {
        object->updateTexture({
            std::string(m_DiffusePath.data()),
            std::string(m_SpecularPath.data())
        });
    }

    ImGui::SameLine();
    drawHelpMarker("If only one texture is desired, use the same file path in both fields.");

    if (ImGui::Button("Delete Object"))
    {
        std::cout << "Selected To Delete " << object->m_EntityInfo.displayName << '\n';
        m_Context.scene.destroyEntity(object->id());
        m_SelectedIndex = std::max(0, m_SelectedIndex - 1);
        m_TexturePathObject = nullptr;
    }
}
