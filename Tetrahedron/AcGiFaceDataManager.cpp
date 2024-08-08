#include "stdafx.h"
#include "AcGiFaceDataManager.h"
#include <utility>
AcGiFaceDataManager::AcGiFaceDataManager() : m_pFaceData(std::make_unique<AcGiFaceData>())
{
}
const short* AcGiFaceDataManager::colors() const noexcept
{
	return m_pColors.data();
}
void AcGiFaceDataManager::setColors(std::vector<short> aColors)
{
	m_pColors = std::move(aColors);
	m_pFaceData->setColors(m_pColors.data());
}

void AcGiFaceDataManager::setColor(size_t aI, short anColor)
{
	m_pColors.at(aI) = anColor;
}

AcGiFaceData* AcGiFaceDataManager::faceData() const noexcept
{
	return m_pFaceData.get();
}
