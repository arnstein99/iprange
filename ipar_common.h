////////////////////////////////////////////////////////
// Common logic for several programs are featured here.
////////////////////////////////////////////////////////

#ifndef IPAR_COMMON_H_ // {
#define IPAR_COMMON_H_

#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include "ipar_iplist.h"

namespace IPAR {

// Handle user request for output format
enum OutputStyle {
    Sunknown,
    Scidr,
    Sdashes,
    Shex
};
OutputStyle o_style (const std::string& arg);

// This class handles text input in a common style for the IPAR programs:
//  * All text from character # to end of line is ignored.
//  * Text is broken down into a stream of words, delimited by spaces.
//  * Current line number can be queried.
//  * Full content of current line of text can be queried.
class TextReader
{
public:

    // The automatic methods
    TextReader() = delete;
    ~TextReader() = default;
    TextReader(TextReader const& other) = delete;
    TextReader& operator=(TextReader const& other) = delete;
    TextReader(TextReader&& other) = default;
    TextReader& operator=(TextReader&& other) = delete;

    TextReader(std::istream& ist);
    TextReader& operator>> (std::string& word);
    operator bool() const;
    std::string current_line() const;
    unsigned int line_no() const;

private:

    std::istream& mIst;
    std::istringstream mSst;
    std::string mLine;
    unsigned int mLineNo;
    bool mLineOk;
    bool mFileOk;

}; // class TextReader

class FileReader
{
public:

    // The automatic methods
    FileReader() = delete;
    ~FileReader() = default;
    FileReader(FileReader const& other) = delete;
    FileReader& operator=(FileReader const& other) = delete;
    FileReader(FileReader&& other) = default;
    FileReader& operator=(FileReader&& other) = delete;

    FileReader(const std::string& filename);
    FileReader& operator>> (std::string& word) { mTr >> word; return *this; }
    operator bool() const { return bool(mTr); }
    std::string current_line() const { return mTr.current_line(); }
    unsigned int line_no() const { return mTr.line_no(); }

private:

    std::ifstream mIfs;
    TextReader mTr;

};

// This method does a batch-read of interval specifiers from an
// istream into an IPAR list.
int common_read (std::istream& ist, List& iplist);

} // namespace IPAR

#endif //  } IPAR_COMMON_H_
