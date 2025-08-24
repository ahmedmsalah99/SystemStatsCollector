# This assumes libpart1.cpp and libpart2.cpp are in ../src/

add_library(data_io
    ${CMAKE_CURRENT_SOURCE_DIR}/src/conf_io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stats_io.c
)

# Export include dir so executables linking this lib can use global headers
target_include_directories(data_io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_library(PROC2_LIB proc2)

if (NOT PROC2_LIB)
    message(FATAL_ERROR "proc2 library not found, please install libproc2 (or equivalent).")
endif()


target_include_directories(data_io PUBLIC /usr/include/libproc2)
target_link_libraries(data_io PUBLIC ${PROC2_LIB})