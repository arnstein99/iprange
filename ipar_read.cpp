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
    cerr << iplist.num_operations() << " operations applied, ";
    auto numLines = iplist.num_output();
    if (style == IPAR::Shex)
    {
	iplist.verify();
	cout << hex << setfill('0');
        for (auto iter = iplist.cbegin() ; iter != iplist.cend() ; ++iter)
	{
	    uint32_t lower = iter->first;
	    while (lower <= iter->second)
	    {
		cout << setw(8) << lower++ << endl;
	    }
	}
    }
    else
    {
	// TODO: write a manipulator for iplist.
	iplist.print(cout, (style == IPAR::Sdashes));
    }
    cerr << iplist.num_output() - numLines << " lines output" << endl;

    return 0;
}
