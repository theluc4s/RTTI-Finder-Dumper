#include "Finder/finder.hpp"

#include <iostream>	//std::cout
#include <memory>	//std::unique_ptr

int main()
{
	auto finder{ std::make_unique< Finder >() };
}
