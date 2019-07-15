#ifndef IPAR_NUMLIST_H_ // {
#define IPAR_NUMLIST_H_

#include <exception>
#include <functional>
#include <limits>
#include <map>
#include "ipar_readonly.h"

namespace IPAR {

// An exception that is thrown by this software.
class numeric_range_error : public std::exception
{
    virtual const char* what() const noexcept;
};

template<typename BOUND, BOUND BMAX> class NumList;

// A dense interval.
template<typename BOUND, BOUND BMAX=std::numeric_limits<BOUND>::max()>
class NumRange : public std::pair< ReadOnly<BOUND>, ReadOnly<BOUND> >
{
public:

    NumRange() = default;
    ~NumRange()= default;
    NumRange(NumRange const& other) = default;
    NumRange(NumRange&& other) = default;
    NumRange& operator=(NumRange const& other) noexcept;
    NumRange& operator=(NumRange&& other) noexcept;

    // Construct from lower and upper bounds. Correct ordering is checked.
    NumRange(BOUND lower, BOUND upper);

    // Splitter-constructor. Note that the argument is NOT const.
    // Shortens the length of this interval and returns a new interval
    // representing that which was removed. The new interval starts with
    // argument middle.
    NumRange(NumRange& nr, BOUND middle);

    friend class NumList<BOUND,BMAX>;

}; // class NumRange

// A collection of intervals, maintained in a standard form. What this means is
// that the intervals are always sorted, and there are never two adjacent or
// overlapping intervals.
template<typename BOUND, BOUND BMAX=std::numeric_limits<BOUND>::max()>
class NumList : private std::map< ReadOnly<BOUND>, ReadOnly<BOUND> >
{
public:

    NumList() noexcept;
    ~NumList() = default;
    NumList(NumList const& other) noexcept;
    NumList& operator=(NumList const& other) noexcept;
    NumList(NumList&& other) noexcept;
    NumList& operator=(NumList&& other) noexcept;

    using MapType=std::map< ReadOnly<BOUND>, ReadOnly<BOUND> >;

    // The only way to access content
    class const_iterator
      : public MapType::const_iterator
    {
        public:
        const_iterator(const typename MapType::const_iterator& m)
         : MapType::const_iterator(m)
        { }
    };
    class const_reverse_iterator
      : public MapType::const_reverse_iterator
    {
        public:
        const_reverse_iterator(
            const typename MapType::const_reverse_iterator& m)
         : MapType::const_reverse_iterator(m)
        { }
    };
    const_iterator cbegin() const {
        return const_iterator(MapType::cbegin()); }
    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(MapType::crbegin()); }
    const_iterator cend() const {
        return const_iterator(MapType::cend()); }
    const_reverse_iterator crend() const {
        return const_reverse_iterator(MapType::crend()); }
    const_iterator lower_bound(BOUND left) {
        return const_iterator(MapType::lower_bound(left)); }
    const_iterator upper_bound(BOUND left) {
        return const_iterator(MapType::upper_bound(left)); }

    // Add an interval to the collection.
    void add (const NumRange<BOUND,BMAX>& range) noexcept;

    // Remove an interval from the collection. The interval need not be part of
    // the collection. This method handles overlaps, etc.
    void subtract (const NumRange<BOUND,BMAX>& range) noexcept;

    // Report extreme values
    BOUND min() const;
    BOUND max() const;

    // For diagnostic use: how many non-trivial transformations have been
    // performed since construction.
    unsigned long num_operations() const;

    // For debugging
    void verify() const;

private:

    void subtract_sub1(
	typename MapType::iterator & check_iter,
        BOUND new_key, BOUND new_upper);
    void subtract_sub2(
	typename MapType::iterator & check_iter,
        BOUND new_upper);

    unsigned long mNumOperations;

}; // class NumList

// Convenience functions

// Compute the intersection of two intervals. If the return value is false, the
// intersection is the empty set. In this case, the result arguments are
// undefined.
// All results are undefined if (a_lower > a_upper) or (b_lower > b_upper).
template<typename BOUND>
bool intersect (BOUND a_lower, BOUND a_upper,
                BOUND b_lower, BOUND b_upper,
		BOUND& result_lower, BOUND& result_upper)
{
     if (a_lower <= b_lower)
     {
	 if (a_upper < b_lower) return false;
	 result_lower = b_lower;
     }
     else // (a_lower > b_lower)
     {
	 if (b_upper < a_lower) return false;
	 result_lower = a_lower;
     }

     if (a_upper <= b_upper)
	 result_upper = a_upper;
     else
	 result_upper = b_upper;

    return true;
}

} // namespace IPAR

#endif // } IPAR_NUMLIST_H_
