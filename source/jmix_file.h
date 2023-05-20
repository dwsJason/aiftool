//
// C++ File Handling Class for Jason's Mixer, Musical Instruments
// for the C256 FMX
//
#ifndef JMIX_FILE_H
#define JMIX_FILE_H


#include "bctypes.h"
#include <vector>

#pragma pack(push, 1)

// Data Stream Header
typedef struct JMIX_Header
{
	char	J,M,I,X;
	unsigned int 	file_length;  // length of the file, including this header
	unsigned short  version;
	unsigned short  freq; 		  // frequency of recorded sample
	unsigned short  note;		  // note of recorded sample
	unsigned short  maxrate;      // probably 8.0 (0x0800)
	unsigned int    loop_point;   // 
	unsigned int    end_point;    // total number of frames in the file

	// for a maxrate of 8.0, there must be a 256 * 8 (2048) sample padding added
	// after the end_point, a maxrate of 10.0, would require 256 * 10 sample
	// padding (etc) where the pad data matches a repeating pattern that starts
	// at loop_point, and ends at end_point

//------------------------------------------------------------------------------
// If you're doing C, just get rid of these methods
	bool IsValid(unsigned int fileLength)
	{
		if ((J!='J')||(M!='M')||(I!='I')||(X!='X'))
			return false;				// signature is not right

		if (file_length != fileLength)
			return false;				// size isn't right

		return true;
	}

} JMIX_Header;

#pragma pack(pop)

class JMIXFile
{
public:
	JMIXFile();
	~JMIXFile();

	void SetWaveData(const std::vector<u16>& WaveData);
	void SetAttributes(u16 freq, u16 note, u32 loop, u32 end);
	void SaveToFile(const char* pFilenamePath);


private:

	u16 m_freq;
	u16 m_note;
	f32 m_maxrate;
	u32 m_loop_point;
	u32 m_end_point;

	std::vector<u16> m_waveData;

};


#endif // JMIX_FILE_H

