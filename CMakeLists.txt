cmake_minimum_required(VERSION 3.15)
project(template)
project(tinylex)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR})
file(GLOB all_SRCS
    "${PROJECT_SOURCE_DIR}/source/*.c"
    "${PROJECT_SOURCE_DIR}/source/generator/*.c"
)

add_executable(template "templates/template.c")
add_custom_command(TARGET template
    POST_BUILD
    COMMAND ./template ${PROJECT_SOURCE_DIR}/templates
    COMMENT "Generating templates"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

#set(CMAKE_BUILD_TYPE Debug)
add_executable(tinylex ${all_SRCS})
add_dependencies(tinylex template)

install(TARGETS tinylex 
    RUNTIME 
        DESTINATION bin
)
