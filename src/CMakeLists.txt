cmake_minimum_required(VERSION 3.20)

set(version_major 1)
set(version_minor 2)
set(version_revision 3)
set(version_commit "00000000000")
string(TIMESTAMP version_date UTC)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

include(GenerateVersion)
message(STATUS "Generated version: \
${version_major}.${version_minor}.\
${version_revision}, ${version_commit}")

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

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Qml Charts Multimedia REQUIRED)

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

get_target_property(QT_TARGET_TYPE Qt${QT_VERSION_MAJOR}::Core TYPE)
if(${QT_TARGET_TYPE} STREQUAL "STATIC_LIBRARY")
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS QmlImportScanner REQUIRED)
    if(${QT_VERSION_MAJOR} EQUAL 5)
        # available only from Qt 5.14
        qt5_import_qml_plugins(${CMAKE_PROJECT_NAME})
    elseif(${QT_VERSION_MAJOR} EQUAL 6)
        qt6_import_qml_plugins(${CMAKE_PROJECT_NAME})
    else()
        message(WARNING "It is unknown if Qt with major version ${QT_VERSION_MAJOR} has an import_qml_plugins function")
    endif()

    # static C++ runtime on Windows
    # not the right way to do it, apparently
    #if(WIN32)
    #    #set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
    #    #set(CMAKE_CXX_FLAGS_RELEASE "/MD")
    #    #set(CMAKE_CXX_FLAGS_DEBUG "/MDd")
    #endif()
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
            PRIVATE
                ${CMAKE_THREAD_LIBS_INIT}
                ${X11_LIBRARIES}
                ${CMAKE_DL_LIBS}
        )
    endif()
endif()

if (APPLE)
    set_target_properties(${CMAKE_PROJECT_NAME}
        PROPERTIES # https://cmake.org/cmake/help/latest/prop_tgt/MACOSX_BUNDLE_INFO_PLIST.html
            MACOSX_BUNDLE_BUNDLE_NAME "Pinger" # CFBundleIdentifier
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
