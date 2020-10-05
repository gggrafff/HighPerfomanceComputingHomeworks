set(TARGET_NAME TimeMeasurer)

message(STATUS "Creating and configuration target ${TARGET_NAME}.")
add_executable (${TARGET_NAME}
        main.cpp)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

# Include
target_include_directories(${TARGET_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/MatrixLibrary/;${PROJECT_BINARY_DIR}/MatrixLibrary/")

# Link
target_link_libraries(${TARGET_NAME} PRIVATE "MatrixLibrary_static")

# add_compile_definitions(MatrixLibrary_shared_EXPORTS)
