//
// C++ Encoder/Decoder
// other remaining codes, are reserved for future expansion
//
#include "aiff_file.h"

#include <stdio.h>

//------------------------------------------------------------------------------
// Load in a FanFile constructor
//
AIFFFile::AIFFFile(const char *pFilePath)
{
	LoadFromFile(pFilePath);
}

//------------------------------------------------------------------------------

AIFFFile::~AIFFFile()
{
	// Free Up the memory
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



			// While we're not at the end of the file
			while (file_offset < bytes.size())
			{
			}
		}

	}
	#endif
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

