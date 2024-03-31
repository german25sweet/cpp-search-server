#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>
#include <tuple>

#include "document.h"
#include "read_input_functions.h"
#include "string_processing.h"

using namespace std;

class SearchServer {

public:
	explicit SearchServer();

	template <typename StringContainer>
	explicit SearchServer(const StringContainer& stop_words);
	explicit SearchServer(const string& stop_words);

	int GetDocumentCount() const;

	void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings);

	static int ComputeAverageRating(const vector<int>& ratings);
	int GetDocumentId(int index) const;

	template<typename Predicate>
	vector<Document> FindTopDocuments(const string& raw_query, Predicate filter_func) const;
	vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus document_status = DocumentStatus::ACTUAL) const;

	tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const;

	static const int MAX_RESULT_DOCUMENT_COUNT;
	static const double EPSILON;

private:

	struct DocumentAttributes {
		int rating;
		DocumentStatus status;
	};

	set<string> stop_words_;
	vector<int> documents_id;
	map<string, map<int, double>> documents_;
	map<int, DocumentAttributes> document_ratings_and_status;

	struct Query {
		set<string> query_words_;
		set<int> minus_words_;
	};

	template <typename StringContainer>
	void SetStopWords(const StringContainer& stop_words);

	static bool IsValidWord(const string& word);

	template<typename WordContainer>
	static bool IsValidWords(const WordContainer& words);

	vector<string> SplitIntoWordsNoStop(const string& text) const;

	struct QueryWord {
		string word;
		bool is_stop = false;
		bool is_minus = false;
	};

	Query ParseQuery(const string& text) const;

	QueryWord ParseQueryWord(const string& raw_query_word) const;

	template<typename Predicate>
	vector<Document> FindAllDocuments(const Query& query, const Predicate& filter_func) const;

	double CalculateIdf(const string& word) const;
};

template <typename StringContainer>
SearchServer::SearchServer(const StringContainer& stop_words) {
	SetStopWords(stop_words);
}

template<typename Predicate>
vector<Document>  SearchServer::FindTopDocuments(const string& raw_query, Predicate filter_func) const {
	auto query = ParseQuery(raw_query);
	auto top_documents = FindAllDocuments(query, filter_func);

	sort(top_documents.begin(), top_documents.end(),
		[](const Document& lhs, const Document& rhs) {
			return (std::abs(lhs.relevance - rhs.relevance) < EPSILON) ? lhs.rating > rhs.rating : lhs.relevance > rhs.relevance;
		});

	if (static_cast<int>(top_documents.size()) > MAX_RESULT_DOCUMENT_COUNT) {
		top_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
	}

	return top_documents;
}

template <typename StringContainer>
void  SearchServer::SetStopWords(const StringContainer& stop_words) {
	if (!IsValidWords(stop_words)) {
		throw invalid_argument("Стоп слово содержит недопустимые символы");
	}
	stop_words_ = set<string>{ stop_words.begin(),stop_words.end() };
}

template<typename WordContainer>
bool SearchServer::IsValidWords(const WordContainer& words) {
	return all_of(words.begin(), words.end(), IsValidWord);
}

template<typename Predicate>
vector<Document>  SearchServer::FindAllDocuments(const Query& query, const Predicate& filter_func) const {
	map<int, double> matched_documents;
	map<string, map<int, double>> relevant_documents;

	for (const auto& query_word : query.query_words_) {
		if (documents_.count(query_word)) {
			relevant_documents[query_word] = documents_.at(query_word);
		}
	}

	for (const auto& [query_word, relev_pairs] : relevant_documents) {
		double idf = CalculateIdf(query_word);
		for (const auto& [document_id, tf_value] : relev_pairs) {
			const auto& [rating, status] = document_ratings_and_status.at(document_id);
			if (!query.minus_words_.count(document_id) && filter_func(document_id, status, rating))
				matched_documents[document_id] += tf_value * idf;
		}
	}

	vector<Document> top_documents;
	top_documents.reserve(matched_documents.size());

	for (const auto& [document_id, document_relevance] : matched_documents) {
		top_documents.push_back({ document_id, document_relevance, document_ratings_and_status.at(document_id).rating });
	}

	return top_documents;
}

