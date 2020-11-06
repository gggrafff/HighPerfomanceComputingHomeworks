set(TARGET_NAME Histogram)

message(STATUS "Creating and configuration target ${TARGET_NAME}.")
add_executable (${TARGET_NAME}
        main.cu
        BMP.h)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CUDA_SEPARABLE_COMPILATION ON)

# Include
target_include_directories(${TARGET_NAME} PRIVATE )

# Link
target_link_libraries(${TARGET_NAME} PRIVATE )

