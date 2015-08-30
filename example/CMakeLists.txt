find_package(PkgConfig REQUIRED)
find_package(PostgreSQL REQUIRED)

pkg_check_modules(LIBEVENT QUIET REQUIRED libevent)
include_directories(${LIBEVENT_INCLUDE_DIRS} ${PostgreSQL_INCLUDE_DIR} "..")

add_executable(pqevent-example main.c)

link_directories(${LIBEVENT_LIBRARY_DIRS})

target_link_libraries(pqevent-example pqevent event ${PostgreSQL_LIBRARY})

