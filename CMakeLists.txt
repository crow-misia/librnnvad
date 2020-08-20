cmake_minimum_required(VERSION 3.5)

project(rnnvad LANGUAGES C CXX)

# C++ standard requirements.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
      "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

option(ENABLE_SSE "Enable SSE" ON)

# rnnoise
set(RNNOISE_SOURCE_FILES
	third_party/rnnoise/src/rnn_vad_weights.cc
)
add_library(rnnoise STATIC ${RNNOISE_SOURCE_FILES})
target_include_directories(rnnoise PRIVATE ${PROJECT_SOURCE_DIR})

# pffft
set(PFFFT_SOURCE_FILES
	third_party/pffft/src/pffft.c
	third_party/pffft/src/fftpack.c
)
add_library(pffft STATIC ${PFFFT_SOURCE_FILES})
target_include_directories(pffft PRIVATE ${PROJECT_SOURCE_DIR})

include(CMakePackageConfigHelpers)

set(SOURCE_FILES
	src/resampler/push_resampler.cc
	src/resampler/push_sinc_resampler.cc
	src/resampler/sinc_resampler.cc
	src/rnn_vad/auto_correlation.cc
	src/rnn_vad/common.cc
	src/rnn_vad/features_extraction.cc
	src/rnn_vad/lp_residual.cc
	src/rnn_vad/pitch_search.cc
	src/rnn_vad/pitch_search_internal.cc
	src/rnn_vad/rnn.cc
	src/rnn_vad/spectral_features.cc
	src/rnn_vad/spectral_features_internal.cc
	src/rtc_base/memory/aligned_malloc.cc
	src/rtc_base/zero_memory.cc
	src/utility/audio_util.cc
	src/utility/biquad_filter.cc
	src/utility/pffft_wrapper.cc
	src/rnn_vad.cc
)

if (ENABLE_NEON)
	add_definitions(-DWEBRTC_HAS_NEON)
	set(SOURCE_FILES ${SOURCE_FILES} src/resampler/sinc_resampler_neon.cc)
elseif(ENABLE_SSE)
	set(SOURCE_FILES ${SOURCE_FILES} src/resampler/sinc_resampler_sse.cc)
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

if (ENABLE_NEON)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -mfpu=neon)
endif()

add_subdirectory(examples)

set(LIBRNNVAD_PKG_CONFIG_FILE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/pkgconfig/rnnvad.pc)
configure_file(rnnvad.pc.in ${LIBRNNVAD_PKG_CONFIG_FILE} @ONLY)

# Private (implementation) header files.
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/third_party)

# Public (interface) header files.
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} rnnoise pffft)

# Install pkg-config files
INSTALL(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
install(FILES include/rnn_vad.h DESTINATION include/rnnvad)
install(FILES ${LIBRNNVAD_PKG_CONFIG_FILE} DESTINATION lib${LIB_SUFFIX}/pkgconfig)
