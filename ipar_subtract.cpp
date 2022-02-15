// Program ipar_subtract
// ---------------------
// Reads a list of IP address ranges from standard input.
// Opens other lists of IP addresses from specified files.
// Logically removes everything in the other lists from the first list.
// Writes out the result to standard output.
// None of the inputs have to be sorted.

#include <string>
using namespace std;
#include "ipar_iplist.h"
#include "ipar_common.h"

int main (int argc, char* argv[])
{
    IPAR::List mainlist;

    // Loop over lines of input
    if (int retval = IPAR::common_read (cin, mainlist) != 0) return retval;

    // Loop over input files to subtract
    for (int iArg = 1 ; iArg < argc ; ++iArg)
    {
	IPAR::FileReader reader2(argv[iArg]);
	if (!reader2) return 1;

	// Loop over words in the subtract file
        string word;
	while (reader2 >> word)
	{
	    // Assume the word is a range of IPv4 addresses
            IPAR::Range iprange;
	    try {
		iprange = IPAR::Range(word);
	    }
	    catch (const exception& ex) {
		cerr << "ERROR: " << ex.what() << " at line "
		     << reader2.line_no()
		     << " of \"" << argv[iArg] << "\": " << endl;
		cerr << reader2.current_line() << endl;
		cerr << "Last input was \"" << word << "\"" << endl;
		return 1;
	    }
            mainlist.subtract(iprange);
	} // End loop overwords in the subtract file
    } // End loop over input files to subtract

    // Report
    auto numLines = mainlist.num_output();
    cerr << mainlist.num_operations() << " operations applied, ";
    cout << mainlist;
    cerr << mainlist.num_output() - numLines << " lines output" << endl;

    return 0;
}
