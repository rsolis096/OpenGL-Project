#pragma once

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
    int m_SelectedIndex = 0;
    bool m_ShowModelDialog = false;
    bool m_ModelLoadFailed = false;
    Object* m_TexturePathObject = nullptr;
    std::array<char, 128> m_ModelPath{};
    std::array<char, 256> m_DiffusePath{};
    std::array<char, 256> m_SpecularPath{};
};
