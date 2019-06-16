#include <iostream>
#include <limits>
#include "ipar_iplist.h"
#include "ipar_numlist.h"

namespace IPAR
{

namespace { // anonymous

    uint32_t string_to_int(const std::string& expr) throw (ip_domain_error)
    {
        uint32_t retval = 0;
	std::string sub(expr);
	while (sub != "")
	{
	    char digit = sub[0];
	    int idigit = static_cast<int>(digit) - '0';
	    if ((idigit < 0) || (idigit > 9)) throw (ip_domain_error());
	    retval = (retval * 10 ) + idigit;
	    sub = sub.substr(1, std::string::npos);
	}
	return retval;
    }

    std::string int8_to_string(uint32_t val)
    {
	std::stringstream ss;
	ss << val;
	return ss.str();
    }

    int count_trailing_zeroes(uint32_t val)
    {
	if (val == 0) return 32;
	static const int MultiplyDeBruijnBitPosition[32] = 
	{
	  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
	  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};
	return
	    MultiplyDeBruijnBitPosition[((val & -val) * 0x077CB531U) >> 27];
    }

int log2(uint32_t val)
{
    #define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
    static const char LogTable256[256] = 
    {
	-1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
	LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
    };

    uint32_t t, tt; // temporaries

    if ((tt = (val >> 16)) != 0)
    {
      return (t = (tt >> 8)) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
    }
    else 
    {
      return (t = (val >> 8)) ? 8 + LogTable256[t] : LogTable256[val];
    }
}

int ceil_log2(uint32_t val)
{
    uint32_t retval = log2(val);
    if ((static_cast<uint32_t>(1) << retval) != val) ++retval;
    return retval;
}

} // namespace anonymous


///////////////////////////////////////
// Implementation of exception classes
///////////////////////////////////////

const char* ip_range_error::what() const noexcept
{
    return "Could not parse IP range";
}
const char* ip_domain_error::what() const noexcept
{
    return "Could not parse IP address";
}


/////////////////////////////////
// Implementation of Range class
/////////////////////////////////

Range::~Range()
{
}
Range::Range(Range const& other)
 : mRep(other.mRep)
{
}
Range& Range::operator=(Range const& other)
{
    if (&other != this)
    {
        mRep = other.mRep;
    }
    return *this;
}
Range::Range(Range&& other)
 : mRep(other.mRep)
{
}
Range& Range::operator=(Range&& other)
{
    if (&other != this)
    {
        mRep = other.mRep;
    }
    return *this;
}

Range::Range(uint32_t lower, uint32_t upper) throw(ip_range_error)
 : mRep(lower, upper)
{
}

uint32_t Range::lower_bound() const noexcept { return mRep.lower_bound(); }
uint32_t Range::upper_bound() const noexcept { return mRep.upper_bound(); }

Range::Range(const std::string& expr) throw (ip_domain_error, ip_range_error)
 : mRep(0,0)
{
    std::string left, right;
    uint32_t lower, upper;
    std::string::size_type index;

    // Is the expression a lower and upper bound?
    index = expr.find('-');
    if (index != std::string::npos)
    {
         left = expr.substr(0, index);
	 lower = quad_to_int(left);
	 right = expr.substr(index+1, std::string::npos);
	 upper = quad_to_int(right);
    }
    // Is the expression a starting point and a bitmask?
    else
    {
        index = expr.find('/');
	if (index != std::string::npos)
	{
	     left = expr.substr(0, index);
	     lower = quad_to_int(left);
	     right = expr.substr(index+1, std::string::npos);
	     uint32_t count = string_to_int(right);
	     if (count > 32) throw(ip_range_error());
	     uint32_t mask = (1 << (32 - count)) - 1;
	     upper = lower | mask;
	     lower = lower & ~mask;
	}
	// Assume the expression is a single IP address
	else
	{
	    upper = lower = quad_to_int(expr);
	}
    }
    mRep = NumRange<uint32_t> (lower, upper);
}

Range::Range(Range& ra, std::string& middle) throw (ip_range_error)
 : mRep(ra.lower_bound(), quad_to_int(middle))
{
}


////////////////////////////////
// Implementation of List class
////////////////////////////////

List::List()
 : mRep{}
{
}
List::~List()
{
}

List::List(List const& other)
 : mRep(other.mRep)
{
}

List& List::operator=(List const& other)
{
    if (&other != this)
    {
        mRep = other.mRep;
    }
    return *this;
}

List::List(List&& other)
 : mRep (other.mRep)
{
}

List& List::operator=(List&& other)
{
    if (&other != this)
    {
        mRep = other.mRep;
    }
    return *this;
}

void List::add(const Range& range) noexcept
{
    NumRange<uint32_t> elem(range.lower_bound(), range.upper_bound());
    mRep.add(elem);
}

void List::subtract(const Range& range) throw (std::exception)
{
    NumRange<uint32_t> elem(range.lower_bound(), range.upper_bound());
    mRep.subtract(elem);
}

void List::print(std::ostream& ost, bool dashes) const
{
    if (dashes)
    {
	mRep.process(
	    [&ost] (uint32_t lower, uint32_t upper) {
	        ost << int_to_quad(lower);
		if (upper != lower)
		    ost << '-' << int_to_quad(upper);
		ost << std::endl;
	    }
	);
    }
    else
    {
	mRep.process(
	    [&ost] (uint32_t lower, uint32_t upper) { // Start lambda function
		while (lower <= upper)
		{
		    int zbits = count_trailing_zeroes(lower);
		    int maxbits = log2(upper - lower + 1);
		    if (zbits > maxbits) zbits = maxbits;
		    if (zbits == 0)
		    {
			ost << int_to_quad(lower) << std::endl;
			++lower;
		    }
		    else
		    {
			uint32_t mask = (1 << zbits) - 1;
			uint32_t middle = lower | mask;
			ost << int_to_quad(lower) << '/' << (32 - zbits)
			    << std::endl;
			// Avoid numeric overflow
			static const uint32_t bmax =
			    std::numeric_limits<uint32_t>::max();
			if (middle == bmax) break;
			lower = middle + 1;
		    }
		}
	    } // End lambda function
	);
    }
}

uint32_t List::min() const throw (numeric_range_error)
{
    return mRep.min();
}
uint32_t List::max() const throw (numeric_range_error)
{
    return mRep.max();
}

std::ostream& operator<< (std::ostream& ost, const List& list)
{
    list.print(ost);
    return ost;
}

unsigned long List::num_operations() const
{
    return mRep.num_operations();
}

void List::process(
   std::function<void(uint32_t lower_limit, uint32_t upper_limit)> fn) const
{
    mRep.process(fn);
}

void List::process(
    uint32_t left, uint32_t right,
    std::function<void(uint32_t,uint32_t)> fn) const
{
    mRep.process(left, right, fn);
}

void List::verify() const throw (std::exception)
{
    mRep.verify();
}


///////////////////////////////////////////
// Implementation of stand-alone functions
///////////////////////////////////////////

std::string int_to_quad(uint32_t val)
{
    return
	int8_to_string((val >> 24) & 0xFF) +
	'.'                                +
	int8_to_string((val >> 16) & 0xFF) +
	'.'                                +
	int8_to_string((val >>  8) & 0xFF) +
	'.'                                +
	int8_to_string (val        & 0xFF) ;
}

uint32_t quad_to_int(const std::string& expr) throw (ip_domain_error)
{
    int part;
    uint32_t retval = 0;
    std::string sub(expr);
    for (part = 0 ; part < 3 ; ++part)
    {
	std::string::size_type index = sub.find('.');
	if (index == std::string::npos) throw (ip_domain_error());
	std::string octet = sub.substr(0, index);
	uint32_t byte = string_to_int(octet);
	if (byte > 255) throw (ip_domain_error());
	retval = (retval << 8) | byte;
	sub = sub.substr(index+1, std::string::npos);
    }
    std::string octet = sub.substr(0, std::string::npos);
    uint32_t byte = string_to_int(octet);
    if (byte > 255) throw (ip_domain_error());
    retval = (retval << 8) | byte;

    return retval;
}

std::pair<uint32_t /*start*/, int /*bits*/> to_cidr (
    uint32_t lower_bound, uint32_t upper_bound)
    throw (ip_range_error)
{
    std::pair<uint32_t,int> retval;

    // Dispense with special cases
    if (upper_bound < lower_bound) throw (ip_range_error());
    if (upper_bound > 32) throw (ip_range_error());
    static const uint32_t bmax = std::numeric_limits<uint32_t>::max();
    if ((lower_bound == 0) && (upper_bound == bmax))
    {
        retval.first = 0;
        retval.second = 32;
        return retval;
    }

    // Start with a minimum length
    uint32_t log_length = ceil_log2(upper_bound - lower_bound + 1);
    uint32_t mask = (1 << log_length) - 1;
    uint32_t base = lower_bound & mask;
    if ((base <= lower_bound) && ((base | mask) <= upper_bound))
    {
        retval.first = base;
        retval.second = 32 - log_length;
        return retval;
    }

    // Double and try again
    ++log_length;
    mask = (1 << log_length) - 1;
    base = lower_bound & mask;
    if ((base <= lower_bound) && ((base | mask) <= upper_bound))
    {
        retval.first = base;
        retval.second = 32 - log_length;
        return retval;
    }

    // The third time has to work (theorem)
    ++log_length;
    mask = (1 << log_length) - 1;
    base = lower_bound & mask;
    retval.first = base;
    retval.second = 32 - log_length;
    return retval;
}

} // namespace IPAR

#include "ipar_numlist.tcc"
