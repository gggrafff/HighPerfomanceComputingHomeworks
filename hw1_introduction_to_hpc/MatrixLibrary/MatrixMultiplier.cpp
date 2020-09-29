#include "MatrixMultiplier.h"

namespace matrix_library {

    void MatrixMultiplier::multiplication_by_definition(const Matrix &lhs, const Matrix &rhs, Matrix &result) {
        assert(lhs.get_column_count() == rhs.get_row_count());
        assert(lhs.get_row_count() == result.get_row_count());
        assert(rhs.get_column_count() == result.get_column_count());

        for (int i = 0 ; i < lhs.get_row_count(); i++)
        {
            for (int j = 0;  j < rhs.get_column_count(); j++)
            {
                result[i * result.get_column_count() + j] = 0.0f;
                for (int k = 0; k < lhs.get_column_count(); k++)
                {
                    result[i * result.get_column_count() + j] += lhs[i * lhs.get_column_count() + k] * rhs[k * rhs.get_column_count() + j];
                }
            }
        }
    }

    Matrix MatrixMultiplier::multiplication_by_definition(const Matrix &lhs, const Matrix &rhs) {
        assert(lhs.get_column_count() == rhs.get_row_count());

        Matrix result(lhs.get_row_count(), rhs.get_column_count());
        multiplication_by_definition(lhs, rhs, result);
        return result;
    }

    Matrix MatrixMultiplier::multiplication_cblas(const Matrix &lhs, const Matrix &rhs) {
        assert(lhs.get_column_count() == rhs.get_row_count());

        Matrix addend_and_result(lhs.get_row_count(), rhs.get_column_count());

        cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,
                    lhs.get_row_count(),rhs.get_column_count(),lhs.get_column_count(),
                    1.0,
                    lhs.get_data(),lhs.get_column_count(),
                    rhs.get_data(),rhs.get_column_count(),
                    0.0,
                    addend_and_result.get_data(),rhs.get_column_count());

        return addend_and_result;
    }

    Matrix MatrixMultiplier::multiplication_strassen(Matrix &lhs, Matrix &rhs) {
        assert(lhs.get_row_count() == lhs.get_column_count());
        assert(rhs.get_row_count() == rhs.get_column_count());
        assert(lhs.get_column_count() == rhs.get_row_count());

        size_t matrix_size = lhs.get_row_count();
        size_t increased_matrix_size = static_cast<size_t>(std::lround(std::pow(2.0, static_cast<double>(round_log2(matrix_size)))));

        lhs.resize(increased_matrix_size, increased_matrix_size);
        rhs.resize(increased_matrix_size, increased_matrix_size);

        Matrix result(lhs.get_row_count(), rhs.get_column_count());

        multiplication_strassen_implementation(lhs, rhs, result);

        lhs.resize(matrix_size, matrix_size);
        rhs.resize(matrix_size, matrix_size);
        result.resize(matrix_size, matrix_size);

        return result;
    }

    size_t MatrixMultiplier::round_log2(size_t number) {
        double x = static_cast<double>(number - 1);  // если убрать "- 1", то "не меньше" в описании нужно заменить на "больше"
        auto answer = reinterpret_cast<unsigned int *>(&x);
        ++answer;
        return static_cast<size_t>(((*answer & 0x7FF00000) >> 20) - 1022);
    }

    void MatrixMultiplier::multiplication_strassen_implementation(const Matrix &lhs, const Matrix &rhs, Matrix &result) {
        static const size_t MATRIX_SIZE_EFFICIENT_BOUND = 64;

        size_t matrix_size = lhs.get_row_count();
        if (matrix_size <= MATRIX_SIZE_EFFICIENT_BOUND) {
            multiplication_by_definition(lhs, rhs, result);
        } else {
            Matrix lhs11(lhs.get_data(), matrix_size / 2, matrix_size / 2);
            Matrix lhs12(lhs.get_data() + matrix_size / 2, matrix_size / 2, matrix_size / 2);
            Matrix lhs21(lhs.get_data() + matrix_size / 2 * matrix_size, matrix_size / 2, matrix_size / 2);
            Matrix lhs22(lhs.get_data() + matrix_size / 2 * matrix_size + matrix_size / 2, matrix_size / 2, matrix_size / 2);

            Matrix rhs11(rhs.get_data(), matrix_size / 2, matrix_size / 2);
            Matrix rhs12(rhs.get_data() + matrix_size / 2, matrix_size / 2, matrix_size / 2);
            Matrix rhs21(rhs.get_data() + matrix_size / 2 * matrix_size, matrix_size / 2, matrix_size / 2);
            Matrix rhs22(rhs.get_data() + matrix_size / 2 * matrix_size + matrix_size / 2, matrix_size / 2, matrix_size / 2);

            Matrix result11(result.get_data(), matrix_size / 2, matrix_size / 2);
            Matrix result12(result.get_data() + matrix_size / 2, matrix_size / 2, matrix_size / 2);
            Matrix result21(result.get_data() + matrix_size / 2 * matrix_size, matrix_size / 2, matrix_size / 2);
            Matrix result22(result.get_data() + matrix_size / 2 * matrix_size + matrix_size / 2, matrix_size / 2, matrix_size / 2);

            auto s1 = lhs21 + lhs22;
            auto s2 = s1 - lhs11;
            auto s3 = lhs11 - lhs21;
            auto s4 = lhs12 - s2;

            auto s5 = rhs12 - rhs11;
            auto s6 = rhs22 - s5;
            auto s7 = rhs22 - rhs12;
            auto s8 = s6 - rhs21;

            Matrix p1(s2.get_row_count(), s6.get_column_count());
            multiplication_strassen_implementation(s2, s6, p1);
            Matrix p2(lhs11.get_row_count(), rhs11.get_column_count());
            multiplication_strassen_implementation(lhs11, rhs11, p2);
            Matrix p3(lhs12.get_row_count(), rhs21.get_column_count());
            multiplication_strassen_implementation(lhs12, rhs21, p3);
            Matrix p4(s3.get_row_count(), s7.get_column_count());
            multiplication_strassen_implementation(s3, s7, p4);
            Matrix p5(s1.get_row_count(), s5.get_column_count());
            multiplication_strassen_implementation(s1, s5, p5);
            Matrix p6(s4.get_row_count(), rhs22.get_column_count());
            multiplication_strassen_implementation(s4, rhs22, p6);
            Matrix p7(lhs22.get_row_count(), s8.get_column_count());
            multiplication_strassen_implementation(lhs22, s8, p7);

            auto t1 = p1 + p2;
            auto t2 = t1 + p4;

            result11 = p2 + p3;
            result12 = t1 + p5 + p6;
            result21 = t2 - p7;
            result22 = t2 + p6;
        }
    }
}