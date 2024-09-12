#ifndef MARKETDATARECEIVER_H
#define MARKETDATARECEIVER_H

#include <string>

#include <hv/WebSocketClient.h>
#include <simdjson.h>

#include <OrderBook/List.h>

namespace Gemini {

class MarketDataReceiver {
	std::ostream& output;
	hv::WebSocketClient client{};

	simdjson::ondemand::parser parser{};

protected:
	std::string symbol;
	std::string url;
	OrderBook::List<OrderBook::Side::BID, double> bids{};
	OrderBook::List<OrderBook::Side::ASK, double> asks{};
	uint8_t levels;

	void out() const;
	void receiveData(const std::string& data);

public:
	explicit MarketDataReceiver(std::string symbol, std::uint32_t levels, std::ostream& output);
	~MarketDataReceiver();
};

} // namespace Gemini

#endif // MARKETDATARECEIVER_H
