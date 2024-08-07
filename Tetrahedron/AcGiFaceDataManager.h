#pragma once
#include <memory>
#include <vector>
/*!
	\brief ¬спомогательный класс, который следит за выделенными через new пол€ми дл€ AcGiFaceData
*/
class AcGiFaceDataManager
{
public:
	AcGiFaceDataManager();
	const short* colors() const noexcept;
	void setColors(std::vector<short> aColors);
	void setColor(size_t aI, short anColor);
	AcGiFaceData* faceData() const noexcept;
private:
	std::unique_ptr<AcGiFaceData> m_pFaceData;
	std::vector<short> m_pColors;
};

