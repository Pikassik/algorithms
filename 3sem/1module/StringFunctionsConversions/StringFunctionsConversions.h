#pragma once
#include <string>
#include <vector>

std::vector<size_t> PrefixFunction(const std::string& line);

std::vector<size_t> ZFunction(const std::string& line);

std::vector<size_t> PrefixFunctionFromZFunction(const std::vector<size_t>&
                                                z_function);

std::vector<size_t> ZFunctionFromPrefixFunction(const std::vector<size_t>&
                                                prefix_function);

std::string MinStringFromPrefixFunction(const std::vector<size_t>&
                                        prefix_function);

std::string MinStringFromZFunction(const std::vector<size_t>& z_function);

