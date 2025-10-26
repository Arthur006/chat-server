add_executable(server
  src/main.cpp
  src/server.cpp
  src/clienthandler.cpp
  src/connectionhandler.cpp
  src/room.cpp
)

target_include_directories(server
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/include
)
