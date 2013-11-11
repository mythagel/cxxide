#include "source/license.h"
#include "cxx_gen.h"
#include <iostream>

using namespace cxxide::source;

int main()
{
    auto header = license::generate_header("Simplified BSD License", { {"project_name", "test_license"}, {"author", "Nicholas Gill"}, {"year", "2013"} });
    auto full = license::generate_full("Simplified BSD License", { {"project_name", "test_license"}, {"author", "Nicholas Gill"}, {"year", "2013"} });
    
    std::cout << cxxgen::comment_t{header, cxxgen::comment_t::style_Block}.str() << "\n";
    std::cout << full << "\n";
}

