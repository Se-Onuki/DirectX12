#pragma once
#include <list>
#include <cstdint>
#include <algorithm>

class ContactRecord
{
public:
	ContactRecord() = default;
	~ContactRecord() = default;

	void push_back(const uint32_t newLog) { log_.push_back(newLog); }
	void clear() { log_.clear(); }

	bool IsContact(const uint32_t newLog) const {
		return std::any_of(log_.cbegin(), log_.cend(), [newLog](const uint32_t element)
			{
				return element == newLog;
			}
		);
	}

	size_t size() const { return log_.size(); }

	std::list<uint32_t>::const_iterator begin() const { return log_.cbegin(); }
	std::list<uint32_t>::const_iterator end() const { return log_.cend(); }

private:

	std::list<uint32_t> log_;

};