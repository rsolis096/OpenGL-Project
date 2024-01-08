#include "TextureManager.h"

unsigned int TextureManager::m_HighestUnit = 0;
unsigned int TextureManager::m_CurrentUnit = 0;

unsigned int TextureManager::getNextUnit()
{
	m_HighestUnit += 1;
	return ++m_CurrentUnit;
}

unsigned int TextureManager::getCurrentUnit()
{
	return m_CurrentUnit;
}
