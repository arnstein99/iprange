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
uint32_t quad_to_int(const std::string& expr);
std::string int_to_quad(uint32_t val);

// Inputs a range of integers and returns the smallest CIDR range that
// contains it. The return value indicates the start of the CIDR range
// and the number of fixed bits in the range. This format is what
// iptables(1) uses.
std::pair<uint32_t /*start*/, int /*bits*/> to_cidr (
    uint32_t lower_bound, uint32_t upper_bound);

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
class Range : public NumRange<uint32_t>
{
public:

    Range() noexcept;
    ~Range();
    Range(Range const& other) noexcept;
    Range& operator=(Range const& other) noexcept;
    Range(Range&& other) noexcept;
    Range& operator=(Range&& other) noexcept;
    Range(uint32_t lower, uint32_t upper);
    Range(const std::string& expr);

    // Splitter-constructor. 
    // Note that the first argument is NOT const.
    // The second argument expresses a numeric IP address a.b.c.d that must be
    // within the range of the first argument.
    Range(Range&, std::string& middle);

}; // class Range

// List of IP addresses. Maintained as a sorted list of non-overlapping,
// non-adjacent intervals in CIDR form. That is, each interval can be
// expressed as nn.nn.nn.nn/mm and the least significant (32-mm) bits of
// nn.nn.nn.nn are zero.
class List : public NumList<uint32_t>
{
public:

    List() noexcept;
    ~List();
    List(List const& other) noexcept;
    List& operator=(List const& other) noexcept;
    List(List&& other) noexcept;
    List& operator=(List&& other) noexcept;

    // Add an interval of IP addresses.
    void add (const Range&) noexcept;
    void add_from (const NumList<uint32_t>::const_iterator& iter) noexcept;
    void add_from (const NumList<uint32_t>::const_reverse_iterator& iter)
        noexcept;

    // Remove an interval of IP addresses.
    void subtract (const Range&);
    void subtract_from (const NumList<uint32_t>::const_iterator& iter) noexcept;
    void subtract_from (const NumList<uint32_t>::const_reverse_iterator& iter)
        noexcept;

    // Print out everything in the list. A series of strings nn.nn.nn.nn/nn in
    // sorted order. If dashes is true, output is nn.nn.nn.nn-nn.nn.nn.nn.
    void print(std::ostream& ost, bool dashes=false) const;

    // Statistics
    uint32_t min() const;
    uint32_t max() const;

    // For diagnostic use: how many non-trivial transformations have been
    // performed since construction.
    unsigned long num_operations() const;

    // For debugging
    void verify() const;

private:

}; // class List

std::ostream& operator<< (std::ostream&, const List&);

} // namespace IPAR

#endif // } IPAR_IPLIST_H_
