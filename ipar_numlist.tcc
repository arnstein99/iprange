#ifndef IPAR_NUMLIST_TCC_ // {
#define IPAR_NUMLIST_TCC_

#include <exception>
#include <functional>
#include <limits>
#include <map>

namespace IPAR {

namespace { // anonymous

// Programming note: all this complexity is a test of
//     (right_first <=  left_second + 1)  (bad, overlap)
// without the possibility of numeric overflow.
template<typename BOUND>
bool bad_order (BOUND left_second, BOUND right_first)
{
    static const BOUND bmax = std::numeric_limits<BOUND>::max();
    bool bOverlap = false;
    BOUND limit;
    if (left_second == bmax)
    {
	bOverlap = true;
    }
    else
    {
	// Safe to increment
        limit = left_second; ++limit;
	bOverlap = (right_first <= limit);
    }
    return bOverlap;
}

template<typename BOUND> 
void verify_special (
    const std::map<BOUND,BOUND>& rep,
    const typename std::map<BOUND,BOUND>::iterator& first)
throw (std::exception)
{
    auto iter = first;
    if (iter == rep.end()) return;
    if (iter->second < iter->first)
    {
	throw (std::exception());
    }
    BOUND prev = iter->second;
    for (++iter ; iter != rep.end() ; ++iter)
    {
	if (iter->second < iter->first)
	{
	    throw (std::exception());
	}
	if (bad_order (prev, iter->first))
	{
	    auto left = prev(iter);
	    throw (std::exception());
	}
	prev = iter->second;
    }
}

} // namespace anonymous


const char* numeric_range_error::what() const noexcept
{
    return "Could not process numeric range";
}


///////////////////////////
// NumRange implementation
///////////////////////////

template<typename BOUND>
NumRange<BOUND>::NumRange() noexcept
 : std::pair<BOUND, BOUND>()
{
}

template<typename BOUND>
NumRange<BOUND>::~NumRange()
{
}

template<typename BOUND>
NumRange<BOUND>::NumRange(NumRange<BOUND> const& other) noexcept
 : std::pair<BOUND, BOUND>(other)
{
}
template<typename BOUND>
NumRange<BOUND>::NumRange(const std::pair<BOUND,BOUND>& other)
    throw (numeric_range_error)
 : std::pair<BOUND, BOUND>(other)
{
}

template<typename BOUND>
NumRange<BOUND>& NumRange<BOUND>::operator=(NumRange<BOUND> const& other)
    noexcept
{
    std::pair<BOUND, BOUND>::operator=(other);
    return *this;
}
template<typename BOUND>
NumRange<BOUND>::NumRange(NumRange<BOUND>&& other) noexcept
 : std::pair<BOUND, BOUND>(other)
{
}
template<typename BOUND>
NumRange<BOUND>& NumRange<BOUND>::operator=(NumRange<BOUND>&& other) noexcept
{
    std::pair<BOUND, BOUND>::operator=(other);
    return *this;
}

template<typename BOUND>
NumRange<BOUND>::NumRange(BOUND lower, BOUND upper) throw (numeric_range_error)
 : std::pair<BOUND, BOUND>(lower, upper)
{
    if (lower > upper) throw numeric_range_error();
}

template<typename BOUND> 
NumRange<BOUND>::NumRange(NumRange& nr, BOUND middle)
   throw (numeric_range_error)
 : std::pair<BOUND, BOUND>(nr.first, middle)
{
    static BOUND bmax = std::numeric_limits<BOUND>::max();
    if (middle == bmax) throw numeric_range_error();

    if (nr.first > middle) throw numeric_range_error();

    nr.mLower = ++middle;
    if (nr.first > nr.second) throw numeric_range_error();
}


//////////////////////////
// NumList implementation
//////////////////////////

template<typename BOUND>
NumList<BOUND>::NumList() noexcept
 : std::map<BOUND,BOUND>(), mNumOperations(0)
{
}

template<typename BOUND>
NumList<BOUND>::~NumList()
{
}

template<typename BOUND>
NumList<BOUND>::NumList(const NumList<BOUND>& other) noexcept
 : std::map<BOUND,BOUND>(other), mNumOperations(0)
{
}

template<typename BOUND>
NumList<BOUND>::NumList(NumList<BOUND>&& other) noexcept
 : std::map<BOUND,BOUND>(other), mNumOperations(0)
{
}

template<typename BOUND>
NumList<BOUND>& NumList<BOUND>::operator=(const NumList<BOUND>& other) noexcept
{
    std::map<BOUND,BOUND>::operator=(other);
    return *this;
}

template<typename BOUND>
NumList<BOUND>& NumList<BOUND>::operator=(NumList<BOUND>&& other) noexcept
{
    std::map<BOUND,BOUND>::operator=(other);
    return *this;
}

template<typename BOUND>
void NumList<BOUND>::add (const NumRange<BOUND>& range) noexcept
{
    BOUND new_key   = range.first;
    BOUND new_upper = range.second;

    // Dispensing with a special case simplifies matters
    if (std::map<BOUND,BOUND>::empty())
    {
        (*this)[new_key] = new_upper;
	return;
    }

    // Find a home for the input element
    auto pr = std::map<BOUND,BOUND>::insert(std::make_pair(new_key, new_upper));
    auto base_iter = pr.first;
    auto check_iter = base_iter;
    if (pr.second)
    {
	// No existing interval starts with this lower bound,
	// so we made one.

	if (base_iter == this->cbegin())
	{
	    // First check will be between new element and its next in map
	    ++check_iter;
	}
	else
	{
	    // Check for overlap with previous element
	    auto prev_iter = std::prev(base_iter);
	    if (bad_order (prev_iter->second, base_iter->first))
	    {
	         // Coalesce
		if (prev_iter->second < base_iter->second)
		    prev_iter->second = base_iter->second;

		// The new element is now completely redundant
		base_iter = std::map<BOUND,BOUND>::erase (base_iter);
		--base_iter; // Points back to prev_iter

		++mNumOperations;
	    }

	    // Will check the rest of the map
	    check_iter = std::next(base_iter);
	}
    }
    else
    {
	// Found existing element, expand if if necessary
        if (bad_order (base_iter->second, new_upper))
	    base_iter->second = new_upper;

	// Begin checking right after existing element
        ++check_iter;
    }

    // Now take care of adjacent or overlapping entries
    while (check_iter != this->cend())
    {
        if (bad_order (base_iter->second, check_iter->first))
	{
	    // coalesce, as before.
	    if (base_iter->second < check_iter->second)
		base_iter->second = check_iter->second;
	    
	    // The old element is now completely redundant, as before.
	    check_iter = std::map<BOUND,BOUND>::erase(check_iter);
	    base_iter = std::prev(check_iter);

	    ++mNumOperations;
	}
	else
	{
	    // Gap exists, so we are done here.
	    break;
	}
    }
}

template<typename BOUND> 
void NumList<BOUND>::subtract (const NumRange<BOUND>& range)
    throw (std::exception)
{
    // Eliminate a special case
    if (std::map<BOUND,BOUND>::empty()) return;

    BOUND new_key   = range.first;
    BOUND new_upper = range.second;

    // Find a location for the input element
    auto check_iter = std::map<BOUND,BOUND>::lower_bound(new_key);
    // if (new_key >= check_iter->first) ...
    if (check_iter != this->cbegin()) --check_iter;

    // ... and this can only happen once.
    if (new_key > check_iter->first)
	subtract_sub1(check_iter, new_key, new_upper);

    // Take care of adjacent or overlapping entries
    while (check_iter != this->cend())
	subtract_sub2(check_iter, new_upper);
}

template<typename BOUND> 
unsigned long NumList<BOUND>::num_operations() const
{
    return mNumOperations;
}

template<typename BOUND> 
BOUND NumList<BOUND>::min() const throw (numeric_range_error)
{
    if (std::map<BOUND,BOUND>::empty()) throw (numeric_range_error());
    return this->cbegin()->first;
}

template<typename BOUND> 
BOUND NumList<BOUND>::max() const throw (numeric_range_error)
{
    if (std::map<BOUND,BOUND>::empty()) throw (numeric_range_error());
    return this->crbegin()->second;
}

template<typename BOUND> 
void NumList<BOUND>::verify() const throw (std::exception)
{
    auto iter = this->cbegin();
    if (iter == this->cend()) return;
    if (iter->second < iter->first)
    {
	throw (std::exception());
    }
    BOUND prev = iter->second;
    for (++iter ; iter != this->cend() ; ++iter)
    {
	if (iter->second < iter->first)
	{
	    throw (std::exception());
	}
	if (bad_order (prev, iter->first))
	{
	    // auto left = std::prev(iter);
	    throw (std::exception());
	}
	prev = iter->second;
    }
}

template<typename BOUND>
void NumList<BOUND>::subtract_sub1(
    typename std::map<BOUND, BOUND>::iterator & check_iter,
    BOUND new_key, BOUND new_upper)
	throw ()
{
    // This method is only valid
    // if (new_key > check_iter->first)

    // This is actually the most common case
    if (check_iter->second < new_key)
    {
	// existing element:      ********** 
	//            minus:                **********
	//           result:      **********  
	++check_iter;
	return;
    }

    BOUND replacement_lower, replacement_upper;

    // Two cases to consider
    if (new_upper < check_iter->second)
    {
	// existing element:      **********
	//            minus:         *****  
	//           result:      ***     **
	replacement_lower = new_upper; ++replacement_lower;
	replacement_upper = check_iter->second;
	check_iter->second = new_key; --(check_iter->second);
	auto pr = std::make_pair(replacement_lower, replacement_upper);
	check_iter = std::map<BOUND,BOUND>::insert(std::next(check_iter), pr);
	++check_iter;
	++mNumOperations;
    }
    else // (new_upper >= check_iter->second)
    {
	// existing element:      **********
	//            minus:         ******* 
	//            minus:         *************
	//           result:      ***        
	check_iter->second = new_key; --(check_iter->second);
	++check_iter;
	++mNumOperations;
    }
}

template<typename BOUND>
void NumList<BOUND>::subtract_sub2(
    typename std::map<BOUND, BOUND>::iterator & check_iter,
    BOUND new_upper)
	throw ()
{
    // This method is only valid
    // if (new_key <= check_iter->first)

    if (check_iter == this->cend()) throw (std::exception());

    // this is actually the most common case
    if (check_iter->first > new_upper)
    {
	// existing element:                ********** 
	//            minus:      ********** 
	//           result:                ********** 

	// Indicate completion
	check_iter = std::map<BOUND,BOUND>::end();
	return;
    }

    BOUND replacement_lower, replacement_upper;

    // Two cases to consider
    if (new_upper < check_iter->second)
    {
	// existing element:      **********
	//            minus: **********
	//           result:           *****
	replacement_lower = new_upper; ++replacement_lower;
	replacement_upper = check_iter->second;
	check_iter = std::map<BOUND,BOUND>::erase(check_iter);
	auto pr = std::make_pair(replacement_lower, replacement_upper);
	check_iter = std::map<BOUND,BOUND>::insert(check_iter, pr);
	++check_iter;
	++mNumOperations;
    }
    else // (new_upper >= check_iter->second)
    {
	// existing element:      **********
	//            minus: ***************
	//            minus: *******************
	//           result:                 
	check_iter = std::map<BOUND,BOUND>::erase(check_iter);
	++mNumOperations;
    }
}


///////////////////////////////////////
// Convenience function implementation
///////////////////////////////////////

} // namespace IPAR

#endif // } IPAR_NUMLIST_TCC_
