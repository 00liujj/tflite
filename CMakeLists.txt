cmake_minimum_required(VERSION 2.8)

if (NOT TOOLCHAIN)
  #set(TOOLCHAIN linux-gcc-4.9-arm-hard)
  #set(TOOLCHAIN linux-gcc-4.9-android-softfp)
  set(TOOLCHAIN linux-gcc-4.9-x86_64)
endif ()

if (NOT CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE /data/public-space/toolchains/${TOOLCHAIN}-toolchain.cmake)
endif ()

project(tflite C CXX)

if (NOT CMAKE_BUILD_TYPE)
  #set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_BUILD_TYPE Release)
  #set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#add_definitions(-DGEMMLOWP_ALLOW_SLOW_SCALAR_FALLBACK)

find_package(OpenCV)

include_directories(
#${PROJECT_SOURCE_DIR}/../../../ 
${PROJECT_SOURCE_DIR}/mock 
${PROJECT_SOURCE_DIR}/downloads/ 
${PROJECT_SOURCE_DIR}/downloads/eigen 
${PROJECT_SOURCE_DIR}/downloads/gemmlowp 
${PROJECT_SOURCE_DIR}/downloads/neon_2_sse 
${PROJECT_SOURCE_DIR}/downloads/farmhash/src 
${PROJECT_SOURCE_DIR}/downloads/flatbuffers/include 
${PROJECT_SOURCE_DIR}/gen
)



file(GLOB CORE_CC_ALL_SRCS
*.cc
kernels/*.cc
kernels/internal/*.cc
kernels/internal/optimized/*.cc
kernels/internal/reference/*.cc
*.c
kernels/*.c
kernels/internal/*.c
kernels/internal/optimized/*.c
kernels/internal/reference/*.c
downloads/farmhash/src/farmhash.cc
mock/third_party/fft2d/fftsg.c
)
file(GLOB_RECURSE CORE_CC_EXCLUDE_SRCS
*test.cc
#*/*test.cc
#*/*/*test.cc
#*/*/*/*test.cc
kernels/test_util.cc
tools/benchmark_model*.cc
)

set(CORE_SRC_FILES ${CORE_CC_ALL_SRCS})
list(REMOVE_ITEM CORE_SRC_FILES ${CORE_CC_EXCLUDE_SRCS})

if (ANDROID)
  include_directories(${ANDROID_NDK})
  list(APPEND CORE_SRC_FILES ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
endif ()

#message("CORE_CC_ALL_SRCS ${CORE_CC_ALL_SRCS}")
#message("CORE_SRC_FILES ${CORE_SRC_FILES}")
#message("CORE_CC_EXCLUDE_SRCS ${CORE_CC_EXCLUDE_SRCS}")


set(CORE_LIB tensorflowlite)
add_library(${CORE_LIB} ${CORE_SRC_FILES})
target_link_libraries(${CORE_LIB} -pthread dl)

set(BENCH benchmark_model_v2)
add_executable(${BENCH} tools/${BENCH}.cc)
target_include_directories(${BENCH} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${BENCH} ${CORE_LIB} ${OpenCV_LIBS})
install(TARGETS ${CORE_LIB} DESTINATION lib)



set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/install)
install(TARGETS ${BENCH} DESTINATION bin)
install(FILES
  ./java/demo/app/src/main/assets/mobilenet.tflite
  ./java/demo/app/src/main/assets/mobilenet_quant_v1_224.tflite
  car-00.png DESTINATION tests)

if (ANDROID)

set(JNI_SRC_FILES
  java/src/main/native/builtin_ops_jni.cc
  java/src/main/native/exception_jni.cc
  java/src/main/native/nativeinterpreterwrapper_jni.cc
  java/src/main/native/tensorflow_lite_jni.cc
  java/src/main/native/tensor_jni.cc
  java/src/main/native/duration_utils_jni.cc
)


# jni library
set(LIB tensorflowlite_jni)
add_library(${LIB} SHARED ${JNI_SRC_FILES})
target_link_libraries(${LIB} ${CORE_LIB})
set_target_properties(${LIB} PROPERTIES
  LINK_FLAGS "-Wl,-z,defs,--as-needed,--version-script=${PROJECT_SOURCE_DIR}/java/src/main/native/version_script.lds"
)


install(TARGETS ${LIB} DESTINATION lib)


# android library
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)


set(CMAKE_PREFIX_PATH /opt/jdk1.8.0_91)
find_package(Java)

include (UseJava)

file(GLOB JAVA_SRC_FILES
  java/src/main/java/org/tensorflow/lite/*.java
)

# message("JAVA_SRC_FILES is ${JAVA_SRC_FILES}")

add_jar(tensorflowlite_java ${JAVA_SRC_FILES})

install_jar(tensorflowlite_java lib/)

endif ()
