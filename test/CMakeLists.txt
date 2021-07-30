cmake_minimum_required(VERSION 3.14)
project(webserv)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories("../srcs")

# test source
file(GLOB TEST_SRC ./*.cpp)

# source files
# ホントはワイルドカードでやりたいけど、main.cppが入っちゃってテストのmainと干渉しちゃう
# file(GLOB WEBSERV_SRC ../srcs/*.cpp)
set(SRC_DIR ../srcs)
set(WEBSERV_SRC
  ${SRC_DIR}/HttpMessage.cpp
  ${SRC_DIR}/Response.cpp
  ${SRC_DIR}/Request.cpp
  ${SRC_DIR}/utility.cpp
  ${SRC_DIR}/CGI.cpp
  ${SRC_DIR}/Client.cpp
  ${SRC_DIR}/Socket.cpp
  ${SRC_DIR}/Server.cpp
)

add_executable(
  WebservTest
  ${TEST_SRC}
  ${WEBSERV_SRC}
)

target_link_libraries(
  WebservTest
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(WebservTest)