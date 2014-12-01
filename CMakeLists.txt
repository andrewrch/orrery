cmake_minimum_required(VERSION 2.8.8)
project(Orrery C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

add_subdirectory(src)

add_executable(orrery
  ${ORRERY_SRC}
  )

target_link_libraries(orrery
  ${OpenGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLFW_STATIC_LIBRARIES}
  )

include_directories(
  include
  ${GLFW_INCLUDE_DIRS}
  )

# So we cna have debug and release builds
add_custom_target(debug
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
  )

add_custom_target(release
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Switch CMAKE_BUILD_TYPE to Release"
  )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
