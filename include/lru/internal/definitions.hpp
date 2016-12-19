/**
* The MIT License (MIT)
* Copyright (c) 2016 Peter Goldsborough and Markus Engel
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef LRU_INTERNAL_GLOBALS_HPP
#define LRU_INTERNAL_GLOBALS_HPP

#include <chrono>
#include <cstddef>
#include <list>
#include <tuple>
#include <unordered_map>

namespace LRU {
namespace Internal {
const std::size_t DEFAULT_CAPACITY = 128;

template <typename T>
using Queue = std::list<T>;

template <typename Key, typename Information>
using Map = std::unordered_map<Key, Information>;

using Clock = std::chrono::steady_clock;
using Timestamp = Clock::time_point;
}  // namespace Internal
}  // namespace LRU


#endif /* LRU_INTERNAL_GLOBALS_HPP*/