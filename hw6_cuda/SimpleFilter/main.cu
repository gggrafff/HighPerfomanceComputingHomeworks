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
 * @brief Получение глабольного индекса потока при 2d адресации блоков и 2d адресации потоков внутри блока.
 * @details https://cs.calvin.edu/courses/cs/374/CUDA/CUDA-Thread-Indexing-Cheatsheet.pdf
 * @return Глобальный индекс нити.
 */
__device__ size_t getGlobalIdx_2D_2D() {
    size_t blockId = blockIdx.x + blockIdx.y * gridDim.x;
    size_t threadId = blockId * (blockDim.x * blockDim.y) + (threadIdx.y * blockDim.x) + threadIdx.x;
    return threadId;
}

/**
 * @brief Получение глабольного индекса потока при 1d адресации блоков и 1d адресации потоков внутри блока.
 * @details https://cs.calvin.edu/courses/cs/374/CUDA/CUDA-Thread-Indexing-Cheatsheet.pdf
 * @return Глобальный индекс нити.
 */
__device__ int getGlobalIdx_1D_1D() {
    return blockIdx.x *blockDim.x + threadIdx.x;
}

/**
 * @brief Простой фильтр, ищет взвешенное среднее значение пикселей.
 * @param d_input Массив пикселей исходного изображения размером 3 * input_width * input_height.
 * @param input_width Ширина в пикселях исходного изображения.
 * @param input_height Высота в пикселях исходного изображения.
 * @param d_output Массив пикселей отфильтрованного изображения размером 3 * input_width * input_height.
 * @param output_width Ширина в пикселях отфильтрованного изображения.
 * @param output_height Высота в пикселях отфильтрованного изображения.
 * @param filter_size Рамзер шаблона фильтра.
 * @param d_pattern Веса фильтра. Массив размером filter_size * filter_size.
 * @param pattern_sum Сумма весов фильтра.
 */
__global__ void filter(
        uint8_t *d_input,
        size_t input_width,
        size_t input_height,
        uint8_t *d_output,
        size_t output_width,
        size_t output_height,
        size_t filter_size,
        uint8_t* d_pattern,
        uint16_t pattern_sum)
{
    // Адресуемся к пикселям исходного и итогового изображений.
    auto output_center_idx = getGlobalIdx_1D_1D();
    if (output_center_idx < output_width * output_height) {
        size_t output_center_row_no = output_center_idx / output_width;
        size_t output_center_column_no = output_center_idx % output_width;
        size_t input_center_row_no = output_center_row_no + filter_size / 2;
        size_t input_center_column_no = output_center_column_no + filter_size / 2;

        // Средние занчения пикселей сохраним в переменные.
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;

        // Усредняем с весами.
        for(auto i = 0; i < filter_size; ++i){
            for(auto j = 0; j < filter_size; ++j){
                size_t pixel_idx = (j + input_center_row_no - filter_size / 2) * input_width + (i + input_center_column_no - filter_size / 2);
                r += std::abs(static_cast<int16_t>(d_input[3 * pixel_idx + 0]) * d_pattern[i * filter_size + j] / pattern_sum);
                g += std::abs(static_cast<int16_t>(d_input[3 * pixel_idx + 1]) * d_pattern[i * filter_size + j] / pattern_sum);
                b += std::abs(static_cast<int16_t>(d_input[3 * pixel_idx + 2]) * d_pattern[i * filter_size + j] / pattern_sum);
            }
        }

        // Записываем результат.
        d_output[3 * output_center_idx + 0] = r;
        d_output[3 * output_center_idx + 1] = g;
        d_output[3 * output_center_idx + 2] = b;
    }
}

int main(int argc, char *argv[])
{
    // Введение в CUDA: https://developer.nvidia.com/blog/easy-introduction-cuda-c-and-c/

    assert(argc == 5);  // SimpleFilter input.bmp output.bmp filter_size pattern.txt

    // Задаём размер фильтра.
    const size_t filter_size = std::stoul(argv[3]);
    assert(filter_size % 2 == 1);

    // Читаем исходное изображение.
    BMP input_image(argv[1]);
    assert(input_image.data.size() == 3 * input_image.bmp_info_header.width * input_image.bmp_info_header.height);

    // Создаём объект отфильтрованного изображения.
    BMP output_image(input_image.bmp_info_header.width - filter_size + 1, input_image.bmp_info_header.height - filter_size + 1, false);
    assert(output_image.data.size() == 3 * output_image.bmp_info_header.width * output_image.bmp_info_header.height);

    // Выделяем память на видеокарте и копируем туда исходное изображение.
    uint8_t* d_input, *d_output;
    cudaMalloc(&d_input, input_image.data.size());
    cudaMalloc(&d_output, output_image.data.size());
    cudaMemcpy(d_input, input_image.data.data(), input_image.data.size(), cudaMemcpyHostToDevice);

    // Читаем фильтр из файла и копируем в память видеокарты.
    uint8_t* pattern;
    uint8_t* d_pattern;
    uint16_t pattern_sum = 0;
    pattern = new uint8_t[filter_size * filter_size];
    std::ifstream pattern_file(argv[4]);
    for(auto i = 0; i < filter_size; ++i) {
        for(auto j = 0; j < filter_size; ++j) {
            uint16_t pattern_element;
            pattern_file >> pattern_element;
            pattern[i * filter_size + j] = pattern_element;
            pattern_sum += pattern_element;
            std::cout << pattern_element << " ";
        }
        std::cout << "\n";
    }
    pattern_file.close();
    if (pattern_sum == 0) {
        pattern_sum = 1;
        std::cout << "pattern_sum is zero\n";
    }
    std::cout << "pattern_sum == " << pattern_sum << "\n";
    cudaMalloc(&d_pattern, filter_size * filter_size);
    cudaMemcpy(d_pattern, pattern, filter_size * filter_size, cudaMemcpyHostToDevice);

    // Запускаем фильтрацию.
    // Про индексацию:
    // https://medium.com/@erangadulshan.14/1d-2d-and-3d-thread-allocation-for-loops-in-cuda-e0f908537a52
    // https://www.researchgate.net/post/The_optimal_number_of_threads_per_block_in_CUDA_programming
    filter<<<(output_image.data.size() + 255) / 256, 256>>>(
            d_input,
            input_image.bmp_info_header.width,
            input_image.bmp_info_header.height,
            d_output,
            output_image.bmp_info_header.width,
            output_image.bmp_info_header.height,
            filter_size,
            d_pattern,
            pattern_sum);

    // Забираем результат с видеокарты.
    cudaMemcpy(const_cast<uint8_t*>(output_image.data.data()), d_output, output_image.data.size(), cudaMemcpyDeviceToHost);

    // Сохраняем.
    output_image.write(argv[2]);

    // Очищаем память.
    cudaFree(d_input);
    cudaFree(d_output);
    delete[] pattern;
    cudaFree(d_pattern);
}