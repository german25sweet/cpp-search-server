#include "document.h"

Document::Document() : id(0), relevance(0), rating(0) {}
Document::Document(int id_, double relevance_, int _rating) : id(id_), relevance(relevance_), rating(_rating) {}

using namespace std::literals::string_literals;

std::ostream& operator <<(std::ostream& out, const Document& document) {
	out << "{ "s
		<< "document_id = "s << document.id << ", "s
		<< "relevance = "s << document.relevance << ", "s
		<< "rating = "s << document.rating
		<< " }"s;
	return out;
}

std::ostream& operator <<(std::ostream& out, const std::vector<Document>& documents) {
	for (const auto& document : documents)
	{
		out << document;
	}
	return out;
}

