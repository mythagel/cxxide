/* List and descriptions from cppreference */


// Utilities library
#include <cstdlib>	 // General purpose utilities: program control, dynamic memory allocation, random numbers, sort and search
#include <csignal>	 // Functions and macro constants for signal management
#include <csetjmp>	 // Macro (and function) that saves (and jumps) to an execution context
#include <cstdarg>	 // Handling of variable length argument lists
#include <typeinfo>	 // Runtime type information utilities
#include <bitset>	// std::bitset class template
#include <functional>	 // Function objects, designed for use with the standard algorithms
#include <utility>	 // Various utility components
#include <ctime>	 // C-style time/date utilites
#include <cstddef>	 // typedefs for types such as size_t, NULL and others

// Dynamic memory management
#include <new>	 // Low-level memory management utilities
#include <memory>	 // Higher level memory management utilities

// Numeric limits
#include <climits>	 // limits of integral types
#include <cfloat>	 // limits of float types
#include <limits>	 // standardized way to query properties of fundamental types

// Error handling
#include <exception>	 // Exception handling utilities
#include <stdexcept>	 // Standard exception objects
#include <cassert>	 // Conditionally compiled macro that compares its argument to zero
#include <cerrno>	 // Macro containing the last error number

// Strings library
#include <cctype>	 // functions to determine the type contained in character data
#include <cwctype>	 // functions for determining the type of wide character data
#include <cstring>	 // various narrow character string handling functions
#include <cwchar>	 // various wide and multibyte string handling functions
#include <string>	// std::basic_string class template

// Containers library
#include <vector>	// std::vector container
#include <deque>	// std::deque container
#include <list>	// std::list container
#include <set>	// std::set and std::multiset associative containers
#include <map>	// std::map and std::multimap associative containers
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
#include <numeric>	 // Numeric operations on values in containers

// Input/output library
#include <iosfwd>	 // forward declarations of all classes in the input/output library
#include <ios>	// std::ios_base class, std::basic_ios class template and several typedefs
#include <istream>	// std::basic_istream class template and several typedefs
#include <ostream>	// std::basic_ostream class template and several typedefs
#include <iostream>	// std::basic_iostream class template and several typedefs
#include <fstream>	// std::basic_fstream, std::basic_ifstream, std::basic_ofstream class templates and several typedefs
#include <sstream>	// std::basic_stringstream, std::basic_istringstream, std::basic_ostringstream class templates and several typedefs
// Disabled - deprecated
//#include <strstream>	// std::strstream, std::istrstream, std::ostrstream(deprecated)
#include <iomanip>	 // Helper functions to control the format or input and output
#include <streambuf>	// std::basic_streambuf class template
#include <cstdio>	 // C-style input-output functions

// Localization library
#include <locale>	 // Localization utilities
#include <clocale>	 // C localization utilities

// C compatibility headers
#include <ciso646>	 // empty header. The macros that appear in iso646.h in C are keywords in C++

