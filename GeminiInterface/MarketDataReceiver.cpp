#include "MarketDataReceiver.h"

static constexpr std::string_view baseURL = "wss://api.gemini.com/v1/marketdata/";
static constexpr std::string_view params = "?trades=false";

static std::string buildURL(const std::string& symbol) {
	std::string ret{};
	ret.reserve(baseURL.size() + symbol.size() + params.size());
	return ret.append(baseURL).append(symbol).append(params);
}

Gemini::MarketDataReceiver::MarketDataReceiver(std::string symbol, const std::uint32_t levels, std::ostream& output)
	: output(output), symbol(std::move(symbol)), url(buildURL(this->symbol)), levels(levels) {
	this->output.setf(std::ios::fixed);

	reconn_setting_t reconnection;
	reconn_setting_init(&reconnection);
	reconnection.min_delay = 500;
	reconnection.max_delay = 5000;
	reconnection.delay_policy = 2;
	client.setReconnect(&reconnection);

	client.onopen = [this]() {
		this->output << "Connection opened to " << this->url << "\nReceiving symbol \"" << this->symbol << "\""
					 << std::endl;
	};
	client.onclose = [this]() { this->output << "Connection closed! (" << this->url << ")" << std::endl; };

	client.onmessage = std::bind(&MarketDataReceiver::receiveData, this, std::placeholders::_1);

	client.open(this->url.c_str());
}

Gemini::MarketDataReceiver::~MarketDataReceiver() {
	client.onopen = client.onclose = nullptr;
	client.onmessage = nullptr;
	client.close();
}

void Gemini::MarketDataReceiver::out() const {
	auto bid = bids.begin(), ask = asks.begin();
	for (uint8_t lvl = 0; lvl < levels; ++lvl) {
		if (bid != bids.end()) {
			const auto& [price, quantity] = *bid;
			output << std::setprecision(2) << price << " " << std::setw(16) << std::setprecision(9) << quantity;
			++bid;
		} else {
			output << "NO BID";
		}

		output << "\t|\t";

		if (ask != asks.end()) {
			const auto& [price, quantity] = *ask;
			output << std::setprecision(2) << price << " " << std::setw(16) << std::setprecision(9) << quantity;
			++ask;
		} else {
			output << "NO ASK";
		}

		output << "\n";
	}

	output << std::endl;
}

void Gemini::MarketDataReceiver::receiveData(const std::string& data) {
	const simdjson::padded_string payload{data};
	auto doc = parser.iterate(payload);
	auto json = doc.get_object();

	auto events = json.find_field("events").get_array();

	for (auto event : events) {
		const double price = event.find_field("price").get_double_in_string();
		const double quantity = event.find_field("remaining").get_double_in_string();
		const OrderBook::Side side = event.find_field("side").get_string().take_value().front() == 'b'
										 ? OrderBook::Side::BID
										 : OrderBook::Side::ASK;

		if (side == OrderBook::Side::BID) {
			bids.levelUpdate(price, quantity);
		} else {
			asks.levelUpdate(price, quantity);
		}
	}

	out();
}
