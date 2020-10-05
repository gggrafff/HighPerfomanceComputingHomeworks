set(TARGET_NAME Jacobi)

message(STATUS "Creating and configuration target ${TARGET_NAME}.")
add_executable (${TARGET_NAME} Axisb.cpp)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

# Include
target_include_directories(${TARGET_NAME} PRIVATE "")

# Link
find_package(OpenMP REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE "OpenMP::OpenMP_CXX")