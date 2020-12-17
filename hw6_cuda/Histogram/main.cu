// Чтобы в Clion было удобнее работать, добавим некоторые определения.
#ifdef __JETBRAINS_IDE__
#define __host__
#define __device__
#define __shared__
#define __constant__
#define __global__

// This is slightly mental, but gets it to properly index device function calls like __popc and whatever.
#define __CUDACC__
#include <device_functions.h>

// These headers are all implicitly present when you compile CUDA with clang. Clion doesn't know that, so
// we include them explicitly to make the indexer happy. Doing this when you actually build is, obviously,
// a terrible idea :D
#include <__clang_cuda_builtin_vars.h>
#include <__clang_cuda_intrinsics.h>
#include <__clang_cuda_math_forward_declares.h>
#include <__clang_cuda_complex_builtins.h>
#include <__clang_cuda_cmath.h>
#endif // __JETBRAINS_IDE__


#include <fstream>
#include <cassert>

#include "BMP.h"  // https://github.com/sol-prog/cpp-bmp-images


/**
 * @brief Получение глабольного индекса потока при 1d адресации блоков и 1d адресации потоков внутри блока.
 * @details https://cs.calvin.edu/courses/cs/374/CUDA/CUDA-Thread-Indexing-Cheatsheet.pdf
 * @return Глобальный индекс нити.
 */
__device__ int getGlobalIdx_1D_1D() {
    return blockIdx.x *blockDim.x + threadIdx.x;
}

__global__ void histogram(
        uint8_t *d_input,
        size_t input_width,
        size_t input_height,
        int *d_r_hist,
        int *d_g_hist,
        int *d_b_hist,
        size_t width_of_intervals)
{
    // Адресуемся к пикселям исходного и итогового изображений.
    const int input_center_idx = getGlobalIdx_1D_1D();
    if (input_center_idx < input_width * input_height) {
        // Подсчитываем количество значений в интервалах.
        atomicAdd(d_r_hist + d_input[3 * input_center_idx + 0] / width_of_intervals, 1);
        atomicAdd(d_g_hist + d_input[3 * input_center_idx + 1] / width_of_intervals, 1);
        atomicAdd(d_b_hist + d_input[3 * input_center_idx + 2] / width_of_intervals, 1);
    }
}

int main(int argc, char *argv[])
{
    // Введение в CUDA: https://developer.nvidia.com/blog/easy-introduction-cuda-c-and-c/

    assert(argc == 3);  // Histogram input.bmp count_of_intervals

    // Задаём размер фильтра.
    const size_t count_of_intervals = std::stoul(argv[2]);
    assert(count_of_intervals <= 256);
    assert(256 % count_of_intervals == 0);
    const size_t width_of_intervals = 256 / count_of_intervals;

    // Читаем исходное изображение.
    BMP input_image(argv[1]);
    assert(input_image.data.size() == 3 * input_image.bmp_info_header.width * input_image.bmp_info_header.height);

    // Выделяем память на видеокарте и копируем туда исходное изображение.
    uint8_t *d_input;
    int *d_r_hist, *d_g_hist, *d_b_hist;
    cudaMalloc(&d_input, input_image.data.size());
    cudaMemcpy(d_input, input_image.data.data(), input_image.data.size(), cudaMemcpyHostToDevice);
    cudaMalloc(&d_r_hist, sizeof(int) * count_of_intervals);
    cudaMalloc(&d_g_hist, sizeof(int) * count_of_intervals);
    cudaMalloc(&d_b_hist, sizeof(int) * count_of_intervals);

    // Запускаем подсчёт гистограммы.
    // Про индексацию:
    // https://medium.com/@erangadulshan.14/1d-2d-and-3d-thread-allocation-for-loops-in-cuda-e0f908537a52
    // https://www.researchgate.net/post/The_optimal_number_of_threads_per_block_in_CUDA_programming
    histogram<<<(input_image.data.size() + 255) / 256, 256>>>(
            d_input,
            input_image.bmp_info_header.width,
            input_image.bmp_info_header.height,
            d_r_hist,
            d_g_hist,
            d_b_hist,
            width_of_intervals);

    // Забираем результат с видеокарты.
    int *r_hist, *g_hist, *b_hist;
    r_hist = new int[count_of_intervals];
    g_hist = new int[count_of_intervals];
    b_hist = new int[count_of_intervals];
    cudaMemcpy(r_hist, d_r_hist, count_of_intervals * sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(g_hist, d_g_hist, count_of_intervals * sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(b_hist, d_b_hist, count_of_intervals * sizeof(int), cudaMemcpyDeviceToHost);

    // Сохраняем.
    std::ofstream r_hist_out("r_hist.txt");
    std::ofstream g_hist_out("g_hist.txt");
    std::ofstream b_hist_out("b_hist.txt");
    for(size_t i = 0; i < count_of_intervals; ++i) {
        r_hist_out << r_hist[i] << " ";
        g_hist_out << g_hist[i] << " ";
        b_hist_out << b_hist[i] << " ";
    }
    r_hist_out.close();
    g_hist_out.close();
    b_hist_out.close();

    // Очищаем память.
    cudaFree(d_input);
    cudaFree(d_r_hist);
    cudaFree(d_g_hist);
    cudaFree(d_b_hist);

    delete[] r_hist;
    delete[] g_hist;
    delete[] b_hist;
}