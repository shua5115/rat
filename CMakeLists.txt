project(rat)

# Set variables LUA_INCLUDE_DIR and LUA_LIBRARY, defined by the Lua51 package on Ubuntu.
if (WIN32)    
    # Make sure to set this to where your Lua files are located.
    # At time of writing, these are the default paths after installing Lua for Windows,
    # which installs a 32-bit version of Lua 5.1.
    set(LUA_INCLUDE_DIR "C:/Program Files (x86)/Lua/5.1/include")
    find_library(LUA_LIBRARY lua51 "C:/Program Files (x86)/Lua/5.1/lib/")
	#set(LUA_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/lua51.lib)
elseif(LINUX)
    find_package(Lua51 REQUIRED)
    # set(LUA_INCLUDE_DIR "/usr/local/include")
    # find_library(LUA_LIBRARY liblua5.1 "/lib" "/usr/lib")
endif()
message(STATUS "Lua library: " ${LUA_LIBRARY})
message(STATUS "Lua include dir: " ${LUA_INCLUDE_DIR})

add_library(rat SHARED rat_lua.c)
set_target_properties(rat PROPERTIES PREFIX "")
target_include_directories(rat PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(rat PRIVATE ${LUA_LIBRARY})

add_executable(test_rat test_rat.c rat.c)