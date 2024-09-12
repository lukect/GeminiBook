add_library(
        GeminiInterface
        MarketDataReceiver.cpp
)

if (BUILD_STATIC)
    target_link_libraries(
            GeminiInterface
            hv_static
    )
else ()
    target_link_libraries(
            GeminiInterface
            hv
    )
endif ()

target_link_libraries(
        GeminiInterface
        simdjson
)
