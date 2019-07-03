// Program ipar_read
// ----------------
// Reads a list of IP address ranges from standard input.
// Writes out equivalent list of ranges in one of three formats, to 
// standard output:
//  * standard form.
//  * intervals, with dashes.
//  * individual 32-bit numbers, in hex format.
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

    IPAR::List iplist;
    if (int retval = IPAR::common_read (cin, iplist) != 0) return retval;

    // Report
    cerr << iplist.num_operations() << " operations applied" << endl;
    if (style == Shex)
    {
	iplist.verify();
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

    return 0;
}
