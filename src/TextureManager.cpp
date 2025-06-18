#include "TextureManager.h"

unsigned int TextureManager::m_HighestUnit = 0;
unsigned int TextureManager::m_CurrentUnit = 0;

// Returns next available integer to use as texture unit
unsigned int TextureManager::getNextUnit()
{
	if (m_HighestUnit >= m_MaxUnits)
	{
		std::cout << "All slots used!" << std::endl;
		return 0;
	}
	else
	{
		m_HighestUnit += 1;
		return ++m_CurrentUnit;
	}

}

unsigned int TextureManager::getCurrentUnit()
{
	return m_CurrentUnit;
}

// Currently resets every frame but could be more dynamically changed per texture
// Effectively a limit of m_MaxUnits textures although much more is possible
void TextureManager::reset()
{
	m_HighestUnit = 0;
	m_CurrentUnit = 0;
}
