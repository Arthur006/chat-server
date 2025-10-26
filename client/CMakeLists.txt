add_executable(client
  src/main.cpp
  src/test.cpp
)

target_include_directories(client
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/include
)
