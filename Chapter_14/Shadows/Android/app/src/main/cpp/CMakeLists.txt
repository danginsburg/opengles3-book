#
# Copyright (C) The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.20)

project(Shadows "C" "CXX")
find_package(game-activity REQUIRED CONFIG)

cmake_path(SET SRC_PATH        NORMALIZE ${CMAKE_CURRENT_LIST_DIR}/../../../../..)
cmake_path(SET COMMON_PATH     NORMALIZE ${SRC_PATH}/../../Common)
cmake_path(SET COMMON_INC_PATH NORMALIZE ${COMMON_PATH}/Include)
cmake_path(SET COMMON_SRC_PATH NORMALIZE ${COMMON_PATH}/Source)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANDROID -std=c++11 -Wall -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DANDROID -Wall -Werror -Wno-unused-but-set-variable")

set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u \
    Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")

add_library(${CMAKE_PROJECT_NAME} SHARED
    ${COMMON_SRC_PATH}/esShader.c
    ${COMMON_SRC_PATH}/esShapes.c
    ${COMMON_SRC_PATH}/esTransform.c
    ${COMMON_SRC_PATH}/esUtil.c
    ${COMMON_SRC_PATH}/Android/esUtil_Android.c
    ${SRC_PATH}/${CMAKE_PROJECT_NAME}.c
    dummy.cpp)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${COMMON_INC_PATH}
    ${SRC_PATH})
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE "-DANDROID")

# add lib dependencies
target_link_libraries(${CMAKE_PROJECT_NAME}
    game-activity::game-activity_static
    EGL
    GLESv3
    android
    log)
