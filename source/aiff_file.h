//
// C++ Encoder/Decoder
//
// Audio Interchange File Format
// 
#ifndef AIFF_FILE_H
#define AIFF_FILE_H

#include <vector>

#pragma pack(push, 1)

struct AIFF_CHUNK
{
	char c0,c1,c2,c3;
	u32  length;

    void Read(u8* pData)
	{
		c0 = pData[0];
		c1 = pData[1];
		c2 = pData[2];
		c3 = pData[3];

		u8 *pLen = (u8*)&length;

		pLen[3] = pData[4];
		pLen[2] = pData[5];
		pLen[1] = pData[6];
		pLen[0] = pData[7];
	}

	bool Is(char* NAME)
	{
		return (c0 == NAME[0]) && (c1 == NAME[1]) && (c2 == NAME[2]) && (c3 == NAME[3]);
	}
};

struct AIFF_FORM
{
	char c0,c1,c2,c3,c4,c5,c6,c7;
	u32  length;

    void Read(u8* pData)
	{
		c0 = pData[0];
		c1 = pData[1];
		c2 = pData[2];
		c3 = pData[3];
		c4 = pData[4];
		c5 = pData[5];
		c6 = pData[6];
		c7 = pData[7];

		u8 *pLen = (u8*)&length;

		pLen[3] = pData[8];
		pLen[2] = pData[9];
		pLen[1] = pData[10];
		pLen[0] = pData[11];
	}

	bool Is(char* NAME)
	{
		return (c0 == NAME[0]) && (c1 == NAME[1]) && (c2 == NAME[2]) && (c3 == NAME[3]) &&
			   (c0 == NAME[4]) && (c1 == NAME[5]) && (c2 == NAME[6]) && (c3 == NAME[7]);

	}
};


#pragma pack(pop)

class AIFFFile
{
public:
	// Load in a GSLA File
	AIFFFile(const char *pFilePath);
	~AIFFFile();

	// Retrieval
	void LoadFromFile(const char* pFilePath);

private:

	u32 m_LoopStart;
	u32 m_LoopEnd;

std::vector<u16> m_SampleData;

};


#endif // AIFF_FILE_H


