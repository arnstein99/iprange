// Program ipar_subtract
// ---------------------
// Reads a list of IP address ranges from standard input.
// Opens other lists of IP addresses from specified files.
// Logically removes everything in the other lists from the first list.
// Writes out the result to standard output.
// None of the inputs have to be sorted and they need not be in standard form.

#include <iostream>
#include <fstream>
#include <string>
using namespace std;
#include "ipar_iplist.h"
#include "ipar_common.h"

int main (int argc, char* argv[])
{
    IPAR::List mainlist;
    string word;

    // Loop over lines of input
    if (int retval = IPAR::common_read (cin, mainlist) != 0) return retval;

    // Loop over input files to subtract
    for (int iArg = 1 ; iArg < argc ; ++iArg)
    {
	ifstream ist ((argv[iArg]), ifstream::in);
	if (ist.fail())
	{
	    cerr << "ERROR: could not open subtract file \"" << argv[iArg]
		 << "\" for reading" << endl;
	    return 1;
	}

	// Loop over words in the subtract file
	IPAR::TextReader reader2(ist);
	while (reader2 >> word)
	{
	    // Assume the word is a range of IPv4 addresses
	    try {
		IPAR::Range iprange(word);
		mainlist.subtract(iprange);
	    }
	    catch (const exception& ex) {
		cerr << "ERROR: " << ex.what() << " at line "
		     << reader2.line_no()
		     << " of \"" << argv[iArg] << "\": " << endl;
		cerr << reader2.current_line() << endl;
		cerr << "Last input was \"" << word << "\"" << endl;
		return 1;
	    }
	} // End loop overwords in the subtract file
    } // End loop over input files to subtract

    // Report
    cerr << mainlist.num_operations() << " operations applied" << endl;
    cout << mainlist;

    return 0;
}
