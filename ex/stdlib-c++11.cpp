/* List and descriptions from cppreference */


// Utilities library
#include <cstdlib>	 // General purpose utilities: program control, dynamic memory allocation, random numbers, sort and search
#include <csignal>	 // Functions and macro constants for signal management
#include <csetjmp>	 // Macro (and function) that saves (and jumps) to an execution context
#include <cstdarg>	 // Handling of variable length argument lists
#include <typeinfo>	 // Runtime type information utilities
#include <typeindex> // std::type_index
#include <type_traits> // Compile-time type information
#include <bitset>	// std::bitset class template
#include <functional>	 // Function objects, designed for use with the standard algorithms
#include <utility>	 // Various utility components
#include <ctime>	 // C-style time/date utilites
#include <chrono> // C++ time utilites
#include <cstddef>	 // typedefs for types such as size_t, NULL and others
#include <initializer_list> // std::initializer_list class template
#include <tuple> // std::tuple class template

// Dynamic memory management
#include <new>	 // Low-level memory management utilities
#include <memory>	 // Higher level memory management utilities
#include <scoped_allocator> // Nested allocator class

// Numeric limits
#include <climits>	 // limits of integral types
#include <cfloat>	 // limits of float types
#include <cstdint> // fixed-size types and limits of other types
#include <cinttypes> // formatting macros , intmax_t and uintmax_t math and conversions
#include <limits>	 // standardized way to query properties of fundamental types

// Error handling
#include <exception>	 // Exception handling utilities
#include <stdexcept>	 // Standard exception objects
#include <cassert>	 // Conditionally compiled macro that compares its argument to zero
#include <system_error> // defines std::error_code, a platform-dependent error code
#include <cerrno>	 // Macro containing the last error number

// Strings library
#include <cctype>	 // functions to determine the type contained in character data
#include <cwctype>	 // functions for determining the type of wide character data
#include <cstring>	 // various narrow character string handling functions
#include <cwchar>	 // various wide and multibyte string handling functions
// cuchar missing - disabled temporairly
//#include <cuchar> // C-style Unicode character conversion functions
#include <string>	// std::basic_string class template

// Containers library
#include <array> // std::array container
#include <vector>	// std::vector container
#include <deque>	// std::deque container
#include <list>	// std::list container
#include <forward_list> // std::forward_list container
#include <set>	// std::set and std::multiset associative containers
#include <map>	// std::map and std::multimap associative containers
#include <unordered_set> // std::unordered_set and std::unordered_multiset unordered associative containers
#include <unordered_map> // std::unordered_map and std::unordered_multimap unordered associative containers
#include <stack>	// std::stack container adaptor
#include <queue>	// std::queue and std::priority_queue container adaptors

// Algorithms library
#include <algorithm>	 // Algorithms that operate on containers

// Iterators library
#include <iterator>	 // Container iterators

// Numerics library
#include <cmath>	 // Common mathematics functions
#include <complex>	 // Complex number type
#include <valarray>	 // Class for representing and manipulating arrays of values
#include <random> // Random number generators and distributions
#include <numeric>	 // Numeric operations on values in containers
#include <ratio> // Compile-time rational arithmetic
#include <cfenv> // Floating-point environment access functions

// Input/output library
#include <iosfwd>	 // forward declarations of all classes in the input/output library
#include <ios>	// std::ios_base class, std::basic_ios class template and several typedefs
#include <istream>	// std::basic_istream class template and several typedefs
#include <ostream>	// std::basic_ostream class template and several typedefs
#include <iostream>	// std::basic_iostream class template and several typedefs
#include <fstream>	// std::basic_fstream, std::basic_ifstream, std::basic_ofstream class templates and several typedefs
#include <sstream>	// std::basic_stringstream, std::basic_istringstream, std::basic_ostringstream class templates and several typedefs
// Disabled - deprecated.
//#include <strstream>	// std::strstream, std::istrstream, std::ostrstream(deprecated)
#include <iomanip>	 // Helper functions to control the format or input and output
#include <streambuf>	// std::basic_streambuf class template
#include <cstdio>	 // C-style input-output functions

// Localization library
#include <locale>	 // Localization utilities
#include <clocale>	 // C localization utilities
// missing in gcc - temporary disable
//#include <codecvt> // Unicode conversion facilities

// Regular Expressions library
#include <regex> // Classes, algorithms and iterators to support regular expression processing

// Atomic Operations library
#include <atomic> // Atomic operations library

// Thread support library
#include <thread> // std::thread class and supporting functions
#include <mutex> // mutual exclusion primitives
#include <future> // primitives for asynchronous computations
#include <condition_variable> // thread waiting conditions

// C compatibility headers
#include <ciso646>	 // empty header. The macros that appear in iso646.h in C are keywords in C++
#include <ccomplex> // simply includes the header <complex>
#include <ctgmath> // simply includes the headers <ccomplex> and <cmath>: the overloads equivalent to the contents of the C header tgmath.h are already provided by <cmath> and <complex>
#include <cstdalign> // defines one compatibility macro constant
#include <cstdbool> // defines one compatibility macro constant

