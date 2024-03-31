#pragma once
#include <iostream>
#include <string>
#include <vector>

enum class DocumentStatus {
	ACTUAL,
	IRRELEVANT,
	BANNED,
	REMOVED
};

struct Document {
	int id;
	double relevance;
	int rating;

	Document();
	Document(int id_, double relevance_, int _rating);
};

std::ostream& operator <<(std::ostream& out, const Document& document);

std::ostream& operator <<(std::ostream& out, const std::vector<Document>& documents);