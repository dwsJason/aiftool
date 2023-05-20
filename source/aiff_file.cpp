//
// C++ Encoder/Decoder
// other remaining codes, are reserved for future expansion
//
#include "aiff_file.h"
#include "bctypes.h"
#include <stdio.h>
#include <cmath>

//------------------------------------------------------------------------------
// Load in a FanFile constructor
//
AIFFFile::AIFFFile(const char *pFilePath)
{
	// COMM
	numChannels = 0;
	numSampleFrames = 0;
	sampleSize = 0;
	sampleRate = 0;
	// INST
	baseNote = 0;
	detune = 0;
	lowNote = 0;
	highNote = 0;
	lowVelocity = 0;
	highVelocity = 0;
	gain = 0;

	sustainLoop.playMode  = 0;
	sustainLoop.beginLoop = 0;
	sustainLoop.endLoop   = 0;

	releaseLoop.playMode  = 0;
	releaseLoop.beginLoop = 0;
	releaseLoop.endLoop   = 0;

	LoadFromFile(pFilePath);
}

//------------------------------------------------------------------------------

AIFFFile::~AIFFFile()
{
	// Free Up the memory
}

//------------------------------------------------------------------------------
// Big Endian Helpers
static std::string ReadPSTR(std::vector<unsigned char>& bytes, size_t& file_offset)
{
	std::string result;
	u8 len = bytes[file_offset++];

	while (len)
	{
		result.push_back(bytes[file_offset++]);
		len--;
	}

	// keep offset even
	if (file_offset & 0x1)
	{
		file_offset+=1;
	}

	return result;
}

static u32 Readu32(std::vector<unsigned char>& bytes, size_t& file_offset)
{
	u32 result;

	u8* pResult = (u8*)&result;

	pResult[3] = bytes[file_offset+0];
	pResult[2] = bytes[file_offset+1];
	pResult[1] = bytes[file_offset+2];
	pResult[0] = bytes[file_offset+3];

	file_offset+=4;

	return result;
}

static u16 Readu16(std::vector<unsigned char>& bytes, size_t& file_offset)
{
	u16 result;

	u8* pResult = (u8*)&result;

	pResult[1] = bytes[file_offset+0];
	pResult[0] = bytes[file_offset+1];

	file_offset+=2;

	return result;
}

static f64 Readf80(std::vector<unsigned char>& bytes, size_t& file_offset)
{
	// f32 is 1:8
	// f64 is 1:11
	// f80 is 1:15
	f64 result;

    //80 bit floating point value according to the IEEE-754 specification and the Standard Apple Numeric Environment specification:
    //1 bit sign, 15 bit exponent, 1 bit normalization indication, 63 bit mantissa

    f64 sign;
    if ((bytes[file_offset] & 0x80) == 0x00)
        sign = 1;
    else
        sign = -1;
    u32 exponent = (((u32)bytes[file_offset] & 0x7F) << 8) | (u32)bytes[file_offset + 1];
    u64 mantissa= 0;// = readUInt64BE(buffer, offset + 2);
	u8* pMantissa = (u8*) &mantissa;

	pMantissa[7] = bytes[file_offset+2];
	pMantissa[6] = bytes[file_offset+3];
	pMantissa[5] = bytes[file_offset+4];
	pMantissa[4] = bytes[file_offset+5];
	pMantissa[3] = bytes[file_offset+6];
	pMantissa[2] = bytes[file_offset+7];
	pMantissa[1] = bytes[file_offset+8];
	pMantissa[0] = bytes[file_offset+9];


    //If the highest bit of the mantissa is set, then this is a normalized number.
    f64 normalizeCorrection;
    if ((mantissa & 0x8000000000000000) != 0x00)
        normalizeCorrection = 1;
    else
        normalizeCorrection = 0;
    mantissa &= 0x7FFFFFFFFFFFFFFF;

    file_offset += 10;

    //value = (-1) ^ s * (normalizeCorrection + m / 2 ^ 63) * 2 ^ (e - 16383)
    result = (sign * (normalizeCorrection + (f64)mantissa / ((u64)1 << 63)) * pow(2, (i32)exponent - 16383));

	return result;
}



//------------------------------------------------------------------------------

