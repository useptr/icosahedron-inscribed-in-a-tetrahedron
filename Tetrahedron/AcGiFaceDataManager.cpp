#include "stdafx.h"
#include "AcGiFaceDataManager.h"
#include <utility>
AcGiFaceDataManager::AcGiFaceDataManager() : m_pFaceData(std::make_unique<AcGiFaceData>())
{
}
short* AcGiFaceDataManager::colors() const noexcept
{
	return m_pColors.get();
}
void AcGiFaceDataManager::setColors(std::unique_ptr<short[]> aColors)
{
	m_pColors = std::move(aColors);
	m_pFaceData->setColors(m_pColors.get());
}

AcGiFaceData* AcGiFaceDataManager::faceData() const noexcept
{
	return m_pFaceData.get();
}
