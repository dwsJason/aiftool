//
// AIFF - AIFF Parse Tool
// 

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "aiff_file.h"

//------------------------------------------------------------------------------
static void helpText()
{
	printf("aifTool - v0.1\n");
	printf("--------------\n");
	printf("Audio Tool, to pre-process JMixer wave files\n");
	printf("\n");
	printf("\naiftool [options] <input_file> <outfile>\n\n");
	printf("options:\n\n");
	// Audio Interchange File Format
	printf("Converts from AIF to JMX\n");

	exit(-1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local helper functions

static bool contains(char x, const char* pSeparators)
{
	while (*pSeparators)
	{
		if (x == *pSeparators)
		{
			return true;
		}
		pSeparators++;
	}

	return false;
}

static std::vector<std::string> split(const std::string& s, const char* separators)
{
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;

	for (int index = 0; index < s.length(); ++index)
	{
		pos = index;

		// if the index is a separator
		if (contains(s[index], separators))
		{
			// if we've skipped a token, collect it
			if (prev_pos != index)
			{
				output.push_back(s.substr(prev_pos, index-prev_pos));

				// skip white space here
				while (index < s.length())
				{
					if (contains(s[index], separators))
					{
						++index;
					}
					else
					{
						prev_pos = index;
						pos = index;
						break;
					}
				}
			}
			else
			{
				prev_pos++;
			}
		}
	}

    output.push_back(s.substr(prev_pos, pos-prev_pos+1)); // Last word

    return output;
}

static std::string toLower(const std::string s)
{
	std::string result = s;

	for (int idx = 0; idx < result.size(); ++idx)
	{
		result[ idx ] = (char)tolower(result[idx]);
	}

	return result;
}

// Case Insensitive
static bool endsWith(const std::string& S, const std::string& SUFFIX)
{
	bool bResult = false;

	std::string s = toLower(S);
	std::string suffix = toLower(SUFFIX);

    bResult = s.rfind(suffix) == (s.size()-suffix.size());

	return bResult;
}
//------------------------------------------------------------------------------


int main(int argc, char* argv[])
{
	char* pInfilePath  = nullptr;
	char* pOutfilePath = nullptr;

	if (argc < 2) helpText();

	for (int idx = 1; idx < argc; ++idx )
	{
		char* arg = argv[ idx ];

		if ('-' == arg[0])
		{
			// Parse as an option
		}
		else if (nullptr == pInfilePath)
		{
			// Assume the first non-option is an input file path
			pInfilePath = argv[ idx ];
		}
		else if (nullptr == pOutfilePath)
		{
			// Assume second non-option is an output file path
			pOutfilePath = argv[ idx ];
		}
		else
		{
			// Oh Crap, we have a non-option, but we don't know what to do with
			// it
			printf("ERROR: Invalid option, Arg %d = %s\n\n", idx, argv[ idx ]);
			helpText();
		}
	}

	if (pInfilePath)
	{
		// See what we can do with the input file path
		// could be a .gsla file, for a .c2 file, or maybe a series of .c1 files
		if (endsWith(pInfilePath, ".aif") || endsWith(pInfilePath, ".aiff"))
		{
			// It's an AIFF file, probably

			printf("Loading AIFF File %s\n", pInfilePath);

			AIFFFile aifdata( pInfilePath );


			if (pOutfilePath)
			{
			}
		}

	}
	else
	{
		helpText();
	}


	return 0;
}

