#ifndef ORDERBOOKSIDE_H
#define ORDERBOOKSIDE_H

#include <cstdint>
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

	// Returns level N where update/insert/delete occurred, or 0 if nothing changed.
	std::uint32_t levelUpdate(const Precision& price, const Precision& quantity) {
		if (quantity <= 0) {
			return levelDelete(price);
		}

		auto bitr = levels.before_begin(), itr = levels.begin();
		std::uint32_t level = 1;
		while (itr != levels.end()) {
			if constexpr (Side == Side::BID) {
				if (*itr <= price) {
					if (*itr >= price) { // `<=` is better than `==`, it should match first condition if true.
						itr->quantity = quantity;
						return level;
					}
					break;
				}
			} else {
				if (*itr >= price) {
					if (*itr <= price) { // `<=` is better than `==`, it should match first condition if true.
						itr->quantity = quantity;
						return level;
					}
					break;
				}
			}

			++level;
			bitr = itr++;
		}

		levels.emplace_after(bitr, price, quantity);
		return level;
	}

	// Returns level N deleted, or 0 if nothing was deleted.
	std::uint32_t levelDelete(const Precision& price) {
		auto bitr = levels.before_begin(), itr = levels.begin();
		std::uint32_t level = 1;
		while (itr != levels.end()) {
			if constexpr (Side == Side::BID) {
				if (price >= itr->price) {
					if (price <= itr->price) { // `<=` is better than `==`, it should match first condition if true.
						return levels.erase_after(bitr) != levels.end() || empty() ? level : 0;
					}
					return 0;
				}
			} else {
				if (price <= itr->price) {
					if (price >= itr->price) { // `<=` is better than `==`, it should match first condition if true.
						return levels.erase_after(bitr) != levels.end() || empty() ? level : 0;
					}
					return 0;
				}
			}

			++level;
			bitr = itr++;
		}

		return 0;
	}
};

} // namespace OrderBook

#endif // ORDERBOOKSIDE_H
