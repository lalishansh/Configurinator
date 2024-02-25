CMAKE_MINIMUM_REQUIRED(VERSION 3.21)

SET(target_dir ${CMAKE_CURRENT_LIST_DIR})
GET_FILENAME_COMPONENT(target_target "${target_dir}" NAME)

IF (TARGET ${target_target})
    MESSAGE(WARNING "TARGET ${target_target} already exists, skipping this directory for this target")
    RETURN()
ENDIF ()

MESSAGE(STATUS "Building target ${target_target} in ${target_dir}")
FILE(GLOB_RECURSE target_src_files
     # header files
     "${target_dir}/src/**.h"
     "${target_dir}/src/**.hh"
     "${target_dir}/src/**.hpp"
     "${target_dir}/src/**.hxx"
     "${target_dir}/src/**.ixx" # Cpp20
     # source files
     "${target_dir}/src/**.c"
     "${target_dir}/src/**.cc"
     "${target_dir}/src/**.cpp"
     "${target_dir}/src/**.cxx"
     "${target_dir}/src/**.cppm" # Cpp20
)

ADD_EXECUTABLE(${target_target} ${target_src_files})

SET_TARGET_PROPERTIES(${target_target}
                      PROPERTIES
                      SOURCE_DIR ${target_dir}
)

TARGET_INCLUDE_DIRECTORIES(${target_target}
                           PRIVATE
                           ${target_dir}/
                           ${target_dir}/src/
                           PUBLIC
                           ${GENERATED_SOURCE_FILES_DIR}
)

#[[TARGET_COMPILE_OPTIONS(${target_target}
                       PRIVATE
                       $<$<CXX_COMPILER_ID:MSVC>:/WX /W4>
                       $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Wconversion -Werror>
)]]
TARGET_COMPILE_DEFINITIONS(${target_target}
                           PRIVATE
                           $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
)

# from ~vendor dir
TARGET_INCLUDE_DIRECTORIES(${target_target} PRIVATE
                           ${target_dir}
                           ${CMAKE_SOURCE_DIR}/~vendor/include
)

# VcPkg packages
IF (NOT TARGET fmt::fmt)
    FIND_PACKAGE(fmt CONFIG REQUIRED)
ENDIF ()
IF (NOT TARGET Boost::program_options)
    FIND_PACKAGE(Boost REQUIRED COMPONENTS program_options)
ENDIF ()
IF (NOT TARGET nlohmann_json::nlohmann_json)
    FIND_PACKAGE(nlohmann_json CONFIG REQUIRED)
ENDIF ()

TARGET_LINK_LIBRARIES(${target_target}
                      PRIVATE
                      fmt::fmt
                      ${Boost_LIBRARIES}
                      nlohmann_json::nlohmann_json
)

# Other packages
IF (NOT TARGET ftxui::screen)
    INCLUDE(FetchContent)

    FETCHCONTENT_DECLARE(ftxui
                         GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
                         GIT_TAG 65bbb4f
    )

    FETCHCONTENT_GETPROPERTIES(ftxui)
    IF (NOT ftxui_POPULATED)
        FETCHCONTENT_POPULATE(ftxui)
        SET(FTXUI_BUILD_EXAMPLES ON)
        ADD_SUBDIRECTORY(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
    ENDIF ()
ENDIF ()
TARGET_LINK_LIBRARIES(${target_target}
                      PRIVATE
                      ftxui::screen
                      ftxui::dom
                      ftxui::component
)

# tests target
ADD_TESTS_FOR_TARGET(${target_target}
                     INCLUDE_DIRECTORIES
                     COMPILE_DEFINITIONS
                     COMPILE_OPTIONS
                     LINK_LIBRARIES
                     LINK_DIRECTORIES
)