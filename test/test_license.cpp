#include "license.h"
#include <iostream>

int main()
{
    std::cout << cxxide::source::license::generate_header("Simplified BSD License", { {"project_name", "test_license"}, {"author", "Nicholas Gill"}, {"year", "2013"} }) << "\n";
    std::cout << cxxide::source::license::generate_full("Simplified BSD License", { {"project_name", "test_license"}, {"author", "Nicholas Gill"}, {"year", "2013"} }) << "\n";
}
