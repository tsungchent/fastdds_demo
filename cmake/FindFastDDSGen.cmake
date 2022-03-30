find_file(FASTDDSGEN_PATH fastddsgen.jar 
    PATHS 
    ${CMAKE_INSTALL_PREFIX}/../fastddsgen/share/fastddsgen/java 
    ${CMAKE_INSTALL_PREFIX}/share/fastddsgen/java
)

if(FASTDDSGEN_PATH)
    set(FastDDSGen-FOUND ON)
endif()