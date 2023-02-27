#include "option.h"
#include "third-party/toml.hpp"

Option::Option(const std::string &toml_file)
{
    auto cfg = toml::parse_file(toml_file);
    
}