//
// C++ Encoder/Decoder
//
// Audio Interchange File Format
// 
#ifndef AIFF_FILE_H
#define AIFF_FILE_H

#include "bctypes.h"

#include <string>
#include <vector>

#pragma pack(push, 1)

struct AIFF_CHUNK
{
	char c0,c1,c2,c3;
	unsigned int length;

    void Read(unsigned char* pData)
	{
		c0 = pData[0];
		c1 = pData[1];
		c2 = pData[2];
		c3 = pData[3];

		unsigned char* pLen = (unsigned char*)&length;

		pLen[3] = pData[4];
		pLen[2] = pData[5];
		pLen[1] = pData[6];
		pLen[0] = pData[7];
	}

	bool Is(const char* NAME)
	{
		return (c0 == NAME[0]) && (c1 == NAME[1]) && (c2 == NAME[2]) && (c3 == NAME[3]);
	}
};

struct AIFF_MARKER
{
	u16 id;
	u32 position;
	std::string markerName;
};

struct AIFF_LOOP
{
	u16 playMode;
	u16 beginLoop;
	u16 endLoop;
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
	// COMM
	u16 numChannels;
	u32 numSampleFrames;
	u16 sampleSize;
	f64 sampleRate;
	// INST
	u8 baseNote;
	u8 detune;
	u8 lowNote;
	u8 highNote;
	u8 lowVelocity;
	u8 highVelocity;
	u16 gain;
	AIFF_LOOP sustainLoop;
	AIFF_LOOP releaseLoop;

	std::vector<u16> m_SampleData;

	std::vector<AIFF_MARKER> m_Markers;

};


#endif // AIFF_FILE_H


