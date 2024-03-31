#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server) : search_server_(search_server), no_results_requests_(0), current_time_(0) {}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
	auto results = search_server_.FindTopDocuments(raw_query, status);
	AddRequest(results.empty() ? 0 : results.size());
	return results;
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
	auto results = search_server_.FindTopDocuments(raw_query);
	AddRequest(results.empty() ? 0 : results.size());
	return results;
}

int RequestQueue::GetNoResultRequests() const {
	return no_results_requests_;
}
void RequestQueue::AddRequest(int results_num) {
	++current_time_;
	while (!requests_.empty() && current_time_ - requests_.front().timestamp >= min_in_day_) {
		if (requests_.front().results == 0) {
			--no_results_requests_;
		}
		requests_.pop();
	}

	// Сохраняем новый результат поиска
	requests_.push({ current_time_, results_num });
	if (results_num == 0) {
		++no_results_requests_;
	}
}