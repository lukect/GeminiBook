#ifndef ORDERBOOKSIDE_H
#define ORDERBOOKSIDE_H

#include <forward_list>
#include <optional>

#include "Level.h"
#include "Side.h"

namespace OrderBook {

template <Side Side, std::floating_point Precision>
class List {
	using Lvl = Level<Precision>;

	std::forward_list<Lvl> levels;

public:
	[[nodiscard]] bool empty() const {
		return levels.empty();
	}

	[[nodiscard]] std::optional<Lvl> topOfBook() const {
		return empty() ? std::nullopt : std::make_optional(levels.front());
	}

	[[nodiscard]] auto begin() const {
		return levels.cbegin();
	}

	[[nodiscard]] auto end() const {
		return levels.cend();
	}

	bool levelUpdate(const Precision& price, const Precision& quantity) {
		if (quantity <= 0) {
			return levelDelete(price);
		}

		auto bitr = levels.before_begin(), itr = levels.begin();
		while (itr != levels.end()) {
			if constexpr (Side == Side::BID) {
				if (price >= itr->price) {
					if (price <= itr->price) { // `<=` is better than `==`, it should match first condition if true.
						itr->quantity = quantity;
						return true;
					}
					break;
				}
			} else {
				if (price <= itr->price) {
					if (price >= itr->price) { // `<=` is better than `==`, it should match first condition if true.
						itr->quantity = quantity;
						return true;
					}
					break;
				}
			}

			bitr = itr++;
		}

		levels.emplace_after(bitr, price, quantity);
		return true;
	}

	bool levelDelete(const Precision& price) {
		auto bitr = levels.before_begin(), itr = levels.begin();
		while (itr != levels.end()) {
			if constexpr (Side == Side::BID) {
				if (price >= itr->price) {
					if (price <= itr->price) { // `<=` is better than `==`, it should match first condition if true.
						return levels.erase_after(bitr) != levels.end() || empty();
					}
					return false;
				}
			} else {
				if (price <= itr->price) {
					if (price >= itr->price) { // `<=` is better than `==`, it should match first condition if true.
						return levels.erase_after(bitr) != levels.end() || empty();
					}
					return false;
				}
			}

			bitr = itr++;
		}

		return false;
	}
};

} // namespace OrderBook

#endif // ORDERBOOKSIDE_H
