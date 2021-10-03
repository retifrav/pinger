cmake_minimum_required(VERSION 3.15)

set(version_major 1)
set(version_minor 1)
set(version_revision 0)
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
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/include/build-info.h"
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
message(STATUS "Version: ${version_major}.${version_minor}.${version_revision}, ${version_commit}")

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

find_package(QT NAMES Qt6 COMPONENTS Quick Qml REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick Qml Charts Multimedia REQUIRED)

if(UNIX)
    if(NOT APPLE)
        find_package(Threads REQUIRED)
        find_package(X11 REQUIRED)
    endif()
endif()

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE pinger.icns)
    set(application_icon "${CMAKE_CURRENT_SOURCE_DIR}/${MACOSX_BUNDLE_ICON_FILE}")
    set_source_files_properties(${application_icon}
        PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    if(APPLE)
        qt_add_executable(${CMAKE_PROJECT_NAME}
            MACOSX_BUNDLE
            ${application_icon}
            MANUAL_FINALIZATION
        )
    elseif(WIN32)
        qt_add_executable(${CMAKE_PROJECT_NAME}
            WIN32
            resources.rc
            MANUAL_FINALIZATION
        )
    else()
        qt_add_executable(${CMAKE_PROJECT_NAME}
            MANUAL_FINALIZATION
        )
    endif()
else()
    if(APPLE)
        add_executable(${CMAKE_PROJECT_NAME}
            MACOSX_BUNDLE
            ${application_icon}
        )
    elseif(WIN32)
        add_executable(${CMAKE_PROJECT_NAME}
            WIN32
            resources.rc
        )
    else()
        add_executable(${CMAKE_PROJECT_NAME})
    endif()
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

#target_compile_definitions(${CMAKE_PROJECT_NAME}
#  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::Charts
        Qt${QT_VERSION_MAJOR}::Multimedia
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

if (APPLE)
    set_target_properties(${CMAKE_PROJECT_NAME}
        PROPERTIES
            MACOSX_BUNDLE_GUI_IDENTIFIER "dev.decovar"
            MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION} # CFBundleLongVersionString, deprecated
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION} # CFBundleShortVersionString
    )
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(${CMAKE_PROJECT_NAME})
    qt_finalize_executable(${CMAKE_PROJECT_NAME})
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
