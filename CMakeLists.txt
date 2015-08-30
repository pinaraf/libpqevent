cmake_minimum_required(VERSION 2.6)
project(libpqevent)

set(PostgreSQL_ADDITIONAL_VERSIONS "9.4" "9.4.4")

find_package(PkgConfig REQUIRED)
find_package(PostgreSQL REQUIRED)

pkg_check_modules(LIBEVENT QUIET REQUIRED libevent)
include_directories(${LIBEVENT_INCLUDE_DIRS} ${PostgreSQL_INCLUDE_DIR})

add_executable(libpqevent main.c pqevent.c)

link_directories(${LIBEVENT_LIBRARY_DIRS})
target_link_libraries(libpqevent event ${PostgreSQL_LIBRARY})

install(TARGETS libpqevent RUNTIME DESTINATION bin)
