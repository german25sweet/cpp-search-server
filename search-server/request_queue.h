#pragma once

#include <vector>
#include <queue>

#include "request_queue.h"
#include "document.h"
#include "search_server.h"

class RequestQueue {
public:
	explicit RequestQueue(const SearchServer& search_server);

	// ќбертка дл€ метода поиска с предикатом документа
	template <typename DocumentPredicate>
	vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate);

	// ќбертка дл€ метода поиска с заданным статусом документа
	vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status);

	// ќбертка дл€ метода поиска без дополнительных условий
	vector<Document> AddFindRequest(const string& raw_query);

	// ѕолучение количества запросов без результатов за последние сутки
	int GetNoResultRequests() const;

private:
	struct QueryResult {
		uint64_t timestamp;
		int results;
	};

	queue<QueryResult> requests_;
	const SearchServer& search_server_;
	int no_results_requests_ = 0;
	uint64_t current_time_ = 0;
	const static int min_in_day_ = 1440;

	// ќбновление очереди запросов
	void AddRequest(int results_num);
};

// ќбертка дл€ метода поиска с предикатом документа
template <typename DocumentPredicate>
vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
	auto results = search_server_.FindTopDocuments(raw_query, document_predicate);
	AddRequest(results.empty() ? 0 : results.size());
	return results;
}
