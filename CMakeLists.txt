# root/cmakelists.txt

cmake_minimum_required(VERSION 3.22)
project(doMINION)

# Reenable this to make debug builds the default
# set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EXTERNAL_LIBS ${CMAKE_SOURCE_DIR}/external)

enable_testing()

# always provide test output on failure
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure" "--timeout" "10")

# exposes the root dir path to all (cpp) project files
add_compile_definitions(PROJECT_ROOT="${CMAKE_SOURCE_DIR}") 

# Uncomment to disable fullscreen mode
#
# For some reason, the game is not displayed correctly in fullscreen mode on some systems
# Enabling this flag might fix the issue
#
# add_definitions(-DNO_FULLSCREEN)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wreturn-type
    -Wshadow
    -Wuninitialized
    # TODO: Remove this suppression
    -Wno-deprecated-enum-enum-conversion
    # This is a warning that exists on more recent versions of clang
    # We don't care too much about shadowing
    -Wno-shadow
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(
        -Werror
    )

    # Enable sanitizers
    set(SAN_FLAG "-fsanitize=address,signed-integer-overflow,undefined")
    if(APPLE)
        # Check if the compiler supports sanitizers
        set(SANITIZER_FLAGS "${SAN_FLAG}") # Exclude 'leak' (ASan handles it on macOS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS} -g") # Add debug symbols
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_FLAGS}")

        # Optional: Suppress certain warnings for better output readability
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
    else()
        # Non-Apple platform configuration (e.g., Linux)
        set(SANITIZER_FLAGS "${SAN_FLAG},leak")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
    endif()

    # Necessary to output stack traces
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
endif()

add_subdirectory(${EXTERNAL_LIBS}/sockpp)
add_subdirectory(${EXTERNAL_LIBS}/googletest) # warnings because of old cmake version, cant supress them

include_directories(external/sockpp/include)

find_package(wxWidgets COMPONENTS core base net REQUIRED)
if (wxWidgets_FOUND)
    message(STATUS "Including wxWidgets for ${PROJECT_NAME}")
    include(${wxWidgets_USE_FILE})  
else()
    message(FATAL_ERROR "wxWidgets not found; ensure it is installed! (use './install_wx_widgets.sh')")
endif()

# Parallel builds by default

include(ProcessorCount)
ProcessorCount(NUM_PROCS)

if (NUM_PROCS EQUAL 0)
    message(WARNING "Processor count could not be determined; defaulting to 1.")
    set(NUM_PROCS 1)
endif()

message(STATUS "Number of processors: ${NUM_PROCS}")

################################
# MACROS
################################
macro(include_rapidjson target)
    message(STATUS "Including rapidjson in ${target}")
    target_include_directories(${target} PUBLIC ${EXTERNAL_LIBS}/rapidjson/include)
endmacro()

macro(include_sockpp target)
    message(STATUS "Including sockpp in ${target}")
    target_link_libraries(${target} PRIVATE sockpp)

    if (WIN32)
        message(WARNING "why would you use windows??")
        target_link_libraries(${target} PRIVATE wsock32 ws2_32)
    else()
        set(THREADS_PREFER_PTHREAD_FLAG ON)
        find_package(Threads REQUIRED)
        target_link_libraries(${target} PRIVATE Threads::Threads)
    endif()
endmacro()

macro(include_gtest target)
    message(STATUS "Including gtest in ${target}")
    target_link_libraries(${target} PRIVATE gtest gtest_main)
    target_include_directories(${target} PRIVATE
        ${EXTERNAL_LIBS}/googletest/lib/googletest/include
    )
endmacro()

macro(include_gmock target)
    message(STATUS "Including gmock in ${target}")
    target_link_libraries(${target} PRIVATE gmock gmock_main)
    target_include_directories(${target} PRIVATE
        ${EXTERNAL_LIBS}/googletest/lib/googlemock/include
    )
endmacro()

macro(include_quick_arg_parser target)
    message(STATUS "Including quick_arg_parser in ${target}")
    target_include_directories(${target} PRIVATE ${EXTERNAL_LIBS}/quick_arg_parser)
    target_compile_options(${target} PRIVATE -Wno-unused-parameter)
endmacro()

macro(include_wxwidgets target)
    if (wxWidgets_FOUND)
        message(STATUS "Including wxWidgets in ${target}")
        target_link_libraries(${target} PRIVATE ${wxWidgets_LIBRARIES})
        target_include_directories(${target} PRIVATE ${wxWidgets_INCLUDE_DIRS})
        target_compile_options(${target} PRIVATE ${wxWidgets_CXX_FLAGS})  # Add wx-config flags
    else()
        message(FATAL_ERROR "wxWidgets not found; skipping ${target}")
    endif()
endmacro()

macro(include_library target library_name)
    message(STATUS "Including ${library_name} in ${target}")
    target_link_libraries(${target} PRIVATE ${library_name})
    string(REPLACE "_lib" "" library_dir ${library_name})
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/modules/${library_dir}/include)
endmacro()

macro(include_server_lib target)
    include_library(${target} server_lib)
endmacro()

macro(include_client_lib target)
    include_library(${target} client_lib)
endmacro()

macro(include_shared_lib target)
    include_library(${target} shared_lib)
endmacro()

################################
# MODULES
################################
add_subdirectory(modules/shared)
add_subdirectory(modules/client)
add_subdirectory(modules/server)
add_subdirectory(unit_tests)

################################
# EXECUTABLES
################################
add_executable(client_exe ${CLIENT_EXECUTABLE_SOURCES})
include_client_lib(client_exe)
include_shared_lib(client_exe)
include_sockpp(client_exe)
include_wxwidgets(client_exe)
include_rapidjson(client_exe)

add_executable(server_exe ${SERVER_EXECUTABLE_SOURCES})
include_shared_lib(server_exe)
include_server_lib(server_exe)
include_sockpp(server_exe)
include_wxwidgets(server_exe)
include_rapidjson(server_exe)

################################
# HELPERS
################################

add_custom_target(run_server
    COMMAND ${CMAKE_COMMAND} -E echo "Starting server..."
    COMMAND ${CMAKE_BINARY_DIR}/server_exe
    COMMENT "Running server_exe"
    USES_TERMINAL
)

add_custom_target(run_client
    COMMAND ${CMAKE_COMMAND} -E echo "Starting client..."
    COMMAND ${CMAKE_BINARY_DIR}/client_exe
    COMMENT "Running client_exe"
    USES_TERMINAL
)

add_custom_target(format
    COMMENT "Running clang-format and checking for formatting issues..."
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND clang-format --version
    # This is a hacky workaround to return a non-zero exit code if there are formatting issues
    COMMAND git diff --exit-code > /dev/null
        || (echo "It seems you have some unstaged changes. Please commit or stash them before running this command." && git diff --stat && exit 1)
    COMMAND find modules unit_tests -name '*.cpp' -o -name '*.h' | xargs clang-format -i
    COMMAND git diff --exit-code || (echo "Formatting issues found!" && exit 1) 
)

add_custom_target(check
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/run-cppcheck.sh ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/compile_commands.json

    COMMAND clang-tidy --version
    COMMAND clang-tidy --list-checks
    COMMAND run-clang-tidy
        -p ${CMAKE_BINARY_DIR}
        -j ${NUM_PROCS}
        -quiet
        -fix
        modules
        unit_tests
)

# copy assets (images) to binary directory
file(INSTALL assets DESTINATION ${CMAKE_BINARY_DIR})
