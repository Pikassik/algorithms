#pragma once
#include <headers/IGraph.hpp>
#include <headers/DSU.h>
#include <memory>
#include <set>

std::shared_ptr<IGraph<double>> FindMST(const std::shared_ptr<IGraph<double>> graph);
