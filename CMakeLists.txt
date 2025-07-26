cmake_minimum_required(VERSION 3.20)
project(cs2_yzx)
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/minhook/share/minhook)

find_package(minhook)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/imgui imgui_src)
aux_source_directory(./src cheat_src)
file(GLOB dumper_header_files ./cs2dumper/*.hpp)
message(${imgui_src})
message(${cheat_src})
message(${dumper_header_files})
add_library(${PROJECT_NAME} SHARED dllmain.cpp ${imgui_src} ${cheat_src} ${dumper_header_files})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/imgui includes)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE d3d11.lib dxgi.lib minhook::minhook)

target_precompile_headers(${PROJECT_NAME} PUBLIC ./pch.h)