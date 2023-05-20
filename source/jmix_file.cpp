//
// C++ File Handling Class for Jason's Mixer, Musical Instruments
// for the C256 FMX
//
#include "jmix_file.h"


// If these structs are the wrong size, there's an issue with type sizes, and
// your compiler
static_assert(sizeof(JMIX_Header)==24, "JMIX_Header is supposed to be 24 bytes");


JMIXFile::JMIXFile()
	: m_freq(22050)
	, m_note(60)
	, m_maxrate(8.0f)
	, m_loop_point(0)
	, m_end_point(0)
{
}

JMIXFile::~JMIXFile()
{
}

void JMIXFile::SetAttributes(u16 freq, u16 note, u32 loop, u32 end)
{
	m_freq = freq;
	m_note = note;
	m_loop_point = loop;
	m_end_point = end;
}

void JMIXFile::SetWaveData(const std::vector<u16>& WaveData)
{
	m_waveData = WaveData;
}

void JMIXFile::SaveToFile(const char* pFilenamePath)
{
	// serialize to memory, then save that to a file
	std::vector<unsigned char> bytes;

	// Add the header
	bytes.resize( bytes.size() + sizeof(JMIX_Header) );

	JMIX_Header* pHeader = (JMIX_Header*)&bytes[0];

	pHeader->J = 'J'; pHeader->M = 'M'; pHeader->I = 'I'; pHeader->X ='X';

	pHeader->file_length = 0; //temp file length

	pHeader->version = 0;  // version 0, the first version

	pHeader->freq = m_freq;
	pHeader->note = m_note;
	pHeader->maxrate = (u16)(256.0f * m_maxrate);
	pHeader->loop_point = m_loop_point * 2;
	pHeader->end_point = m_end_point * 2;

	size_t wave_offset = bytes.size();

	// Add Space for Wave Data
	bytes.resize( bytes.size() + (m_end_point*2) );

	// copy in the Wave Data
	memcpy(&bytes[wave_offset], &m_waveData[0], (m_end_point*2));
	 
	size_t end_offset = bytes.size();

	// Adjust size for the max play rate
	int overflow_samples = (int)((256.0f * m_maxrate));

	bytes.resize( bytes.size() + (overflow_samples*2));

	u32 sample_index = m_loop_point;

	// generate the overflow pattern
	for (int sample_count = 0; sample_count < overflow_samples; ++sample_count)
	{
		u16 sample = m_waveData[ sample_index ];
		((u16*)(&bytes[ end_offset + (sample_count*2)]))[0] = sample;

		sample_index++;
		if (sample_index > m_end_point)
		{
			sample_index = m_loop_point;
		}
	}

	// We need to massage the data little bit
	pHeader = (JMIX_Header*)&bytes[0]; // Required
	// Update Header Length
	pHeader->file_length = (unsigned int)bytes.size(); // get some valid data in there

	u16* pWave = (u16*) & bytes[wave_offset];

	int length = (overflow_samples + m_end_point); // total number of samples

	// Convert into JMIXER indices
	for (int idx = 0; idx < length; ++idx)
	{
		u16 sample = pWave[idx];
		sample>>=7;
		sample &= 0x1FE;
		pWave[idx] = sample;
	}

	//--------------------------------------------------------------------------
	// Create the file and write it
	FILE* pFile = nullptr;
	errno_t err = fopen_s(&pFile, pFilenamePath, "wb");

	if (0==err)
	{
		fwrite(&bytes[0], sizeof(unsigned char), bytes.size(), pFile);
		fclose(pFile);
	}
}

