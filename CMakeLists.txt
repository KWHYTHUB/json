cmake_minimum_required(VERSION 3.10.0)

project(mat-json VERSION 0.2.0)

# This option is only useful for Sapphire itself
if (DEFINED MAT_JSON_AS_INTERFACE AND MAT_JSON_AS_INTERFACE)
    add_library(mat-json INTERFACE)

    target_sources(mat-json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/json.cpp)
    
    target_include_directories(mat-json INTERFACE include)
else()
    add_library(mat-json src/json.cpp)

    target_compile_features(mat-json PUBLIC cxx_std_20)
    
    target_include_directories(mat-json PUBLIC include)
endif()

if (PROJECT_IS_TOP_LEVEL)
    add_executable(test test.cpp)
    target_link_libraries(test mat-json)
endif()
