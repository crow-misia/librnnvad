project(rnn_vad_tool)

find_package(PkgConfig)

# libsndfile
pkg_check_modules(SNDFILE REQUIRED sndfile)
if (SNDFILE_FOUND)
    message(STATUS "sndfile found")
else()
    message(FATAL_ERROR "sndfile not found")
endif()

add_executable(${PROJECT_NAME} rnn_vad_tool.cc)

target_link_directories (${PROJECT_NAME} PRIVATE ${SNDFILE_LIBRARY_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${SNDFILE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} rnnvad ${SNDFILE_LIBRARIES})
