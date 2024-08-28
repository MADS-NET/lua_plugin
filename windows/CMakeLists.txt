cmake_minimum_required(VERSION 3.20)
project(lua VERSION 5.4.3 LANGUAGES C)

if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

set(LIB_SOURCES lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c lbaselib.c lcorolib.c ldblib.c liolib.c lmathlib.c loadlib.c loslib.c lstrlib.c ltablib.c lutf8lib.c linit.c)

list(TRANSFORM LIB_SOURCES PREPEND ${SRC_DIR}/)

add_library(lua STATIC ${LIB_SOURCES})

add_executable(lua_exec ${SRC_DIR}/lua.c)
target_link_libraries(lua_exec lua)
set_target_properties(lua_exec PROPERTIES OUTPUT_NAME lua)

add_executable(luac ${SRC_DIR}/luac.c)
target_link_libraries(luac lua)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/..")
install(TARGETS lua lua_exec luac
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)