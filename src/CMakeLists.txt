cmake_minimum_required(VERSION 3.5)

set(PATH ${CMAKE_SOURCE_DIR}/include)

add_executable(test main.cpp inverted_index.cpp ${PATH}/inverted_index.h ${PATH}/threadPool.h)

target_link_directories(test PRIVATE ${PATH})

install(TARGETS test
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
