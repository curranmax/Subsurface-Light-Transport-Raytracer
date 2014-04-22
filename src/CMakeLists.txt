cmake_minimum_required (VERSION 2.6)
project (final)


# all the .cpp files that make up this project
add_executable(render
  MersenneTwister.h
  boundingbox.cpp
  boundingbox.h
  camera.cpp
  camera.h
  canvas.cpp
  canvas.h
  cone.cpp
  cone.h
  cube.cpp
  cube.h
  cylinder.cpp
  cylinder.h
  edge.cpp
  edge.h
  face.cpp
  face.h
  hash.h
  hit.h
  inputparser.h
  main.cpp
  material.cpp
  material.h
  matrix.cpp
  matrix.h
  mesh.cpp
  mesh.h
  primitive.h
  ray.h
  raytracer.cpp
  raytracer.h
  raytree.cpp
  raytree.h
  sphere.cpp
  sphere.h
  utils.h
  vbo_structs.h
  vector.h
  vertex.h
)


# platform specific compiler flags to output all compiler warnings
if (UNIX)
  if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    set_target_properties (render PROPERTIES COMPILE_FLAGS "-g -Wall -pedantic -DFreeBSD")
  else()
    set_target_properties (render PROPERTIES COMPILE_FLAGS "-g -Wall -pedantic -std=c++0x")
  endif()
endif()

if (APPLE)
set_target_properties (render PROPERTIES COMPILE_FLAGS "-g -Wall -pedantic")
endif()

if (WIN32)
set_target_properties (render PROPERTIES COMPILE_FLAGS "/W4")
endif()



# a work-around function to handle a list of libraries that include a
#  NOTFOUND library
function (add_lib_list target liblist)
  foreach (lib ${liblist})
    if (lib)
      target_link_libraries(${target} "${lib}")
    else()
      message(STATUS "WARNING: missing library: ${lib}")
    endif()
  endforeach()
endfunction()



# search for the GL & GLUT & GLEW libraries

find_package(GLUT)
if (NOT GLUT_FOUND)
  message(FATAL_ERROR "Cannot find GLUT library")
endif()
message(STATUS "Found GLUT at \"${GLUT_LIBRARIES}\"")

find_package(OpenGL)
if (NOT OPENGL_FOUND)
   message(FATAL_ERROR "Cannot find OpenGL library")
endif()
message(STATUS "Found OpenGL at \"${OPENGL_LIBRARIES}\"")

add_lib_list(render "${OPENGL_LIBRARIES}")
add_lib_list(render "${GLUT_LIBRARIES}")

if (WIN32)
  find_library(GLEW_LIBRARIES glew32 HINT "lib")
  if (NOT GLEW_LIBRARIES)
    message(FATAL_ERROR "Cannot find GLEW library")
  endif()
  message(STATUS "Found GLEW at \"${GLEW_LIBRARIES}\"")
  add_lib_list(render "${GLEW_LIBRARIES}")
endif()

#include_directories(".")
