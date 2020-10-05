project(Jacobi)

add_subdirectory(MatrixLibrary)
add_subdirectory(LinearSystemsLibrary)

set(TARGET_NAME Jacobi)

message(STATUS "Creating and configuration target ${TARGET_NAME}.")
add_executable (${TARGET_NAME} Axisb.cpp)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

# Include
target_include_directories(${TARGET_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/MatrixLibrary/;${PROJECT_BINARY_DIR}/MatrixLibrary/;${PROJECT_SOURCE_DIR}/LinearSystemsLibrary/;${PROJECT_BINARY_DIR}/LinearSystemsLibrary/")

# Link
find_package(OpenMP REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE "MatrixLibrary_static;LinearSystemsLibrary_static")