set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CUDA_COMPILER /usr/local/cuda-10.2/bin/nvcc)
project(hw6_cuda CUDA)

set(CMAKE_CUDA_STANDARD 17)

add_subdirectory(SimpleFilter)
add_subdirectory(MedianFilter)
add_subdirectory(Histogram)