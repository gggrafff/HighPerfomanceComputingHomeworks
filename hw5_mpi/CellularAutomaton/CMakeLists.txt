set(TARGET_NAME CellularAutomaton)

message(STATUS "Creating and configuration target ${TARGET_NAME}.")
add_executable (${TARGET_NAME}
        main.cpp)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

find_package(MPI REQUIRED)

# Include
target_include_directories(${TARGET_NAME} PRIVATE "")

# Link
target_link_libraries(${TARGET_NAME} PRIVATE "MPI::MPI_CXX")

