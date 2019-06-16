// Program ipar_gap_analyzer
// -------------------------
// Reads a list of IP address ranges from standard input.
// Compiles a sorted list of intervals that are dense.
// ...

#include <iostream>
#include <string>
#include <set>
#include <cstdint>
#include <limits>
using namespace std;
#include "ipar_iplist.h"
#include "ipar_common.h"

////////////
// Tuning //
////////////

// Stop searching for gaps after this distance. Speeds up the program
// tremendously.
static const uint32_t max_search = 0x00FFFFFF;

// Info about a range of IP addresses
struct GapInfo
{
    std::pair<uint32_t,uint32_t> mOriginalRange;
    uint32_t mExpandedUpper;
    uint32_t mNumCovered;
    double mScore;
};
bool operator< (const GapInfo& left, const GapInfo& right)
    { return (left.mScore < right.mScore); }
using GapInfoSet = std::set<GapInfo>;

// Compute score of a range wrt an IPAR list
std::pair<uint32_t,double>  coverage (
    uint32_t lower_bound, uint32_t upper_bound, IPAR::List& list);
// Note: there must exist a [lower_bound, ????] in the list.

// Find extension of interval with the most coverage
GapInfo max_coverage (
    uint32_t lower_bound, uint32_t upper_bound, IPAR::List& list);
// Note: [lower_bound, upper_bound] must belong to the list.

// Compare one member of the input data list with another one, on the right.
void my_process(
    IPAR::List& iplist, GapInfoSet& gset,
    uint32_t lower_bound, uint32_t upper_bound,
    double& best_score, uint32_t& best_upper,
    uint32_t other_lower_bound, uint32_t other_upper_bound);
// Note: [lower_bound, upper_bound] must belong to the list.

// Process one member of the input data list
void my_process(
    IPAR::List& iplist, GapInfoSet& gset,
    uint32_t lower_bound, uint32_t upper_bound);

// Process all members of the input data list
void find_gaps(IPAR::List& iplist);

int main (int argc, char* /*argv*/[])
{
    // No arguments allowed
    if (argc != 1)
    {
        cerr << "Usage: ipar_gap_analyzer" << endl;
	cerr << "(no arguments)" << endl;
	return 1;
    }

    IPAR::List iplist;

    // Loop over lines of input
    if (int retval = IPAR::common_read (cin, iplist) != 0) return retval;

    // Analyze and report
    find_gaps(iplist);

    return 0;
}

// Process all members of the input data list
void find_gaps(IPAR::List& iplist)
{
    GapInfoSet gset;

    iplist.process(
        [&iplist, &gset] (uint32_t lower_bound, uint32_t upper_bound) {
	    // Debug code
            cerr << '.' << flush;
	    my_process(iplist, gset, lower_bound, upper_bound);
	}
    );

    // Report
    for (auto entry : gset)
    {
        cout << "["         << IPAR::int_to_quad(entry.mOriginalRange.first)
	     << " "         << IPAR::int_to_quad(entry.mOriginalRange.second)
	     << "] --> [- " << IPAR::int_to_quad(entry.mExpandedUpper)
	     << "] : "      << entry.mNumCovered <<
	     "" " i, "       << static_cast<int>((entry.mScore * 100) + 0.5)
	     << "% of "     << 
	         entry.mExpandedUpper - entry.mOriginalRange.first + 1
             << endl;
    }
}

// Process one member of the input data list
void my_process(
    IPAR::List& iplist, GapInfoSet& gset,
    uint32_t lower_bound, uint32_t upper_bound)
// Note: [lower_bound, upper_bound] must belong to the list.
{
    GapInfo best = max_coverage (lower_bound, upper_bound, iplist);
    gset.insert(best);
}

GapInfo max_coverage (
    uint32_t lower_bound, uint32_t upper_bound, IPAR::List& iplist)
{
    GapInfo best {
        make_pair(lower_bound, upper_bound),
        upper_bound,
	0,
	0.0
    };

    // Set a limit of lower_bound + max_search, being careful of numeric
    // overflow.
    uint32_t bmax = std::numeric_limits<uint32_t>::max();
    uint32_t my_max = iplist.max();
    // if (lower_bound + max_search > bmax)
    if (bmax - lower_bound < max_search) 
    {
        // my_max unchanged.
    }
    else
    {
	uint32_t test_val = lower_bound + max_search;
	if (test_val < my_max)
	    my_max = test_val;
    }

    iplist.process(
        lower_bound,
	my_max,
        [lower_bound, &iplist, &best] (
	    uint32_t other_lower_bound, uint32_t other_upper_bound)
	{
	    // Skip own self
	    if (other_lower_bound == lower_bound) return;

	    // Compute a score for this candidate
	    auto cov = coverage (lower_bound, other_upper_bound, iplist);
	    if (cov.second > best.mScore)
	    {
		best.mExpandedUpper = other_upper_bound;
		best.mNumCovered = cov.first;
		best.mScore = cov.second;
	    }
	}
    );

    return best;
}

// Compute score of a range wrt an IPAR list
std::pair<uint32_t,double>  coverage (
    uint32_t lower_bound, uint32_t upper_bound, IPAR::List& iplist)
// Note: there must exist a [lower_bound, ????] in the list.
{
    std::pair<uint32_t,double> cov {0, 0.0};

    iplist.process(
        lower_bound,
	upper_bound,
        [&cov, lower_bound, upper_bound] (
	    uint32_t other_lower_bound, uint32_t other_upper_bound)
	{
	    uint32_t inter_lower_bound, inter_upper_bound;
	    if (!IPAR::intersect (lower_bound, upper_bound,
	                         other_lower_bound, other_upper_bound,
				 inter_lower_bound, inter_upper_bound))
	    throw (std::exception());
	    ++cov.first;
	    cov.second += (inter_upper_bound - inter_lower_bound + 1);
	}
    );
    cov.second = cov.second / (upper_bound - lower_bound + 1);
    return cov;
}
