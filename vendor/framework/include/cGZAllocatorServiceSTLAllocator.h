#pragma once
#include "cIGZAllocatorService.h"
#include "cRZSysServPtr.h"
#include <memory>

/**
 * @brief A STL allocator that uses the game's memory pool 
 * 
 * This is an allocator for STL containers like lists and sets which uses the
 * game's built-in memory allocator, which (presumably) allocates from a pool
 * that it uses for optimization.
 *
 * As of present, there doesn't appear to be a need to actually use this
 * allocator in custom COM directors, and trying to use it with class methods
 * that take STL containers will cause compilation errors because, as it turns
 * out, std::list<T> is different from std::list<T, A>.
 */
template<typename T>
class cGZAllocatorServiceSTLAllocator
{
	public:
		typedef T               value_type;
		typedef T*              pointer;
		typedef const T*        const_pointer;
		typedef T&              reference;
		typedef const T&        const_reference;
		typedef std::size_t     size_type;
		typedef std::ptrdiff_t  difference_type;
		typedef std::true_type  is_always_equal;
		typedef std::true_type  propagate_on_container_move_assignment;

		template <class U>
		struct rebind {
			typedef cGZAllocatorServiceSTLAllocator<U> other;
		};

	public:
		cRZSysServPtr<cIGZAllocatorService, 988069547ul, 988069539ul> AS(void) {
			return cRZSysServPtr<cIGZAllocatorService, 988069547ul, 988069539ul>();
		}

		pointer allocate(size_type nCount) { return (pointer)(AS()->Allocate(sizeof(T) * nCount)); }
		pointer allocate(size_type nCount, void const* pHint) { return allocate(nCount); }
		void deallocate(pointer pElem, size_type nCount) { AS()->Deallocate(pElem); }
		void construct(pointer pElem, const_reference sValue) { new(pElem) T(sValue); }
		void destroy(pointer pElem) { pElem->~T(); }
		
		size_type max_size(void) const { return std::numeric_limits<size_t>::max() / sizeof(T); }
		pointer address(reference sValue) { return &sValue; }
		const_pointer address(const_reference sValue) const { return &sValue; }
};