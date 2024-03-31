#pragma once

#include <iostream>

template <typename Iterator>
class IteratorRange
{
public:
	IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end) {}
	IteratorRange() {}

	Iterator begin_;
	Iterator end_;

private:
};

template <typename Iterator>
class Paginator {

public:
	Paginator(Iterator begin, Iterator end, size_t page_size) {

		int pages = round(double((distance(begin, end))) / page_size);

		Iterator mid = begin;

		for (int i = 0; i < pages - 1; ++i)
		{
			pages_.push_back({ mid, mid + page_size });
			advance(mid, page_size);
		}

		pages_.push_back({ mid, end });
	}

	typename vector<IteratorRange<Iterator>> ::const_iterator begin() const {
		return pages_.cbegin();
	};
	typename vector<IteratorRange<Iterator>> ::const_iterator end() const {
		return pages_.cend();
	};

private:
	vector<IteratorRange<Iterator>> pages_;
};

template <typename Iterator>
static std::ostream& operator <<(std::ostream& out, const IteratorRange<Iterator>& document) {

	vector <typename Iterator::value_type> result{ document.begin_, document.end_ };
	cout << result;
	return out;
}

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
	return Paginator(begin(c), end(c), page_size);
}