////////////////////////////////////////////////////////
// Common logic for several programs are featured here.
////////////////////////////////////////////////////////

#ifndef IPAR_COMMON_H_ // {
#define IPAR_COMMON_H_

#include <istream>
#include "ipar_iplist.h"

namespace IPAR {

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
    ~TextReader();
    TextReader(TextReader const& other) = delete;
    TextReader& operator=(TextReader const& other) = delete;
    TextReader(TextReader&& other) = default;
    TextReader& operator=(TextReader&& other) = default;

    TextReader(std::istream& ist);
    TextReader& operator>> (std::string& word);
    operator bool() const;
    std::string current_line() const;
    unsigned int line_no() const;

private:

    std::istream& mIst;
    std::istringstream* mSst;
    std::string mLine;
    unsigned int mLineNo;
    bool mLineOk;
    bool mFileOk;

}; // class TextReader

// This method does a batch-read of interval specifiers from an
// istream into an IPAR list.
int common_read (std::istream& ist, List& iplist) throw();

} // namespace IPAR

#endif //  } IPAR_COMMON_H_
