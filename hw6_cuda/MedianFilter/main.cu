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
 * @brief Простая сортировка элементов массива за n^2.
 * @details Эффективно только для маленьких массивов.
 * @details На маленьких массивах накладные расходы более сложных алгоритмов сортировки не всегда себя окупают.
 * @param arr Массив.
 * @param size Размер массива.
 */
__device__ void sort(uint16_t *arr, size_t size)
{
    for (int i = 0; i < size; i++) {
        for(int j = i + 1; j < size; j++) {
            if (arr[j] < arr[i]) {
                uint16_t temp = arr[j];
                arr[j] = arr[i];
                arr[i] = temp;
            }
        }
    }
}


/**
 * @brief Медианный фильтр.
 * @param d_input Массив пикселей исходного изображения размером 3 * input_width * input_height.
 * @param input_width Ширина в пикселях исходного изображения.
 * @param input_height Высота в пикселях исходного изображения.
 * @param d_output Массив пикселей отфильтрованного изображения размером 3 * input_width * input_height.
 * @param output_width Ширина в пикселях отфильтрованного изображения.
 * @param output_height Высота в пикселях отфильтрованного изображения.
 * @param filter_size Размер области для поиска медианы (квадратная область filter_size * filter_size).
 */
__global__ void filter(
        uint8_t *d_input,
        size_t input_width,
        size_t input_height,
        uint8_t *d_output,
        size_t output_width,
        size_t output_height,
        size_t filter_size)
{
    /**
     * Прочитал, что обращение к shared памяти может быть быстрее в сотни раз, чем обращение к global памяти.
     * В случаях, когда обращение к элементам производится более одного раза,
     * предварительное копирование элементов в shared память часто даёт хороший прирост производительности.
     * Решил попробовать копировать элементы из global в shared перед дальнейшей работой.
     * Dynamic shared memory допускает использование только одного массива в kernel.
     * Кроме того, необходимо использовать клетки-призраки на границах блоков.
     * Эти факторы привели к довольно страшной индексации элементов.
     */

    // Адресуемся к пикселям исходного и итогового изображений.
    const int output_center_column_no = blockIdx.x * blockDim.x + threadIdx.x;
    const int output_center_row_no = blockIdx.y * blockDim.y + threadIdx.y;
    if ((output_center_column_no < output_width) && (output_center_row_no < output_height)) {
        size_t input_center_row_no = output_center_row_no + filter_size / 2;
        size_t input_center_column_no = output_center_column_no + filter_size / 2;

        // массивы пикселей, за которые отвечает данный блок, с клетками-призраками
        extern __shared__ uint8_t pixels[];
        size_t pixels_width = blockDim.x + (filter_size / 2) * 2;
        size_t pixels_height = pixels_width;
        uint8_t* r = pixels;
        uint8_t* g = pixels + pixels_width * pixels_height;
        uint8_t* b = pixels + 2 * pixels_width * pixels_height;

        if (threadIdx.x > blockDim.x - filter_size / 2 - 1 && threadIdx.y > blockDim.y - filter_size / 2 - 1) {
            // правые нижние клетки-призраки
            size_t pixel_idx = (input_center_row_no + filter_size / 2) * input_width + input_center_column_no + filter_size / 2;
            r[(threadIdx.x + (filter_size / 2) * 2) * pixels_width + threadIdx.y + (filter_size / 2) * 2] = d_input[3 * pixel_idx + 0];
            g[(threadIdx.x + (filter_size / 2) * 2) * pixels_width + threadIdx.y + (filter_size / 2) * 2] = d_input[3 * pixel_idx + 1];
            b[(threadIdx.x + (filter_size / 2) * 2) * pixels_width + threadIdx.y + (filter_size / 2) * 2] = d_input[3 * pixel_idx + 2];
        }
        if (threadIdx.x > blockDim.x - filter_size / 2 - 1 && threadIdx.y < filter_size / 2) {
            // правые верхние клетки-призраки
            size_t pixel_idx = (input_center_row_no - filter_size / 2) * input_width + (input_center_column_no + filter_size / 2);
            r[(threadIdx.x + (filter_size / 2) * 2) * pixels_width + threadIdx.y] = d_input[3 * pixel_idx + 0];
            g[(threadIdx.x + (filter_size / 2) * 2) * pixels_width + threadIdx.y] = d_input[3 * pixel_idx + 1];
            b[(threadIdx.x + (filter_size / 2) * 2) * pixels_width + threadIdx.y] = d_input[3 * pixel_idx + 2];
        }
        if (threadIdx.x < filter_size / 2 && threadIdx.y > blockDim.y - filter_size / 2 - 1) {
            // левые нижние клетки-призраки
            size_t pixel_idx = (input_center_row_no + filter_size / 2) * input_width + (input_center_column_no - filter_size / 2);
            r[(threadIdx.x) * pixels_width + threadIdx.y + (filter_size / 2) * 2] = d_input[3 * pixel_idx + 0];
            g[(threadIdx.x) * pixels_width + threadIdx.y + (filter_size / 2) * 2] = d_input[3 * pixel_idx + 1];
            b[(threadIdx.x) * pixels_width + threadIdx.y + (filter_size / 2) * 2] = d_input[3 * pixel_idx + 2];
        }
        if (threadIdx.x < filter_size / 2 && threadIdx.y < filter_size / 2) {
            // левые верхние клетки-призраки
            size_t pixel_idx = (input_center_row_no - filter_size / 2) * input_width + (input_center_column_no - filter_size / 2);
            r[(threadIdx.x) * pixels_width + threadIdx.y] = d_input[3 * pixel_idx + 0];
            g[(threadIdx.x) * pixels_width + threadIdx.y] = d_input[3 * pixel_idx + 1];
            b[(threadIdx.x) * pixels_width + threadIdx.y] = d_input[3 * pixel_idx + 2];
        }
        if (threadIdx.x < filter_size / 2) {  // левые клетки-призраки
            size_t pixel_idx = input_center_row_no * input_width + (input_center_column_no - filter_size / 2);
            r[(threadIdx.x) * pixels_width + threadIdx.y + filter_size / 2] = d_input[3 * pixel_idx + 0];
            g[(threadIdx.x) * pixels_width + threadIdx.y + filter_size / 2] = d_input[3 * pixel_idx + 1];
            b[(threadIdx.x) * pixels_width + threadIdx.y + filter_size / 2] = d_input[3 * pixel_idx + 2];
        }
        if (threadIdx.x > blockDim.x - filter_size / 2 - 1) {  // правые клетки-призраки
            size_t pixel_idx = input_center_row_no * input_width + (input_center_column_no + filter_size / 2);
            r[(threadIdx.x + (filter_size / 2) * 2) * pixels_width + threadIdx.y + filter_size / 2] = d_input[3 * pixel_idx + 0];
            g[(threadIdx.x + (filter_size / 2) * 2) * pixels_width + threadIdx.y + filter_size / 2] = d_input[3 * pixel_idx + 1];
            b[(threadIdx.x + (filter_size / 2) * 2) * pixels_width + threadIdx.y + filter_size / 2] = d_input[3 * pixel_idx + 2];
        }
        if (threadIdx.y < filter_size / 2) {  // верхние клетки-призраки
            size_t pixel_idx = (input_center_row_no - filter_size / 2) * input_width + input_center_column_no;
            r[(threadIdx.x + filter_size / 2) * pixels_width + threadIdx.y] = d_input[3 * pixel_idx + 0];
            g[(threadIdx.x + filter_size / 2) * pixels_width + threadIdx.y] = d_input[3 * pixel_idx + 1];
            b[(threadIdx.x + filter_size / 2) * pixels_width + threadIdx.y] = d_input[3 * pixel_idx + 2];
        }
        if (threadIdx.y > blockDim.y - filter_size / 2 - 1) {  // нижние клетки-призраки
            size_t pixel_idx = (input_center_row_no + filter_size / 2) * input_width + input_center_column_no;
            r[(threadIdx.x + filter_size / 2) * pixels_width + threadIdx.y + (filter_size / 2) * 2] = d_input[3 * pixel_idx + 0];
            g[(threadIdx.x + filter_size / 2) * pixels_width + threadIdx.y + (filter_size / 2) * 2] = d_input[3 * pixel_idx + 1];
            b[(threadIdx.x + filter_size / 2) * pixels_width + threadIdx.y + (filter_size / 2) * 2] = d_input[3 * pixel_idx + 2];
        }

        // клетки не призраки
        size_t pixel_idx = input_center_row_no * input_width + input_center_column_no;
        r[(threadIdx.x + filter_size / 2) * pixels_width + threadIdx.y + filter_size / 2] = d_input[3 * pixel_idx + 0];
        g[(threadIdx.x + filter_size / 2) * pixels_width + threadIdx.y + filter_size / 2] = d_input[3 * pixel_idx + 1];
        b[(threadIdx.x + filter_size / 2) * pixels_width + threadIdx.y + filter_size / 2] = d_input[3 * pixel_idx + 2];

        // Дожидаемся, когда всё будет скопировано в shared память.
        __syncthreads();

        // Создаём в каждой нити массивы, которые будем сортировать для поиска медианы.
        uint16_t* window_r = new uint16_t[filter_size * filter_size];
        uint16_t* window_g = new uint16_t[filter_size * filter_size];
        uint16_t* window_b = new uint16_t[filter_size * filter_size];

        size_t window_idx = 0;

        // Заполняем массивы.
        for (int i = 0; i < filter_size; i ++)
        {
            for (int j = 0; j < filter_size; j++)
            {
                window_r[window_idx] = r[(threadIdx.x + i) * pixels_width + threadIdx.y + j];
                window_g[window_idx] = g[(threadIdx.x + i) * pixels_width + threadIdx.y + j];
                window_b[window_idx] = b[(threadIdx.x + i) * pixels_width + threadIdx.y + j];
                window_idx += 1;
            }
        }

        // Сортируем массивы.
        sort(window_r, filter_size * filter_size);
        sort(window_g, filter_size * filter_size);
        sort(window_b, filter_size * filter_size);

        // Получаем значения медиан.
        uint8_t r_median = static_cast<uint8_t>((window_r[filter_size * filter_size / 2] + window_r[(filter_size * filter_size + 1) / 2]) / 2);
        uint8_t g_median = static_cast<uint8_t>((window_g[filter_size * filter_size / 2] + window_g[(filter_size * filter_size + 1) / 2]) / 2);
        uint8_t b_median = static_cast<uint8_t>((window_b[filter_size * filter_size / 2] + window_b[(filter_size * filter_size + 1) / 2]) / 2);

        // Записываем результат.
        size_t output_center_idx = output_center_row_no * output_width + output_center_column_no;
        d_output[3 * output_center_idx + 0] = r_median;
        d_output[3 * output_center_idx + 1] = g_median;
        d_output[3 * output_center_idx + 2] = b_median;

        // Чистим память.
        delete[] window_r;
        delete[] window_g;
        delete[] window_b;
    }
}

