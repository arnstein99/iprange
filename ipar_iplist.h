#ifndef IPAR_IPLIST_H_ // {
#define IPAR_IPLIST_H_

#include <iostream>
#include <sstream>
#include <string>
// #include <cstdint>
#include "ipar_numlist.h"

namespace IPAR {

// Exceptions that are thrown by this software
class ip_range_error : public std::exception
{
public:
    virtual const char* what() const noexcept;
};
class ip_domain_error : public std::exception
{
public:
    virtual const char* what() const noexcept;
};

// Convenience functions.

// Reads a IP address in format nn.nn.nn.nn and outputs an equivalent 32-bit
// number
uint32_t quad_to_int(const std::string& expr) throw (ip_domain_error);
std::string int_to_quad(uint32_t val);

// Inputs a range of integers and returns the smallest CIDR range that
// contains it. The return value indicates the start of the CIDR range
// and the number of fixed bits in the range. This format is what
// iptables(1) uses.
std::pair<uint32_t /*start*/, int /*bits*/> to_cidr (
    uint32_t lower_bound, uint32_t upper_bound)
    throw (ip_range_error);

// Available from ipar_numlist.h:
// Compute the intersection of two intervals. If the return value is false, the
// intersection is the empty set. In this case, the result argumentss are
// undefined.
// All results are undefined if (a_lower > a_upper) or (b_lower > b_upper).
// template<typename BOUND>
// bool intersect (BOUND a_lower, BOUND a_upper,
//                 BOUND b_lower, BOUND b_upper,
//                 BOUND& result_lower, BOUND& result_upper)

// An interval of IP addresses. Mostly used to translate to and from strings.
class Range
{
public:

    Range() = delete;
    ~Range();
    Range(Range const& other);
    Range& operator=(Range const& other);
    Range(Range&& other);
    Range& operator=(Range&& other);
    Range(uint32_t lower, uint32_t upper) throw(ip_range_error);
    Range(const std::string& expr) throw(ip_domain_error, ip_range_error);
    uint32_t lower_bound() const noexcept;
    uint32_t upper_bound() const noexcept;

    // Splitter-constructor. 
    // Note that the first argument is NOT const.
    // The second argument expresses a numeric IP address a.b.c.d that must be
    // within the range of the first argument.
    Range(Range&, std::string& middle) throw (ip_range_error);

private:

    NumRange<uint32_t> mRep;

}; // class Range

// List of IP addresses. Maintained as a sorted list of non-overlapping,
// non-adjacent intervals in standard form. That is, each interval can be
// expressed as nn.nn.nn.nn/nn and the mask implied by /nn does not knock out
// any bits in nn.nn.nn.nn.
class List
{
public:

    List();
    ~List();
    List(List const& other);
    List& operator=(List const& other);
    List(List&& other);
    List& operator=(List&& other);

    // Add an interval of IP addresses.
    void add (const Range&) noexcept;

    // Remove an interval of IP addresses.
    void subtract (const Range&) throw (std::exception);

    // Print out everything in the list. A series of strings nn.nn.nn.nn/nn in
    // sorted order. If dashes is true, output is nn.nn.nn.nn-nn.nn.nn.nn.
    void print(std::ostream& ost, bool dashes=false) const;

    // Statistics
    uint32_t min() const throw (numeric_range_error);
    uint32_t max() const throw (numeric_range_error);

    // For diagnostic use: how many non-trivial transformations have been
    // performed since construction.
    unsigned long num_operations() const;

    // Apply a function to every interval in list, in sorted order.
    // Useful for testing.
    void process(
       std::function<void(uint32_t lower_limit, uint32_t upper_limit)> fn)
    const;

    // Same, but only for intervals that have intersection with [left, right]
    void process(
        uint32_t left, uint32_t right,
	std::function<void(uint32_t,uint32_t)> fn) const;

    // For debugging
    void verify() const throw (std::exception);

private:

    NumList<uint32_t> mRep;

}; // class List
std::ostream& operator<< (std::ostream&, const List&);

} // namespace IPAR

#endif // } IPAR_IPLIST_H_
