cmake_minimum_required(VERSION 2.4)

set(EXECUTABLE_OUTPUT_PATH "./bin")

project(PGM_Toolkit)

if(NOT MSVC)
   set(CMAKE_C_FLAGS "-std=c11")
endif()

#set(CMAKE_VERBOSE_MAKEFILE on)
#set(CMAKE_BUILD_TYPE Debug)

#Keine Optimisation
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

include_directories(usr/include)

# A1
set(SOURCES_A1
   src/p1.c
   src/pgmIO.c
)
add_executable(p1 ${SOURCES_A1})

# A2
set(SOURCES_A2
   src/p2.c
   src/pgmIO.c
)
add_executable(p2 ${SOURCES_A2})

# A3
set(SOURCES_A3
   src/p3.c
   src/pgmIO.c
)
add_executable(p3 ${SOURCES_A3})

# A4
set(SOURCES_A4
   src/p4.c
   src/pgmIO.c
)
add_executable(p4 ${SOURCES_A4})

target_link_libraries(p4 m)
