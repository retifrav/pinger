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
    "\t\tconst std::string repositoryURL = \"https://github.com/retifrav/pinger\";\n"
    "\t\tconst std::string licenseURL = \"https://www.gnu.org/licenses/gpl-3.0.en.html\";\n"
    "\t}\n}\n\n"
    "#endif // BUILD_INFO_H"
)
