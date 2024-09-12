#ifndef ORDERBOOKLEVEL_H
#define ORDERBOOKLEVEL_H

#include <cmath>
#include <limits>

namespace OrderBook {

template <std::floating_point Precision>
struct Level {
	const Precision price;
	Precision quantity;

	// We'll probably be dealing with prices around 100,000.
	static constexpr Precision EPSILON = std::numeric_limits<Precision>::epsilon() * 100'000;

	bool operator==(const Precision& rhs) {
		return price == rhs || std::fabs(price - rhs) <= EPSILON;
	}

	bool operator>(const Precision& rhs) {
		return price > rhs && (price - rhs) > EPSILON;
	}

	bool operator>=(const Precision& rhs) {
		return price >= rhs || std::fabs(price - rhs) <= EPSILON;
	}

	bool operator<(const Precision& rhs) {
		return price < rhs && (rhs - price) > EPSILON;
	}

	bool operator<=(const Precision& rhs) {
		return price <= rhs || std::fabs(price - rhs) <= EPSILON;
	}
};

}; // namespace OrderBook

#endif // ORDERBOOKLEVEL_H
