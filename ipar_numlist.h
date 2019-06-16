#ifndef IPAR_NUMLIST_H_ // {
#define IPAR_NUMLIST_H_

#include <exception>
#include <functional>
#include <map>

namespace IPAR {

// An exception that is thrown by this software.
class numeric_range_error : public std::exception
{
    virtual const char* what() const noexcept;
};

// A dense interval.
template<typename BOUND>
class NumRange
{
public:

    NumRange() = delete;
    ~NumRange();
    NumRange(NumRange const& other) noexcept;
    NumRange& operator=(NumRange const& other) noexcept;
    NumRange(NumRange&& other) noexcept;
    NumRange& operator=(NumRange&& other) noexcept;

    // Construct from lower and upper bounds. Correct ordering is checked.
    NumRange(BOUND lower, BOUND upper) throw (numeric_range_error);

    // Splitter-constructor. Note that the argument is NOT const.
    // Shortens the length of this interval and returns a new interval
    // representing that which was removed. The new interval starts with
    // argument middle.
    NumRange(NumRange& nr, BOUND middle) throw (numeric_range_error);

    // Report contents
    BOUND lower_bound() const noexcept;
    BOUND upper_bound() const noexcept;

private:

    BOUND mLower;
    BOUND mUpper;

}; // class NumRange

// A collection of intervals, maintained in a standard form. What this means is
// that the intervals are always sorted, and there are never two adjacent or
// overlapping intervals.
template<typename BOUND>
class NumList
{
public:

    NumList();
    ~NumList();
    NumList(NumList const& other);
    NumList& operator=(NumList const& other);
    NumList(NumList&& other);
    NumList& operator=(NumList&& other);

    // Add an interval to the collection.
    void add (const NumRange<BOUND>& range) noexcept;

    // Remove an interval from the collection. The interval need not be part of
    // the collection. This method handles overlaps, etc.
    void subtract (const NumRange<BOUND>& range) throw (std::exception);

    // Apply a function to every interval in the collection, in sorted order.
    void process(std::function<void(BOUND,BOUND)> fn) const;

    // Same, but only for intervals that have intersection with [left, right]
    void process(
        BOUND left, BOUND right, std::function<void(BOUND,BOUND)> fn) const;

    // Report extreme values
    BOUND min() const throw (numeric_range_error);
    BOUND max() const throw (numeric_range_error);

    // For diagnostic use: how many non-trivial transformations have been
    // performed since construction.
    unsigned long num_operations() const;

    // For debugging
    void verify() const throw (std::exception);

private:

    void subtract_sub1(
	typename std::map<BOUND, BOUND>::iterator & check_iter,
        BOUND new_key, BOUND new_upper)
	    throw ();
    void subtract_sub2(
	typename std::map<BOUND, BOUND>::iterator & check_iter,
        BOUND new_upper)
	    throw ();

    std::map<BOUND,BOUND> mRep;
    unsigned long mNumOperations;

}; // class NumList

// Convenience functions

// Compute the intersection of two intervals. If the return value is false, the
// intersection is the empty set. In this case, the result argumentss are
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
     else // (a_lower > b_loser)
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
