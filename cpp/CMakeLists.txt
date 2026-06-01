cmake_minimum_required(VERSION 3.2)
project(SquatOpt)

set(CMAKE_CXX_STANDARD 17)  # Change made here to C++17

# Find OpenSim package
find_package(OpenSim REQUIRED HINTS "${CMAKE_SOURCE_DIR}/../../../../../../")
include("${OpenSim_USE_FILE}")

# MocoAccelerationGoal Library
add_library(osimMocoAccelGoal SHARED
        MocoAccelGoal.h
        MocoAccelGoal.cpp
        osimMocoAccelGoalDLL.h
        RegisterTypes_osimMocoAccelGoal.h
        RegisterTypes_osimMocoAccelGoal.cpp)

target_link_libraries(osimMocoAccelGoal
        osimTools
        osimExampleComponents
        osimMoco)

# Define export properties for the library
string(TOUPPER MocoAccelGoal example_name_upper)
set_target_properties(osimMocoAccelGoal PROPERTIES
        DEFINE_SYMBOL OSIM${example_name_upper}_EXPORTS)

# MocoHeightGoal Library
add_library(osimMocoHeightGoal SHARED
        MocoHeightGoal.h
        MocoHeightGoal.cpp
        osimMocoHeightGoalDLL.h
        RegisterTypes_osimMocoHeightGoal.h
        RegisterTypes_osimMocoHeightGoal.cpp)

target_link_libraries(osimMocoHeightGoal
        osimTools
        osimExampleComponents
        osimMoco)

# Define export properties for the library
string(TOUPPER MocoHeightGoal example_name_upper)
set_target_properties(osimMocoHeightGoal PROPERTIES
        DEFINE_SYMBOL OSIM${example_name_upper}_EXPORTS)

# MocoHeightVelGoal Library
add_library(osimMocoHeightVelGoal SHARED
        MocoHeightVelGoal.h
        MocoHeightVelGoal.cpp
        osimMocoHeightVelGoalDLL.h
        RegisterTypes_osimMocoHeightVelGoal.h
        RegisterTypes_osimMocoHeightVelGoal.cpp)

target_link_libraries(osimMocoHeightVelGoal
        osimTools
        osimExampleComponents
        osimMoco)

# Define export properties for the library
string(TOUPPER MocoHeightVelGoal example_name_upper)
set_target_properties(osimMocoHeightVelGoal PROPERTIES
        DEFINE_SYMBOL OSIM${example_name_upper}_EXPORTS)

# MocoBalanceGoal Library
add_library(osimMocoBalanceGoal SHARED
        MocoBalanceGoal.h
        MocoBalanceGoal.cpp
        osimMocoBalanceGoalDLL.h
        RegisterTypes_osimMocoBalanceGoal.h
        RegisterTypes_osimMocoBalanceGoal.cpp)

target_link_libraries(osimMocoBalanceGoal
        osimTools
        osimExampleComponents
        osimMoco)

# Define export properties for the library
string(TOUPPER MocoBalanceGoal example_name_upper)
set_target_properties(osimMocoBalanceGoal PROPERTIES
        DEFINE_SYMBOL OSIM${example_name_upper}_EXPORTS)

# MocoBalanceGoalRight Library
add_library(osimMocoBalanceGoalRight SHARED
        MocoBalanceGoalRight.h
        MocoBalanceGoalRight.cpp
        osimMocoBalanceGoalRightDLL.h
        RegisterTypes_osimMocoBalanceGoalRight.h
        RegisterTypes_osimMocoBalanceGoalRight.cpp)

target_link_libraries(osimMocoBalanceGoalRight
        osimTools
        osimExampleComponents
        osimMoco)

# Define export properties for the library
string(TOUPPER MocoBalanceGoalRight example_name_upper)
set_target_properties(osimMocoBalanceGoalRight PROPERTIES
        DEFINE_SYMBOL OSIM${example_name_upper}_EXPORTS)

# MocoLigamentGoal Library
add_library(osimMocoLigamentGoal SHARED
        MocoLigamentGoal.h
        MocoLigamentGoal.cpp
        osimMocoLigamentGoalDLL.h
        RegisterTypes_osimMocoLigamentGoal.h
        RegisterTypes_osimMocoLigamentGoal.cpp)

target_link_libraries(osimMocoLigamentGoal
        osimTools
        osimExampleComponents
        osimMoco)

# Define export properties for the library
string(TOUPPER MocoLigamentGoal example_name_upper)
set_target_properties(osimMocoLigamentGoal PROPERTIES
        DEFINE_SYMBOL OSIM${example_name_upper}_EXPORTS)

# Include the utilities library
add_library(utilities STATIC
        utilities.h
        utilities.cpp)

# Create the executable for Study1
add_executable(Study1 Study1.cpp)

# Link the executable against the libraries
target_link_libraries(Study1
        osimMocoAccelGoal
        osimMocoHeightGoal
        osimMocoHeightVelGoal
        osimMocoBalanceGoal
        osimMocoBalanceGoalRight
        osimMocoLigamentGoal
        utilities)

# Create the executable for CAMS_TKR
add_executable(CAMS_TKR CAMS_TKR.cpp)

# Link the executable against the libraries
target_link_libraries(CAMS_TKR
        osimMocoAccelGoal
        osimMocoHeightGoal
        osimMocoHeightVelGoal
        osimMocoBalanceGoal
        osimMocoBalanceGoalRight
        osimMocoLigamentGoal
        utilities)

# Create the executable for Grid_Search
add_executable(CAMS_TKR_gridSearch CAMS_TKR_gridSearch.cpp)

# Link the executable against the libraries
target_link_libraries(CAMS_TKR_gridSearch
        osimMocoAccelGoal
        osimMocoHeightGoal
        osimMocoHeightVelGoal
        osimMocoBalanceGoal
        osimMocoBalanceGoalRight
        osimMocoLigamentGoal
        utilities)

# Create the executable for ligamentLength
add_executable(ligamentLength ligamentLength.cpp)

# Link the executable against the libraries
target_link_libraries(ligamentLength
        osimTools
        osimExampleComponents
        osimMoco
        utilities)

# For Windows: ensure DLLs for dependencies are available
OpenSimCopyDependencyDLLsForWin(DEP_NAME OpenSim
        DEP_BIN_DIR "${OpenSim_BIN_DIR}")

if(WIN32)
    add_dependencies(osimMocoAccelGoal Copy_OpenSim_DLLs)
    add_dependencies(osimMocoHeightGoal Copy_OpenSim_DLLs)
    add_dependencies(osimMocoHeightVelGoal Copy_OpenSim_DLLs)
    add_dependencies(osimMocoBalanceGoal Copy_OpenSim_DLLs)
    add_dependencies(osimMocoBalanceGoalRight Copy_OpenSim_DLLs)
    add_dependencies(osimMocoLigamentGoal Copy_OpenSim_DLLs)
endif()

# Copy files to the binary directory
file(COPY  DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
