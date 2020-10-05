#ifndef HIGHPERFOMANCECOMPUTINGHOMEWORKS_JACOBISOLVER_H
#define HIGHPERFOMANCECOMPUTINGHOMEWORKS_JACOBISOLVER_H

#include <omp.h>

#include "LinearSystem.h"
#include "Matrix.h"
#include "MatrixMultiplier.h"

namespace linear_systems_library {
    class JacobiSolver {
    public:
        /**
         * @brief Решить СЛАУ.
         * @param system СЛАУ.
         * @param eps Требуемая точность.
         * @return Вектор-столбец с решением.
         */
        static matrix_library::Matrix solve(const linear_systems_library::LinearSystem &system, float eps = 1e-5);

        /**
         * /**
         * @brief Решить СЛАУ. Используется OMP.
         * @param system СЛАУ.
         * @param eps Требуемая точность.
         * @return Вектор-столбец с решением.
         */
        static matrix_library::Matrix solve_omp(const linear_systems_library::LinearSystem &system, float eps = 1e-5);

    private:
        /**
         * @brief Найти норму матрицы.
         * @param matrix Матрица
         * @return Норма матрицы.
         */
        static float matrix_norm_inf(const matrix_library::Matrix &matrix);

        /**
         * @brief Найти норму матрицы. Используется OMP.
         * @param matrix Матрица
         * @return Норма матрицы.
         */
        static float matrix_norm_inf_omp(const matrix_library::Matrix &matrix);

        /**
         * @brief Найти разницу матриц. Используется OMP.
         * @param lhs Уменьшаемое.
         * @param rhs Вычитаемое.
         * @return Разность матриц.
         */
        static matrix_library::Matrix matrix_subtraction_omp(const matrix_library::Matrix& lhs, const matrix_library::Matrix& rhs);

        /**
         * @brief Найти сумму матриц. Используется OMP.
         * @param lhs Первое слагаемое.
         * @param rhs Второе слагаемое.
         * @return Сумма матриц.
         */
        static matrix_library::Matrix matrix_addition_omp(const matrix_library::Matrix& lhs, const matrix_library::Matrix& rhs);
    };
}

#endif //HIGHPERFOMANCECOMPUTINGHOMEWORKS_JACOBISOLVER_H
