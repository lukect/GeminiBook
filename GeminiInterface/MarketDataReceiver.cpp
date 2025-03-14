#include "MarketDataReceiver.h"

#include <regex>

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
	if (std::regex_match(this->symbol, std::regex{".*USD", std::regex_constants::icase}))
		this->pricePrecision = 2;

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
	for (uint32_t lvl = 0; lvl < levels; ++lvl) {
		if (bid != bids.end()) {
			const auto& [price, quantity] = *bid;
			output << std::setprecision(pricePrecision) << price << " " << std::setw(16) << std::setprecision(9)
				   << quantity;
			++bid;
		} else {
			output << "NO BID";
		}

		output << "\t|\t";

		if (ask != asks.end()) {
			const auto& [price, quantity] = *ask;
			output << std::setprecision(pricePrecision) << price << " " << std::setw(16) << std::setprecision(9)
				   << quantity;
			++ask;
		} else {
			output << "NO ASK";
		}

		output << "\n";
	}

	output << std::endl;
}

/*
 * This function loads the JSON message into the padded re-usable buffer and opens a document iterator.
 * The buffer is automatically resized if too small.
 * Don't pass the document to another thread (buffer and parser are thread_local).
 */
static simdjson::simdjson_result<simdjson::ondemand::document> loadJsonDocument(const std::string& json) {
	thread_local std::size_t bufferSize = 1024 * 4; // Start with 4KB.
	thread_local std::unique_ptr jsonBuffer{std::make_unique<char[]>(bufferSize)};

	const auto required =
		json.size() +
		simdjson::SIMDJSON_PADDING; // The padding acts as termination. We don't need to copy the null-terminator.

	if (required > bufferSize) {
		do { // Keep doubling bufferSize until enough.
			bufferSize *= 2;
		} while (required > bufferSize);
		jsonBuffer = std::make_unique<char[]>(bufferSize);
	}

	strncpy(jsonBuffer.get(), json.data(), json.size());

	thread_local simdjson::ondemand::parser parser{};

	return parser.iterate(jsonBuffer.get(), json.size(), bufferSize);
}

void Gemini::MarketDataReceiver::receiveData(const std::string& data) {
	auto doc = loadJsonDocument(data);
	auto json = doc.get_object();

	auto events = json.find_field("events").get_array();

	std::uint32_t maxUpdateLevel = 0;

	for (auto event : events) {
		const double price = event.find_field("price").get_double_in_string();
		const double quantity = event.find_field("remaining").get_double_in_string();
		const OrderBook::Side side = event.find_field("side").get_string().take_value().front() == 'b'
										 ? OrderBook::Side::BID
										 : OrderBook::Side::ASK;

		if (side == OrderBook::Side::BID) {
			if (auto lvl = bids.levelUpdate(price, quantity); lvl > maxUpdateLevel)
				maxUpdateLevel = lvl;
		} else {
			if (auto lvl = asks.levelUpdate(price, quantity); lvl > maxUpdateLevel)
				maxUpdateLevel = lvl;
		}
	}

	if (maxUpdateLevel == 0) {
		std::cerr << "ERROR: " << data << "\n" << std::endl;
		return;
	}

	if (maxUpdateLevel <= levels)
		out();
}
