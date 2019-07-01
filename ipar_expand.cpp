// Program ipar_expand
// -------------------
// Reads a list of IP address ranges from standard input.
// Writes out equivalent list of individual IP addresses.
// No sorting or combining of intervals is performed.
// Useful for testing other programs.

#include <iostream>
#include <iomanip>
#include <string>
using namespace std;
#include "ipar_iplist.h"
#include "ipar_common.h"

int main (int argc, char* /*argv*/[])
{
    // No arguments allowed
    if (argc != 1)
    {
        cerr << "Usage: ipar_expand" << endl;
	cerr << "(no arguments)" << endl;
	return 1;
    }

    // Choose hex output
    cout << hex << setfill('0');
    cerr << hex << setfill('0');

    // Loop over lines of input
    IPAR::TextReader reader(cin);
    string word;
    IPAR::Range iprange;
    while (reader >> word)
    {
	// Assume the word is a range of IPv4 addresses
	try {
	    iprange = IPAR::Range(word);
	}
	catch (const exception& ex) {
	    cerr << "ERROR: " << ex.what() << " at line " << reader.line_no()
		 << " of input: " << endl;
	    cerr << reader.current_line() << endl;
	    cerr << "Last input was \"" << word << "\"" << endl;
	    return 1;
	}
	// Expand the range
	auto lower = iprange.get().first;
	auto upper = iprange.get().second;
	while (lower <= upper)
	{
	    cout << setw(8) << lower++ << endl;
	}
    }

    return 0;
}
