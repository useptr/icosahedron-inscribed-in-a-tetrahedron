#pragma once
#include <memory>
/*!
	\brief ¬спомогательный класс, который следит за выделенными через new пол€ми дл€ AcGiFaceData
*/
class AcGiFaceDataManager
{
public:
	AcGiFaceDataManager();
	short* colors() const noexcept;
	void setColors(std::unique_ptr<short[]> aColors); // maybe use vector
	AcGiFaceData* faceData() const noexcept;
private:
	std::unique_ptr<AcGiFaceData> m_pFaceData;
	std::unique_ptr<short[]> m_pColors;
};

