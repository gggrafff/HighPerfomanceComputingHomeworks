#ifndef HIGHPERFOMANCECOMPUTINGHOMEWORKS_MATRIXMULTIPLICATION_H
#define HIGHPERFOMANCECOMPUTINGHOMEWORKS_MATRIXMULTIPLICATION_H

#include <cassert>

#include <cblas.h>  // заголовочный файл C-интерфейса библиотеки BLAS

#include "Matrix.h"

namespace matrix_library {

    class MatrixMultiplier{
    public:
        static void multiplication_by_definition(const Matrix& lhs, const Matrix& rhs, Matrix& result);

        static Matrix multiplication_by_definition(const Matrix& lhs, const Matrix& rhs);

        static Matrix multiplication_cblas(const Matrix& lhs, const Matrix& rhs);

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

        static void multiplication_strassen_implementation(
                const Matrix& lhs, const Matrix& rhs, Matrix& result);
    };


}


#endif //HIGHPERFOMANCECOMPUTINGHOMEWORKS_MATRIXMULTIPLICATION_H
