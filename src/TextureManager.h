#pragma once

class TextureManager
{
public:
    static unsigned int getNextUnit();
    static unsigned int getCurrentUnit();
    static void reset(unsigned int val = 0);

private:
    static unsigned int m_CurrentUnit;
    static constexpr unsigned int m_MaxUnits = 16;
};