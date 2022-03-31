cmake_minimum_required(VERSION 3.10)

project(example_shared_mem)

set(RTPS_SOURCE
    message_publisher.cpp
    message_subscriber.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/image.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/imagePubSubTypes.cxx  
)

add_executable(example_shared_mem
    main.cpp
    ${RTPS_SOURCE}
)

set(CMAKE_CXX_STANDARD 17)

target_include_directories(example_shared_mem PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
# MESSAGE(CMAKE_CURRENT_BINARY_DIR:${CMAKE_CURRENT_BINARY_DIR})

find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)
find_package(cxxopts REQUIRED)

target_link_libraries(example_shared_mem PUBLIC fastcdr fastrtps cxxopts::cxxopts)
find_package(FastDDSGen REQUIRED)

# find fastddsgen.jar
# image transport test
set(OpenCV_DIR "E:/opencv_build/install")
find_package(OpenCV REQUIRED)

add_executable(transport_test
    ${RTPS_SOURCE}
    transport_test.h
    transport_test.cpp
    transport_test_main.cpp
)
target_link_libraries(transport_test PUBLIC fastcdr fastrtps cxxopts::cxxopts ${OpenCV_LIBS})
target_include_directories(transport_test PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# videcompare tools
add_executable(videocompare_tools
    video_compare.cpp
)
target_link_libraries(videocompare_tools 
    PUBLIC ${OpenCV_LIBS} cxxopts::cxxopts
)

if(FASTDDSGEN_PATH)
    set( GENERATE_SOURCE
        ${CMAKE_CURRENT_BINARY_DIR}/image.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/image.cxx
        ${CMAKE_CURRENT_BINARY_DIR}/imagePubSubTypes.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/imagePubSubTypes.cxx
    )

    add_custom_command(OUTPUT ${GENERATE_SOURCE}
        # COMMAND "java -jar ${FASTDDSGEN_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.idl" 
        COMMAND java "-jar" "${FASTDDSGEN_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/hello_world.idl"  "-d" "${CMAKE_CURRENT_BINARY_DIR}"
        COMMAND java "-jar" "${FASTDDSGEN_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/image.idl"  "-d" "${CMAKE_CURRENT_BINARY_DIR}"
        DEPENDS "${FASTDDSGEN_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.idl" "${FASTDDSGEN_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/image.idl"
    )
    #  add_dependencies(example_shared_mem generate_dds_shared_mem)
    # message(java " -jar" " ${FASTDDSGEN_PATH}" " ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.idl"  " -d" " ${CMAKE_CURRENT_BINARY_DIR}")
endif()

install(TARGETS example_shared_mem transport_test videocompare_tools
   EXPORT fastdds_demoTargets
   LIBRARY DESTINATION lib  
   ARCHIVE DESTINATION lib  
   RUNTIME DESTINATION bin 
   PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include  
)
