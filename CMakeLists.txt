# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.4.3)
project(CCov)

find_package(LLVM REQUIRED CONFIG)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

add_subdirectory(CCov)
