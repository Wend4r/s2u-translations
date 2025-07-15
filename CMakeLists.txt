# Translations
# Copyright (C) 2024 Wend4r
# Licensed under the GPLv3 license. See LICENSE file in the project root for details.

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

if(POLICY CMP0092)
	cmake_policy(SET CMP0092 NEW) # Don't add -W3 warning level by default.
endif()

project(s2u-translations
	VERSION 1.0.0
	DESCRIPTION "Translations"
	HOMEPAGE_URL "https://github.com/Wend4r/s2u-translations"
	LANGUAGES C CXX
)

string(REGEX REPLACE "^[^-]*-" "" PROJECT_NAME_SUBSTRING "${PROJECT_NAME}")
string(TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)
string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)

set(PROJECT_OUTPUT_NAME "${PROJECT_NAME_SUBSTRING}")

set(PROJECT_AUTHOR "Wend4r")
set(PROJECT_DESCRIPTION_FULL "Source 2 util to read translations")
set(PROJECT_LICENSE "GPLv3")
string(TIMESTAMP PROJECT_BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP PROJECT_BUILD_TIME "%H:%M:%S")

set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCE_TRANSLATIONS_DIR "${SOURCE_DIR}/translations")
set(SOURCE_TRANSLATIONS_PHRASE_DIR "${SOURCE_TRANSLATIONS_DIR}/phrase")
set(SOURCE_TRANSLATIONS_PHRASE_FORMAT_DIR "${SOURCE_TRANSLATIONS_PHRASE_DIR}/format")

if(SOURCESDK_DIR)
	file(TO_CMAKE_PATH "${SOURCESDK_DIR}" SOURCESDK_DIR)
endif()

include("cmake/platform/shared.cmake")

include("cmake/sourcesdk.cmake")

set(SOURCE_FILES
	${SOURCE_TRANSLATIONS_PHRASE_DIR}/content.cpp
	${SOURCE_TRANSLATIONS_PHRASE_DIR}/format.cpp
	${SOURCE_TRANSLATIONS_DIR}/phrase.cpp
	${SOURCE_DIR}/translations.cpp
)

set(INCLUDE_DIRS
	${INCLUDE_DIR}
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 17
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_OUTPUT_NAME})

if(WINDOWS)
	set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(MACOS)
	set_target_properties(${PROJECT_NAME} PROPERTIES OSX_ARCHITECTURES "x86_64")
endif()

target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILER_OPTIONS} ${SOURCESDK_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PRIVATE ${LINK_OPTIONS} ${SOURCESDK_LINK_OPTIONS})

target_compile_definitions(${PROJECT_NAME} PRIVATE ${COMPILE_DEFINITIONS} ${SOURCESDK_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS} ${SOURCESDK_INCLUDE_DIRS})

# target_link_libraries(${PROJECT_NAME} PRIVATE ${LINK_LIBRARIES} ${SOURCESDK_LINK_LIBRARIES}) # Will be linked in a final project
