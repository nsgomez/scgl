#pragma once
#include "cGZAllocatorServiceSTLAllocator.h"
#include <list>

template<typename T>
using ilist = std::list<T, cGZAllocatorServiceSTLAllocator<T>>;