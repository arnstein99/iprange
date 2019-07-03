// Program ipar_read
// ----------------
// Reads IP address ranges from standard input.
// Performs the following, until EOF on input:
//  * Prompt for input (> ).
//  * Read in a line of input.
//  * Write out an equivalent list of ranges in one of three formats, to 
//    standard output:
//    * standard form.
//    * intervals, with dashes.
//    * individual 32-bit numbers, in hex format.
// The last format is useful for testing.

#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
using namespace std;
#include "ipar_iplist.h"
#include "ipar_common.h"

int main (int argc, char* argv[])
{
    enum Style {
        Scidr,
	Sdashes,
	Shex
    };
    Style style;

    // Process arguments
    switch (argc)
    {
    case 1:
	style = Scidr;
        break;
    case 2:
        if (strcmp (argv[1], "-cidr") == 0)
	{
	    style = Scidr;
	}
        else if (strcmp (argv[1], "-dashes") == 0)
	{
	    style = Sdashes;
	}
        else if (strcmp (argv[1], "-hex") == 0)
	{
	    style = Shex;
	}
	else
	{
	    cerr << "Usage: ipar_read [-cidr|-dashes|-hex]" << endl;
	    cerr << "(no other arguments)" << endl;
	    return 1;
	}
        break;
    default:
        cerr << "Usage: ipar_read [-cidr|-dashes|-hex]" << endl;
	cerr << "(no other arguments)" << endl;
	return 1;
    }

    string line;
    cout << "> " << flush;
    while (getline(cin, line))
    {
	// Process one line of input
	string word;
	istringstream sstr(line);
	IPAR::List iplist;
	while (sstr >> word)
	{
	    // Ignore comments to end of line
	    if (word[0] == '#')
	    {
		word = "";
		break;
	    }
	    IPAR::Range iprange;
	    try {
		iprange = IPAR::Range(word);
	    }
	    catch (const exception& ex) {
		cerr << "ERROR: could not parse \"" << word << "\"" << endl;
	        break;
	    }
	    iplist.add (iprange);
	}

	// Report from one line of input
	if (style == Shex)
	{
	    cout << hex << setfill('0');
	    for (auto pr : iplist.get())
	    {
		uint32_t lower = pr.first;
		while (lower <= pr.second)
		{
		    cout << setw(8) << lower++ << endl;
		}
	    }
	}
	else
	{
	    if (style == Sdashes)
	    {
		// TODO: write a manipulator for iplist.
		iplist.print(cout, true);
	    }
	    else // CIDR format
	    {
		cout << iplist;
	    }
	}
	cout << "> " << flush;
    }

    // Left over from last prompt
    cout << endl;

    return 0;
}
