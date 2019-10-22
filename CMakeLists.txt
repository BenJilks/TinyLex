cmake_minimum_required(VERSION 3.15)
project(TinyLex)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB all_SRCS
    "${PROJECT_SOURCE_DIR}/source/*.cpp"
    "${PROJECT_SOURCE_DIR}/source/generator/*.cpp"
)

add_executable(TinyLex ${all_SRCS})
set_target_properties(TinyLex PROPERTIES PUBLIC_HEADER "tinylex/header.h;tinylex/implemenation.h")
install(TARGETS TinyLex 
    RUNTIME 
        DESTINATION bin
    PUBLIC_HEADER 
        DESTINATION include/tinylex
)
