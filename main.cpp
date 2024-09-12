#include <iostream>

#include <GeminiInterface/MarketDataReceiver.h>

static int usage(const char* argv0) {
	std::cerr << "Usage: " << argv0 << " <symbol> [book levels]" << std::endl;
	return -1;
}

int main(int argc, char* argv[]) {
	if (argc < 2 || argc > 3) {
		return usage(argv[0]);
	}

	std::string symbol{argv[1]};
	std::uint32_t levels{1};

	if (argc == 3) {
		try {
			levels = std::stoul(argv[2]);
		} catch (...) {
			return usage(argv[0]);
		}

		if (levels < 1)
			return usage(argv[0]);
	}
	{
		Gemini::MarketDataReceiver mdr{symbol, levels, std::cout};

		std::cin.ignore(); // Quit when ENTER is pressed
	}

	return 0;
}
