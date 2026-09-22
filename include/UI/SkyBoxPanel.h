#pragma once

struct UIContext;

class SkyBoxPanel
{
public:
    explicit SkyBoxPanel(UIContext& context);

    void drawSidebar();
    void drawInspector();

private:
    UIContext& m_Context;
    int m_SelectedIndex = 0;
};
