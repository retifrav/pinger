cmake_minimum_required(VERSION 3.17)

project("Pinger"
    DESCRIPTION "Application for testing packet losses in your internet connection"
    VERSION 0.9.1
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

qt_add_resources(sources qml.qrc)

target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${sources}
)

#target_include_directories(${CMAKE_PROJECT_NAME}
#    PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}
#)

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
