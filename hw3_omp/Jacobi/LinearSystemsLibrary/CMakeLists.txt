include(GenerateExportHeader)

set(TARGET_NAME LinearSystemsLibrary)

message(STATUS "Creating and configuration target ${TARGET_NAME}.")

# list of source files
set(MATRIX_LIBRARY_SOURCES
        JacobiSolver.h
        JacobiSolver.cpp
        LinearSystem.h
        LinearSystem.cpp)

# this is the "object library" target: compiles the sources only once
add_library (${TARGET_NAME}_object OBJECT ${MATRIX_LIBRARY_SOURCES})

# properties
set_target_properties(${TARGET_NAME}_object PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
# shared libraries need PIC
set_property(TARGET ${TARGET_NAME}_object PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(${TARGET_NAME}_shared SHARED $<TARGET_OBJECTS:${TARGET_NAME}_object>)
add_library(${TARGET_NAME}_static STATIC $<TARGET_OBJECTS:${TARGET_NAME}_object>)

find_package(OpenMP REQUIRED)

foreach(target ${TARGET_NAME}_object ${TARGET_NAME}_shared ${TARGET_NAME}_static)
    # Include
    target_include_directories(${target} PRIVATE "${PROJECT_SOURCE_DIR}/MatrixLibrary/;${PROJECT_BINARY_DIR}/MatrixLibrary/")
    target_link_libraries(${target} "OpenMP::OpenMP_CXX;MatrixLibrary_static")
endforeach(target)

foreach(target ${TARGET_NAME}_shared ${TARGET_NAME}_static)
    # rename
    set_target_properties(${target} PROPERTIES OUTPUT_NAME ${TARGET_NAME})
endforeach(target)

add_compile_definitions(BeakLibrary_shared_EXPORTS)

GENERATE_EXPORT_HEADER(${TARGET_NAME}_shared
        BASE_NAME ${TARGET_NAME}
        EXPORT_FILE_NAME ${TARGET_NAME}_export.h
        STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)

set_target_properties(${TARGET_NAME}_static PROPERTIES COMPILE_FLAGS -DLIBSHARED_AND_STATIC_STATIC_DEFINE)
