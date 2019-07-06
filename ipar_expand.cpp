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

int main (int argc, char* argv[])
{
    // Process arguments
    IPAR::OutputStyle style;
    switch (argc)
    {
    case 1:
	style = IPAR::Scidr;
        break;
    case 2:
        style = IPAR::o_style(argv[1]);
	if (style == IPAR::Sunknown)
	{
	    cerr << "Usage: ipar_expand [-cidr|-dashes|-hex]" << endl;
	    cerr << "(no other arguments)" << endl;
	    return 1;
	}
        break;
    default:
        cerr << "Usage: ipar_expand [-cidr|-dashes|-hex]" << endl;
	cerr << "(no other arguments)" << endl;
	return 1;
    }

    // Choose hex output if necessary
    if (style == IPAR::Shex) cout << hex << setfill('0');

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
	    cerr << "ERROR: " << ex.what() << endl;
	    cerr << "Last input was \"" << word << "\"" << endl;
	    return 1;
	}
	// Expand the range
	auto lower = iprange.get().first;
	auto upper = iprange.get().second;
	while (lower <= upper)
	{
	    if (style == IPAR::Shex)
		cout << setw(8) << lower++ << endl;
	    else
		cout << IPAR::int_to_quad(lower++) << endl;
	}
    }

    return 0;
}
