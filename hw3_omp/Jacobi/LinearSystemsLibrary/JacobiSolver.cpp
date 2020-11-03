#include "JacobiSolver.h"

namespace linear_systems_library {

    matrix_library::Matrix JacobiSolver::solve(const linear_systems_library::LinearSystem &system, float eps) {
        /**
         * Об алгоритме: https://ru.wikipedia.org/wiki/Метод_Якоби
         * Метод Якоби — разновидность метода простой итерации для решения системы линейных алгебраических уравнений.
         */
        assert(system.get_A().get_row_count() == system.get_A().get_column_count());
        auto matrix_size = system.get_A().get_row_count();

        matrix_library::Matrix D_inv(matrix_size, matrix_size);
        for (size_t i = 0; i < matrix_size; ++i) {
            D_inv.get_element(i, i) = 1.0f / system.get_A().get_element(i, i);
        }

        matrix_library::Matrix g = matrix_library::MatrixMultiplier::multiplication_cblas(D_inv, system.get_b());

        matrix_library::Matrix E(matrix_size, matrix_size);
        for (size_t i = 0; i < matrix_size; ++i) {
            E.get_element(i, i) = 1.0f;
        }

        matrix_library::Matrix B = E - matrix_library::MatrixMultiplier::multiplication_cblas(D_inv, system.get_A());

        float q = matrix_norm_inf(B);
        assert(q < 1.0f);

        matrix_library::Matrix x_prev(matrix_size, 1);
        matrix_library::Matrix x_current(matrix_size, 1);
        do {
            x_prev = x_current;
            x_current = matrix_library::MatrixMultiplier::multiplication_cblas(B, x_prev) + g;
        } while (matrix_norm_inf(x_current - x_prev) > (1 - q) / q * eps);

        return x_current;
    }

    matrix_library::Matrix JacobiSolver::solve_omp(const linear_systems_library::LinearSystem &system, float eps) {
        /**
         * Об алгоритме: https://ru.wikipedia.org/wiki/Метод_Якоби
         * Метод Якоби — разновидность метода простой итерации для решения системы линейных алгебраических уравнений.
         * Сам алгоритм итерационный и не поддаётся распараллеливанию, 
         * но можно распараллелить операции с матрицами: заполнение, сложение, умножение.
         */
        assert(system.get_A().get_row_count() == system.get_A().get_column_count());
        auto matrix_size = system.get_A().get_row_count();

        matrix_library::Matrix D_inv(matrix_size, matrix_size);
#pragma omp parallel for default(none) shared(D_inv, system, matrix_size)
        for (size_t i = 0; i < matrix_size; ++i) {
            D_inv.get_element(i, i) = 1.0f / system.get_A().get_element(i, i);
        }

        matrix_library::Matrix g = matrix_library::MatrixMultiplier::multiplication_cblas(D_inv, system.get_b());

        matrix_library::Matrix E(matrix_size, matrix_size);
#pragma omp parallel for default(none) shared(E, matrix_size)
        for (size_t i = 0; i < matrix_size; ++i) {
            E.get_element(i, i) = 1.0f;
        }

        matrix_library::Matrix B = matrix_subtraction_omp(E, matrix_library::MatrixMultiplier::multiplication_cblas(D_inv, system.get_A()));

        float q = matrix_norm_inf_omp(B);
        assert(q < 1.0f);

        matrix_library::Matrix x_prev(matrix_size, 1);
        matrix_library::Matrix x_current(matrix_size, 1);
        do {
            x_prev = x_current;
            x_current = matrix_addition_omp(matrix_library::MatrixMultiplier::multiplication_cblas(B, x_prev), g);
        } while (matrix_norm_inf_omp(matrix_subtraction_omp(x_current, x_prev)) > (1 - q) / q * eps);

        return x_current;
    }

    float JacobiSolver::matrix_norm_inf(const matrix_library::Matrix &matrix) {
        float norm = 0.0f;
        for (size_t i = 0; i < matrix.get_row_count(); ++i) {
            float sum = 0.0f;
            for (size_t j = 0; j < matrix.get_column_count(); ++j) {
                sum += std::abs(matrix.get_element(i, j));
            }
            if (sum > norm) {
                norm = sum;
            }
        }
        return norm;
    }

    float JacobiSolver::matrix_norm_inf_omp(const matrix_library::Matrix &matrix) {
        float norm = 0.0f;
        /**
         * Один поток работает с одной строкой матрицы.
         */
#pragma omp parallel for default(none) shared(norm, matrix)
        for (size_t i = 0; i < matrix.get_row_count(); ++i) {
            float sum = 0.0f;
            for (size_t j = 0; j < matrix.get_column_count(); ++j) {
                sum += std::abs(matrix.get_element(i, j));
            }
#pragma omp critical
            {
                if (sum > norm) {
                    norm = sum;
                }
            }
        }
        return norm;
    }

    matrix_library::Matrix
    JacobiSolver::matrix_subtraction_omp(const matrix_library::Matrix &lhs, const matrix_library::Matrix &rhs) {
        assert(lhs.get_row_count() == rhs.get_row_count());
        assert(lhs.get_column_count() == rhs.get_column_count());

        matrix_library::Matrix result(lhs.get_row_count(), lhs.get_column_count());
#pragma omp parallel for collapse(2) default(none) shared(lhs, rhs, result)
        for (size_t i = 0; i < result.get_row_count(); ++i) {
            for (size_t j = 0; j < result.get_column_count(); ++j) {
                result.get_element(i, j) = lhs.get_element(i, j) - rhs.get_element(i, j);
            }
        }

        return result;
    }

    matrix_library::Matrix
    JacobiSolver::matrix_addition_omp(const matrix_library::Matrix &lhs, const matrix_library::Matrix &rhs) {
        assert(lhs.get_row_count() == rhs.get_row_count());
        assert(lhs.get_column_count() == rhs.get_column_count());

        matrix_library::Matrix result(lhs.get_row_count(), lhs.get_column_count());
#pragma omp parallel for collapse(2) default(none) shared(lhs, rhs, result)
        for (size_t i = 0; i < result.get_row_count(); ++i) {
            for (size_t j = 0; j < result.get_column_count(); ++j) {
                result.get_element(i, j) = lhs.get_element(i, j) + rhs.get_element(i, j);
            }
        }

        return result;
    }
}