#pragma once

#include "Objects/ObjectTypes.h"

#include <array>

struct UIContext;
class Object;

class SceneObjectsPanel
{
public:
    explicit SceneObjectsPanel(UIContext& context);

    void drawSidebar();
    void drawInspector();

private:
    Object* selectedObject();
    void syncTexturePaths(Object* object);
    void drawModelDialog();

    UIContext& m_Context;

    EntityId m_SelectedEntityId = InvalidEntityId;

    bool m_ShowModelDialog = false;
    bool m_ModelLoadFailed = false;
    EntityId m_TexturePathEntityId = InvalidEntityId;
    std::array<char, 128> m_ModelPath{};
    std::array<char, 256> m_DiffusePath{};
    std::array<char, 256> m_SpecularPath{};
};