void AIFFFile::LoadFromFile(const char* pFilePath)
{
	std::vector<unsigned char> bytes;

	//--------------------------------------------------------------------------
	// Read the file into memory
	FILE* pFile = nullptr;
	errno_t err = fopen_s(&pFile, pFilePath, "rb");

	if (0==err)
	{
		fseek(pFile, 0, SEEK_END);
		size_t length = ftell(pFile);	// get file size
		fseek(pFile, 0, SEEK_SET);

		bytes.resize( length );			// make sure buffer is large enough

		// Read in the file
		fread(&bytes[0], sizeof(unsigned char), bytes.size(), pFile);
		fclose(pFile);
	}

	if (bytes.size())
	{
		size_t file_offset = 0;	// File Cursor

		// Check for FORM
		AIFF_CHUNK tempChunk;

		tempChunk.Read(&bytes[file_offset]);
		file_offset+=8;

		if (tempChunk.Is("FORM"))
		{
			tempChunk.Read(&bytes[file_offset]);
			file_offset+=4;

			if (tempChunk.Is("AIFF"))
			{
				// While we're not at the end of the file
				while (file_offset < bytes.size())
				{
					tempChunk.Read(&bytes[file_offset]);
					file_offset+=8;

					if (tempChunk.Is("COMM"))
					{
						numChannels     = Readu16(bytes, file_offset);
						numSampleFrames = Readu32(bytes, file_offset);
						sampleSize      = Readu16(bytes, file_offset);
						sampleRate      = Readf80(bytes, file_offset);

						printf("COMM CHUNK, Length=%d\n", tempChunk.length);

						printf("     numChannels     = %d\n", numChannels);
						printf("     numSampleFrames = %d\n", numSampleFrames);
						printf("     sampleSize      = %d\n", sampleSize);
						printf("          sampleRate = %lf\n", sampleRate);

					}
					else if (tempChunk.Is("SSND"))
					{
						//printf("Skipping %c%c%c%c CHUNK, Length=%d\n",
						//	  tempChunk.c0, tempChunk.c1,
						//	  tempChunk.c2, tempChunk.c3,
						//	  tempChunk.length );

						u32 offset    = Readu32(bytes,file_offset);
						u32 blockSize = Readu32(bytes,file_offset);

						printf("SSND CHUNK, Length = %d\n", tempChunk.length);
						printf("     offset = %d\n", offset);
						printf("  blockSize = %d\n", blockSize);

						if ((1 == numChannels)&&(16 == sampleSize))
						{
							for (unsigned int idx = 0; idx < numSampleFrames; ++idx)
							{
								m_SampleData.push_back(Readu16(bytes,file_offset));
							}
						}
						else
						{
							printf("UNSUPPORTED numChannels && sampleSize\n");
							file_offset+=tempChunk.length-8;
						}

					}
					else if (tempChunk.Is("MARK"))
					{
						u32 numMarkers = Readu16(bytes,file_offset);

						printf("MARK CHUNK, Length = %d\n", tempChunk.length);
						printf("        numMarkers = %d\n", numMarkers);

						for (unsigned int idx = 0; idx < numMarkers; ++idx)
						{
							AIFF_MARKER marker;

							marker.id = Readu16(bytes,file_offset);
							marker.position = Readu32(bytes,file_offset);
							marker.markerName = ReadPSTR(bytes,file_offset);

							printf("     id = %04x\n", marker.id);
							printf("    pos = %d\n", marker.position);
							printf("   name = %s\n", marker.markerName.c_str());

							m_Markers.push_back(marker);
						}

					}
					else if (tempChunk.Is("INST"))
					{
						printf("INST CHUNK, Length = %d\n", tempChunk.length);
						
						baseNote     = bytes[file_offset++];
						detune       = bytes[file_offset++];
						lowNote      = bytes[file_offset++];
						highNote     = bytes[file_offset++];
						lowVelocity  = bytes[file_offset++];
						highVelocity = bytes[file_offset++];
						gain        = Readu16(bytes,file_offset);
						sustainLoop.playMode  = Readu16(bytes,file_offset);
						sustainLoop.beginLoop = Readu16(bytes,file_offset);
						sustainLoop.endLoop   = Readu16(bytes,file_offset);

						releaseLoop.playMode  = Readu16(bytes,file_offset);
						releaseLoop.beginLoop = Readu16(bytes,file_offset);
						releaseLoop.endLoop   = Readu16(bytes,file_offset);

						printf("     baseNote=%02x\n", baseNote);
						printf("       detune=%02x\n", detune);
						printf("      lowNote=%02x\n", lowNote);
						printf("     highNote=%02x\n", highNote);
						printf("  lowVelocity=%02x\n", lowVelocity);
						printf(" highVelocity=%02x\n", highVelocity);
						printf("         gain=%d\n", gain);
						printf("  sustainLoop.playMode =%04x\n", sustainLoop.playMode);
						printf("  sustainLoop.beginLoop=%04x\n", sustainLoop.beginLoop);
						printf("  sustainLoop.endLoop  =%04x\n", sustainLoop.endLoop);
						printf("  releaseLoop.playMode =%04x\n", releaseLoop.playMode);
						printf("  releaseLoop.beginLoop=%04x\n", releaseLoop.beginLoop);
						printf("  releaseLoop.endLoop  =%04x\n", releaseLoop.endLoop);

					}
					else
					{
						printf("Skipping %c%c%c%c CHUNK, Length=%d\n",
							  tempChunk.c0, tempChunk.c1,
							  tempChunk.c2, tempChunk.c3,
							  tempChunk.length );

						file_offset+=tempChunk.length;
					}
				}
			}
		}

	}
}

//------------------------------------------------------------------------------

AIFF_MARKER* AIFFFile::FindMarker(u16 id)
{
	AIFF_MARKER* result = nullptr;

	for (int idx = 0; idx < m_Markers.size(); ++idx)
	{
		if (m_Markers[idx].id == id)
		{
			result = &m_Markers[idx];
			break;
		}
	}

	return result;
}

//------------------------------------------------------------------------------

u32 AIFFFile::GetLoop()
{
	u32 result = (u32)m_SampleData.size()-1;

	AIFF_MARKER *pMarker = FindMarker(sustainLoop.beginLoop);

	if (pMarker)
	{
		result = pMarker->position;
	}

	return result;
}
//------------------------------------------------------------------------------

u32 AIFFFile::GetEnd()
{
	u32 result = (u32)m_SampleData.size()-1; // fallback

	AIFF_MARKER *pMarker = FindMarker(sustainLoop.endLoop);

	if (pMarker)
	{
		result = pMarker->position;
	}

	return result;

}

//------------------------------------------------------------------------------

