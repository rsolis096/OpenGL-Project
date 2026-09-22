#pragma once

#include <string>

struct UIContext;
class DirectionalLight;
class PointLight;
class SpotLight;

class LightingPanel
{
public:
    explicit LightingPanel(UIContext& context);

    void drawSidebar();
    void drawInspector();

private:
    enum class SelectionType
    {
        None,
        Directional,
        Spot,
        Point
    };

    void drawSpotLight(SpotLight& light);
    void drawPointLight(PointLight& light);
    void drawDirectionalLight(DirectionalLight& light);

    UIContext& m_Context;
    SelectionType m_SelectionType = SelectionType::None;
    unsigned int m_SelectedIndex = 0;
    bool m_RenderDebugDepthMap = false;
};
