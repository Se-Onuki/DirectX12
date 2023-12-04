#pragma once
#include <list>
#include <cstdint>
#include <algorithm>
#include <unordered_set>

class ContactRecord
{
public:
	ContactRecord() = default;
	~ContactRecord() = default;

	void push_back(const uint32_t newLog) { log_.insert(newLog); }
	void clear() { log_.clear(); }

	bool IsContact(const uint32_t newLog) const {
		return log_.contains(newLog);

	}

	size_t size() const { return log_.size(); }

	std::unordered_set<uint32_t>::const_iterator begin() const { return log_.cbegin(); }
	std::unordered_set<uint32_t>::const_iterator end() const { return log_.cend(); }

private:

	std::unordered_set<uint32_t> log_;

};