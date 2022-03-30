cmake_minimum_required(VERSION 3.10)

project(example_simple)

add_executable(example_simple
    main.cpp
    hello_publisher.cpp
    hello_subscriber.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/hello_world.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/hello_worldPubSubTypes.cxx
)

target_include_directories(example_simple PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
# MESSAGE(CMAKE_CURRENT_BINARY_DIR:${CMAKE_CURRENT_BINARY_DIR})

find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)
find_package(cxxopts REQUIRED)

target_link_libraries(example_simple PUBLIC fastcdr fastrtps cxxopts::cxxopts)
find_package(FastDDSGen REQUIRED)

# find fastddsgen.jar
# MESSAGE(FASTDDSGEN_PATH: ${FASTDDSGEN_PATH})
if(FASTDDSGEN_PATH)
    add_custom_target(generate_dds
        # COMMAND "java -jar ${FASTDDSGEN_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.idl" 
        COMMAND java "-jar" "${FASTDDSGEN_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/hello_world.idl" "-d" "${CMAKE_CURRENT_BINARY_DIR}"
        DEPENDS ${FASTDDSGEN_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.idl
    )
    add_dependencies(example_simple generate_dds)
endif()

# message(java " -jar" " ${FASTDDSGEN_PATH}" " ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.idl"  " -d" " ${CMAKE_CURRENT_BINARY_DIR}")

install(TARGETS example_simple
   EXPORT fastdds_demoTargets
   LIBRARY DESTINATION lib  
   ARCHIVE DESTINATION lib  
   RUNTIME DESTINATION bin 
   PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include  
)