int main(int argc, char *argv[])
{
    // Введение в CUDA: https://developer.nvidia.com/blog/easy-introduction-cuda-c-and-c/

    assert(argc == 4);  // SimpleFilter input.bmp output.bmp filter_size

    // Задаём размер фильтра.
    const size_t filter_size = std::stoul(argv[3]);

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

    // Запускаем фильтрацию.
    // Про индексацию:
    // https://medium.com/@erangadulshan.14/1d-2d-and-3d-thread-allocation-for-loops-in-cuda-e0f908537a52
    // https://www.researchgate.net/post/The_optimal_number_of_threads_per_block_in_CUDA_programming
    dim3 dimBlock(16, 16);
    dim3 dimGrid((output_image.bmp_info_header.width + 15) / 16, (output_image.bmp_info_header.height + 15) / 16);
    // Про динамическую общую память: https://stackoverflow.com/questions/24942073/dynamic-shared-memory-in-cuda
    const size_t shared_memory_size = (dimBlock.x + (filter_size / 2) * 2) * (dimBlock.x + (filter_size / 2) * 2) * 3;
    filter<<<dimGrid, dimBlock, shared_memory_size>>>(
            d_input,
            input_image.bmp_info_header.width,
            input_image.bmp_info_header.height,
            d_output,
            output_image.bmp_info_header.width,
            output_image.bmp_info_header.height,
            filter_size);

    // Забираем результат с видеокарты.
    cudaMemcpy(const_cast<uint8_t*>(output_image.data.data()), d_output, output_image.data.size(), cudaMemcpyDeviceToHost);

    // Сохраняем.
    output_image.write(argv[2]);

    // Очищаем память.
    cudaFree(d_input);
    cudaFree(d_output);
}