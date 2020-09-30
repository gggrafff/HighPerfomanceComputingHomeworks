#ifndef HIGHPERFOMANCECOMPUTINGHOMEWORKS_MATRIXMULTIPLICATION_H
#define HIGHPERFOMANCECOMPUTINGHOMEWORKS_MATRIXMULTIPLICATION_H

#include <cassert>

#include <cblas.h>  // заголовочный файл C-интерфейса библиотеки BLAS

#include "Matrix.h"

namespace matrix_library {

    class MatrixMultiplier{
    public:
        /**
         * @brief Умножение матриц по определению произведения матриц.
         * @param lhs Первый множитель.
         * @param rhs Второй множитель.
         * @param result Произведение матриц.
         */
        static void multiplication_by_definition(const Matrix& lhs, const Matrix& rhs, Matrix& result);

        /**
         * @brief Умножение матриц по определению произведения матриц.
         * @param lhs Первый множитель.
         * @param rhs Второй множитель.
         * @return Произведение матриц.
         */
        static Matrix multiplication_by_definition(const Matrix& lhs, const Matrix& rhs);

        /**
         * @brief Умножение матриц с использованием библиотеки cBLAS.
         * @param lhs Первый множитель.
         * @param rhs Второй множитель.
         * @return Произведение матриц.
         */
        static Matrix multiplication_cblas(const Matrix& lhs, const Matrix& rhs);

        /**
         * @brief Умножение матриц с использованием алгоритма Винограда-Штрассена.
         * @details Только для квадратных матриц.
         * @details Обёртка алгоритма.
         * @param lhs Первый множитель.
         * @param rhs Второй множитель.
         * @return Произведение матриц.
         */
        static Matrix multiplication_strassen(Matrix& lhs, Matrix& rhs);
    private:
        /**
         * @brief Округлённый в большую сторону до целого числа логарифм по основанию 2.
         * @details Используется знание о представлении чисел с плавающей точкой в памяти.
         * @details Подробнее: https://ru.wikipedia.org/wiki/Число_двойной_точности
         * @param number Число, от которого нужно взять логарифм по основанию 2.
         * @return Значение логарифма, округлённое вверх до целого.
         */
        static size_t round_log2(size_t number);

        /**
         * @brief Умножение матриц с использованием алгоритма Винограда-Штрассена.
         * @details Только для квадратных матриц.
         * @details Рекурсивная часть алгоритма.
         * @param lhs Первый множитель.
         * @param rhs Второй множитель.
         * @param result Произведение матриц.
         */
        static void multiplication_strassen_implementation(
                const Matrix& lhs, const Matrix& rhs, Matrix& result);
    };


}


#endif //HIGHPERFOMANCECOMPUTINGHOMEWORKS_MATRIXMULTIPLICATION_H
