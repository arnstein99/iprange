#include <exception>
#include <iostream>
#include <istream>
#include "ipar_common.h"

namespace IPAR {


OutputStyle o_style (const std::string& arg)
{
    OutputStyle style;

    if (arg == "-cidr")
    {
	style = Scidr;
    }
    else if (arg == "-dashes")
    {
	style = Sdashes;
    }
    else if (arg == "-hex")
    {
	style = Shex;
    }
    else
    {
	style = Sunknown;
    }
    return style;
}

//////////////////////////////////////
// Implementation of TextReader class
//////////////////////////////////////

TextReader::TextReader(std::istream& ist)
 : mIst(ist), mSst{}, mLine{}, mLineNo(0), mLineOk(false), mFileOk(ist)
{
}

TextReader& TextReader::operator>> (std::string& word)
{
    while (mFileOk)
    {
	if (!mLineOk)
	{
	    if (!(mFileOk = bool(getline(mIst, mLine)))) break;
	    mSst.str(mLine);
	    mSst.clear();
	    ++mLineNo;
	}

	if (!(mLineOk = bool(mSst >> word))) continue;

	// Ignore comments to end of line
	if (word[0] == '#')
	{
	    mLineOk = false;
	    word = "";
	    continue;
	}

	// Testing complete. Return last word read.
	break;
    }
    return *this;
}

TextReader::operator bool() const
{
    return mFileOk;
}

std::string TextReader::current_line() const
{
    return mLine;
}

unsigned int TextReader::line_no() const
{
    return mLineNo;
}


//////////////////////////////////////
// Implementation of FileReader class
//////////////////////////////////////

FileReader::FileReader(const std::string& filename)
 : mIfs(filename, std::ifstream::in), mTr(mIfs)
{
    if (!mIfs)
    {
        std::cerr << "ERROR: could not open input file \"" << filename
                  << "\" for reading" << std::endl;
    }
}


///////////////////////////////////////////
// Implementation of stand-alone functions
///////////////////////////////////////////

int common_read (std::istream& ist, List& iplist)
{
    // Loop over lines of input
    IPAR::TextReader reader(ist);
    std::string word;
    IPAR::Range iprange;
    while (reader >> word)
    {
	// Assume the word is a range of IPv4 addresses
	try {
	    iprange = IPAR::Range(word);
	}
	catch (const std::exception& ex) {
	    std::cerr << "ERROR: " << ex.what() << " at line "
                      << reader.line_no() << " of input: " << std::endl;
	    std::cerr << reader.current_line() << std::endl;
	    std::cerr << "Last input was \"" << word << "\"" << std::endl;
	    return 1;
	}
        iplist.add(iprange);
    }

    return 0;
}

} // namespace IPAR
