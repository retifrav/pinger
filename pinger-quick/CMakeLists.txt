cmake_minimum_required(VERSION 3.15)

set(version_major 1)
set(version_minor 0)
set(version_revision 1)
set(version_commit "00000000000")
string(TIMESTAMP version_date UTC)

execute_process(
    COMMAND git rev-parse --short=11 HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../"
    RESULT_VARIABLE gitResult
    ERROR_VARIABLE gitError
    OUTPUT_VARIABLE gitOut
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT gitResult EQUAL 0)
    message(WARNING "Git command status: ${gitResult}")
    if(NOT gitError STREQUAL "")
        message(WARNING "${gitError}")
    endif()
    message(WARNING "Couldn't get commit hash from Git. Will fallback to default value")
else()
    #message(${gitOut})
    #string(REPLACE "build-" "" gitBuildNumber ${gitOut})
    #set(version_commit ${gitBuildNumber})
    set(version_commit ${gitOut})
endif()
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/include/build_info.h"
    "#ifndef BUILD_INFO_H\n"
    "#define BUILD_INFO_H\n\n"
    "namespace decovar\n{\n"
    "\tnamespace pinger\n\t{\n"
    "\t\tconst int versionMajor = ${version_major};\n"
    "\t\tconst int versionMinor = ${version_minor};\n"
    "\t\tconst int versionRevision = ${version_revision};\n"
    "\t\tconst std::string versionCommit = \"${version_commit}\";\n"
    "\t\tconst std::string versionDate = \"${version_date}\";\n"
    "\t}\n}\n\n"
    "#endif // BUILD_INFO_H"
)

project("Pinger"
    DESCRIPTION "Network connection quality analyzer"
    VERSION ${version_major}.${version_minor}.${version_revision}
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# for Sublime Text LSP
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    SET(PROGRAMS-PREFIX "d:/programs")
else()
    SET(PROGRAMS-PREFIX "~/programs")
endif()

set(CMAKE_PREFIX_PATH "${PROGRAMS-PREFIX}/qt")
find_package(Qt5 REQUIRED Quick Qml Charts Multimedia)

if(UNIX)
    if(NOT APPLE)
        find_package(Threads REQUIRED)
        find_package(X11 REQUIRED)
    endif()
endif()

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE pinger.icns)
    set_source_files_properties(${MACOSX_BUNDLE_ICON_FILE}
        PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
    add_executable(${CMAKE_PROJECT_NAME} MACOSX_BUNDLE ${MACOSX_BUNDLE_ICON_FILE})
elseif(WIN32)
    add_executable(${CMAKE_PROJECT_NAME} WIN32 resources.rc)
else()
    add_executable(${CMAKE_PROJECT_NAME})
endif()

set(sources
    main.cpp
    functions.cpp
    pingdata.cpp
    backend.cpp
)

set(files_for_ide
    backend.h
    functions.h
    pingdata.h
)

qt_add_resources(sources qml.qrc)

target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${sources}
        ${files_for_ide}
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        #${CMAKE_CURRENT_SOURCE_DIR}
        "${PROJECT_BINARY_DIR}/include/"
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    Qt5::Quick
    Qt5::Qml
    Qt5::Charts
    Qt5::Multimedia
)

if (UNIX)
    if (APPLE)
        # target_link_libraries(${CMAKE_PROJECT_NAME}
        #     "-framework Cocoa"
        #     "-framework IOKit"
        #     "-framework CoreVideo"
        # )
    else()
        target_link_libraries(${CMAKE_PROJECT_NAME}
            ${CMAKE_THREAD_LIBS_INIT}
            ${X11_LIBRARIES}
            ${CMAKE_DL_LIBS}
            )
    endif()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
