#include "read_input_functions.h"

std::string ReadLine() {
	std::string s;
	getline(std::cin, s);
	return s;
}

int ReadLineWithNumber() {
	int result = 0;
	std::cin >> result;
	std::cin.ignore();
	return result;
}

std::vector<int> ReadLineWithRatings() {
	int times;
	std::cin >> times;
	std::vector<int> ratings;
	int result = 0;

	for (int i = 0; i < times; ++i) {
		std::cin >> result;
		ratings.push_back(result);

	}
	std::cin.ignore();
	return ratings;
}
