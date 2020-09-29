#ifndef HIGHPERFOMANCECOMPUTINGHOMEWORKS_MATRIX_H
#define HIGHPERFOMANCECOMPUTINGHOMEWORKS_MATRIX_H

#include <cassert>
#include <random>
#include <iostream>

namespace matrix_library {
    class Matrix {
    public:
        Matrix(float* data, size_t row_count, size_t column_count);

        Matrix(size_t row_count, size_t column_count);

        ~Matrix();

        Matrix(const Matrix& other);

        Matrix(Matrix && other);

        Matrix& operator=(const Matrix& rhs);

        Matrix& operator=(Matrix&& rhs);

        float& operator[](size_t element_index);

        const float& operator[](size_t element_index) const;

        Matrix operator+(const Matrix &rhs) const;

        Matrix operator-(const Matrix &rhs) const;

        Matrix& operator+=(const Matrix& rhs);

        Matrix& operator-=(const Matrix& rhs);

        void initialize_randomly();

        void resize(size_t new_row_count, size_t new_column_count);

        float* get_data() const;

        size_t get_row_count() const;

        size_t get_column_count() const;

        /**
         * @brief Напечатать матрицу в поток вывода.
         * @param stream Поток вывода.
         */
        void print(std::ostream &stream = std::cout) const;

    private:
        float *data_{nullptr};
        size_t row_count_{0};
        size_t column_count_{0};

        bool is_pointer_{false};
    };
}

#endif //HIGHPERFOMANCECOMPUTINGHOMEWORKS_MATRIX_H
