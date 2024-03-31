#include "search_server.h"

SearchServer::SearchServer() {}

SearchServer::SearchServer(const string& stop_words) {
	SetStopWords(SplitIntoWords(stop_words));
}

int  SearchServer::GetDocumentCount() const
{
	return static_cast<int>(documents_id.size());
}

void  SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status,
	const vector<int>& ratings) {
	if (document_id < 0) {
		throw invalid_argument("id("s + to_string(document_id) + ") меньше 0 "s);
	}
	if (document_ratings_and_status.count(document_id)) {
		throw invalid_argument("Документ с id - "s + to_string(document_id) + "уже был добавлен");
	}
	if (!IsValidWord(document)) {
		throw invalid_argument(" Наличие недопустимых символов в тексте добавляемого документа с id = " + to_string(document_id));
	}

	documents_id.push_back(document_id);

	int rating = ComputeAverageRating(ratings);

	document_ratings_and_status[document_id] = { rating, status };

	const vector<string> documentWords = SplitIntoWordsNoStop(document);
	const double words_count = 1 / static_cast<double>(documentWords.size());

	map<std::string, int> countMap;

	for (const auto& word : documentWords) {
		documents_[word][document_id] += words_count;
	}
}

int SearchServer::ComputeAverageRating(const vector<int>& ratings) {
	if (ratings.empty()) {
		return 0;
	}
	int sum = std::accumulate(ratings.begin(), ratings.end(), 0);
	return sum / static_cast<int>(ratings.size());
}

int  SearchServer::GetDocumentId(int index) const {
	return documents_id.at(index);
}

vector<Document>  SearchServer::FindTopDocuments(const string& raw_query, DocumentStatus document_status) const {
	return FindTopDocuments(raw_query, [document_status](int document_id, DocumentStatus status, int rating) { return status == document_status; });
}

tuple<vector<string>, DocumentStatus>  SearchServer::MatchDocument(const string& raw_query, int document_id) const {
	auto query = ParseQuery(raw_query);

	set<string> matched_strings;

	if (!query.minus_words_.count(document_id)) {
		for (const auto& query_word : query.query_words_) {
			if (documents_.count(query_word) > 0 && documents_.at(query_word).count(document_id)) {
				matched_strings.insert(query_word);
			}
		}
	}
	vector <string> matched_words_vector{ matched_strings.begin(), matched_strings.end() };

	return { { matched_words_vector}, {document_ratings_and_status.at(document_id).status  } };
}

bool  SearchServer::IsValidWord(const string& word) {
	return none_of(word.begin(), word.end(), [](char c) {
		return c >= '\0' && c < ' ';
		});
}

vector<string>  SearchServer::SplitIntoWordsNoStop(const string& text) const {
	vector<string> words;
	for (const string& word : SplitIntoWords(text)) {
		if (!stop_words_.count(word)) {
			words.push_back(word);
		}
	}
	return words;
}

SearchServer::Query  SearchServer::ParseQuery(const string& text) const {
	if (!IsValidWord(text)) {
		throw invalid_argument("В словах поискового запроса есть недопустимые символы");
	}
	set<string> query_words;
	set<int> minus_words;

	for (const auto& raw_query_word : SplitIntoWords(text)) {
		auto query = ParseQueryWord(raw_query_word);

		if (query.is_stop) {
			if (!stop_words_.count(query.word)) {
				query_words.insert(query.word);
			}
		}
		else if (query.is_minus) {
			if (auto it = documents_.find(query.word); it != documents_.end() && !stop_words_.count(query.word)) {
				for (const auto& [document_id, value] : it->second) {
					minus_words.insert(document_id);
				}
			}
		}
	}
	return { query_words,minus_words };
}

SearchServer::QueryWord  SearchServer::ParseQueryWord(const string& raw_query_word) const {
	if (raw_query_word == "-") {
		throw invalid_argument("Отсутствие текста после символа «минус» в поисковом запросе");
	}
	if (raw_query_word[0] != '-') {
		return { raw_query_word, true, false };
	}
	else {
		if (raw_query_word[1] == '-') {
			throw invalid_argument("Наличие более чем одного минуса у минус слов поискового запроса");
		}
		return { raw_query_word.substr(1), false, true };
	}
}

double  SearchServer::CalculateIdf(const string& word) const {
	return log(static_cast<double>(GetDocumentCount()) / static_cast<int>(documents_.at(word).size()));
}

const int SearchServer::MAX_RESULT_DOCUMENT_COUNT = 5;
const double SearchServer::EPSILON = 1e-6;