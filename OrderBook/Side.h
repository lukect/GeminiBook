#ifndef SIDE_H
#define SIDE_H

#include <cstdint>

namespace OrderBook {

enum class Side : std::uint_fast8_t {
	BID = 0,
	ASK = 1
};

}; // namespace OrderBook

#endif // SIDE_H
