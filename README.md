# GeminiBook

### Requirements

- GCC or LLVM/Clang (recent version with C++20 support)
- CMake 3.26 (or newer)
- A build system supported by CMake. I recommend Ninja, but Make, Visual Studio, Xcode should all work fine.

### Build

```bash
rm -rf ./build-*/ ; cmake --preset release --toolchain toolchains/standard-gcc && ninja -C build-release/
```

*`release` preset is O3 optimized.*

*`debug` preset also available (has debug symbols, no optimization).*

*You can use any of the provided toolchain configs, remove the arg (leave default) or set your own compiler.*

### Run

```
./GeminiBook <symbol> [book levels]
```

##### Example

```
./GeminiBook ETHUSD 5
```

```
./GeminiBook BTCUSD 1
```

##### Output Example

```
% ./build-release/bin/GeminiBook ETHUSD 5 
Connection opened to wss://api.gemini.com/v1/marketdata/ETHUSD?trades=false
Receiving symbol "ETHUSD"
2357.10      0.466587000	|	2357.11      4.664000000
2357.07      4.199286000	|	2357.61      0.466587000
2356.81     41.992856000	|	2357.67      4.199286000
2356.14      4.023072000	|	2357.78     21.200000000
2356.00     10.000000000	|	2357.90      4.083010000

2357.10      0.466587000	|	2357.11      4.664000000
2357.07      4.199286000	|	2357.67      4.199286000
2356.81     41.992856000	|	2357.78     21.200000000
2356.14      4.023072000	|	2357.90      4.083010000
2356.00     10.000000000	|	2358.07     41.992856000
```

*Every empty newline indicates the end of a book update.*