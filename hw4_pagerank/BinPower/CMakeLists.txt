set(TARGET_NAME BinPower)

message(STATUS "Creating and configuration target ${TARGET_NAME}.")
add_executable (${TARGET_NAME}
        BinPower.cpp)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package(OpenMP REQUIRED)

# Include
target_include_directories(${TARGET_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../hw2_cblas/MatrixLibrary/;${PROJECT_BINARY_DIR}/../hw2_cblas/MatrixLibrary/")

# Link
target_link_libraries(${TARGET_NAME} PRIVATE "MatrixLibrary_static;OpenMP::OpenMP_CXX;Eigen3::Eigen")

