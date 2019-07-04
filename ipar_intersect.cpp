// Program ipar_intersect
// ----------------------
// Reads a list of IP address ranges from standard input.
// Opens other lists of IP addresses from specified files.
// Logically intersects everything in the other lists with the first list.
// Writes out the result to standard output.
// None of the inputs have to be sorted.

#include <string>
#include <limits>
using namespace std;
#include "ipar_iplist.h"
#include "ipar_common.h"

int main (int argc, char* argv[])
{
    IPAR::List mainlist;

    // Loop over lines of input
    if (int retval = IPAR::common_read (cin, mainlist) != 0) return retval;

    // This will hold the complement of what follows
    IPAR::List complem;
    static const uint32_t bmin = numeric_limits<uint32_t>::min();
    static const uint32_t bmax = numeric_limits<uint32_t>::max();
    complem.add(IPAR::Range(bmin, bmax));

    // Loop over input files to intersect
    for (int iArg = 1 ; iArg < argc ; ++iArg)
    {
	IPAR::FileReader reader2(argv[iArg]);
	if (!reader2) return 1;

	// Loop over words in the intersect file
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
            complem.subtract(iprange);
	} // End loop over words in the intersect file
    } // End loop over input files to intersect

    // Now subtract from the main list
    for (auto range : complem.get())
    {
	mainlist.subtract (IPAR::Range(range.first, range.second));
    }

    // Report
    cerr << mainlist.num_operations() << " operations applied" << endl;
    cout << mainlist;

    return 0;
}
