cmake_minimum_required(VERSION 3.1...3.28)

add_executable(data_parser_test data_parser_test.cpp)

message(STATUS ${PROJECT_SOURCE_DIR})
message(STATUS "CMAKE_GENERATOR == ${CMAKE_GENERATOR}" )
target_include_directories(data_parser_test PUBLIC ${PROJECT_SOURCE_DIR}/data_parser_lib)
target_link_libraries(data_parser_test data_parser_lib)

if(WIN32)
    string(FIND ${CMAKE_GENERATOR} "Visual Studio" index)
    message(STATUS "index==${index}")
    if(${index} EQUAL "0")
        string(REPLACE "/" "\\" input_file_path "${CMAKE_SOURCE_DIR}/docs/S11.s1p")
        set_target_properties(data_parser_test PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS ${input_file_path})
    endif()
elseif(LINUX)
    #set_target_properties(data_parser_test VS_DEBUGGER_COMMAND_ARGUMENTS "${CMAKE_SOURCE_DIR}/docs/S11.s1p")
endif()    