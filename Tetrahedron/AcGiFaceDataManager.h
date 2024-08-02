#pragma once
#include <memory>
class AcGiFaceDataManager
{
public:
	AcGiFaceDataManager();
	short* colors() const noexcept;
	void setColors(std::unique_ptr<short[]> aColors); // or use vector
	//void setTransparency(std::unique_ptr<AcCmTransparency[]> aTransparency);
	AcGiFaceData* faceData() const noexcept;
private:
	std::unique_ptr<AcGiFaceData> m_pFaceData;
	//std::unique_ptr<AcCmTransparency[]> m_pTransparency;
	std::unique_ptr<short[]> m_pColors;
};

