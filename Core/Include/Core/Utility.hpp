#pragma once

#include <random>

namespace BoostAsioPractice {
	template<typename Type>
	int RandomNumber(Type min, Type max)
	{
		static std::random_device rd;
		static std::mt19937 mt(rd());
		std::uniform_int_distribution<Type> result(min, max);
		return result(mt);
	}
}