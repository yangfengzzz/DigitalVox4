//----------------------------------------------------------------------------//
//                                                                            //
// vox-animation is hosted at http://github.com/guillaumeblanc/vox-animation  //
// and distributed under the MIT License (MIT).                               //
//                                                                            //
// Copyright (c) Guillaume Blanc                                              //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included in //
// all copies or substantial portions of the Software.                        //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    //
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    //
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        //
// DEALINGS IN THE SOFTWARE.                                                  //
//                                                                            //
//----------------------------------------------------------------------------//

#ifndef VOX_ALIGN_H_
#define VOX_ALIGN_H_

#include <stdint.h>
#include <cassert>
#include <cstddef>

namespace vox {

// Finds the number of elements of a statically allocated array.
#define VOX_ARRAY_SIZE(_array) (sizeof(_array) / sizeof(_array[0]))

// Instructs the compiler to try to inline a function, regardless cost/benefit
// compiler analysis.
// Syntax is: "VOX_INLINE void function();"
#if defined(_MSC_VER)
#define VOX_INLINE __forceinline
#else
#define VOX_INLINE inline __attribute__((always_inline))
#endif

// Tells the compiler to never inline a function.
// Syntax is: "VOX_NO_INLINE void function();"
#if defined(_MSC_VER)
#define VOX_NOINLINE __declspec(noinline)
#else
#define VOX_NOINLINE __attribute__((noinline))
#endif

// Tells the compiler that the memory addressed by the restrict -qualified
// pointer is not aliased, aka no other pointer will access that same memory.
// Syntax is: void function(int* VOX_RESTRICT _p);"
#define VOX_RESTRICT __restrict

// Defines macro to help with DEBUG/NDEBUG syntax.
#if defined(NDEBUG)
#define VOX_IF_DEBUG(...)
#define VOX_IF_NDEBUG(...) __VA_ARGS__
#else  // NDEBUG
#define VOX_IF_DEBUG(...) __VA_ARGS__
#define VOX_IF_NDEBUG(...)
#endif  // NDEBUG

// Case sensitive wildcard string matching:
// - a ? sign matches any character, except an empty string.
// - a * sign matches any string, including an empty string.
bool strmatch(const char* _str, const char* _pattern);

// Tests whether _block is aligned to _alignment boundary.
template <typename _Ty>
VOX_INLINE bool IsAligned(_Ty _value, size_t _alignment) {
  return (_value & (_alignment - 1)) == 0;
}
template <typename _Ty>
VOX_INLINE bool IsAligned(_Ty* _address, size_t _alignment) {
  return (reinterpret_cast<uintptr_t>(_address) & (_alignment - 1)) == 0;
}

// Aligns _block address to the first greater address that is aligned to
// _alignment boundaries.
template <typename _Ty>
VOX_INLINE _Ty Align(_Ty _value, size_t _alignment) {
  return static_cast<_Ty>(_value + (_alignment - 1)) & (0 - _alignment);
}
template <typename _Ty>
VOX_INLINE _Ty* Align(_Ty* _address, size_t _alignment) {
  return reinterpret_cast<_Ty*>(
      (reinterpret_cast<uintptr_t>(_address) + (_alignment - 1)) &
      (0 - _alignment));
}

// Offset a pointer from a given number of bytes.
template <typename _Ty>
_Ty* PointerStride(_Ty* _ty, size_t _stride) {
  return reinterpret_cast<_Ty*>(reinterpret_cast<uintptr_t>(_ty) + _stride);
}

size_t alignSize(size_t inSize, size_t alignment) {
    // Asset if align is not a power of 2
    assert(((alignment - 1) & alignment) == 0);
    
    const size_t alignmentMask = alignment - 1;
    
    return ((inSize + alignmentMask) & (~alignmentMask));
}

}  // namespace vox
#endif  // VOX_ALIGN_H_
