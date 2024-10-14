add_executable(example example.cpp)

target_include_directories(example PRIVATE ${FTMR_SRC_DIR})

target_link_libraries(example ftmr)

add_executable(search search_polytree.cpp)

target_include_directories(search PRIVATE ${FTMR_SRC_DIR})

target_link_libraries(search ftmr)
