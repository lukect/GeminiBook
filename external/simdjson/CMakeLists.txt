file(
        DOWNLOAD https://github.com/simdjson/simdjson/releases/download/v3.10.1/simdjson.cpp
        ${CMAKE_CURRENT_LIST_DIR}/simdjson.cpp
)
file(
        DOWNLOAD https://github.com/simdjson/simdjson/releases/download/v3.10.1/simdjson.h
        ${CMAKE_CURRENT_LIST_DIR}/simdjson.h
)

if (BUILD_STATIC)
    add_library(
            simdjson
            STATIC
            ${CMAKE_CURRENT_LIST_DIR}/simdjson.cpp
    )
else ()
    add_library(
            simdjson
            SHARED
            ${CMAKE_CURRENT_LIST_DIR}/simdjson.cpp
    )
endif ()

target_include_directories(
        simdjson
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)
