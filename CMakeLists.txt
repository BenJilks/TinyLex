cmake_minimum_required(VERSION 3.15)
project(tinylex)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB all_SRCS
    "${PROJECT_SOURCE_DIR}/source/*.cpp"
    "${PROJECT_SOURCE_DIR}/source/generator/*.cpp"
)

add_executable(tinylex ${all_SRCS})
set_target_properties(tinylex PROPERTIES PUBLIC_HEADER 
    "tinylex/cpp/cpp_header.txt;tinylex/cpp/cpp_implement.txt")

install(TARGETS tinylex 
    RUNTIME 
        DESTINATION bin
    PUBLIC_HEADER 
        DESTINATION include/tinylex
)
