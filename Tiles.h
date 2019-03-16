#pragma once
#include <string>
int toInt(std::string str)
{
	if (str == "0")
		return 0;
	int result = 0;
	int nums[9] = { 0 };
	for (auto &c : str) {
		auto n = c - '0' - 1;
		if (n < 0 || n > 9) {
			return -1;
		}
		nums[n]++;
	}

	for (size_t i = 0; i < 9; i++) {
		if (nums[i] < 0 || nums[i] > 4) {
			return -1;
		}
		result |= nums[i] << (i * 3);
	}

	return result;
}