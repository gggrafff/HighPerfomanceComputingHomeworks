#include "LinearSystem.h"

namespace linear_systems_library {

    LinearSystem::LinearSystem(size_t size, bool diagonally_dominant) : A_(size, size), x_(matrix_library::Matrix(size, 1)) {
        A_.initialize_randomly();
        if (diagonally_dominant) {
            for (size_t i = 0; i < A_.get_row_count(); ++i) {
                float sum = 0.0f;
                for (size_t j = 0; j < A_.get_column_count(); ++j) {
                    sum += A_.get_element(i, j);
                }
                A_.get_element(i, i) = sum * 2;
            }
        }
        x_->initialize_randomly();
        b_ = matrix_library::MatrixMultiplier::multiplication_cblas(A_, x_.value());
    }

    const matrix_library::Matrix &LinearSystem::get_A() const {
        return A_;
    }

    const matrix_library::Matrix &LinearSystem::get_b() const {
        return b_;
    }

    const matrix_library::Matrix &LinearSystem::get_x() const {
        assert(x_.has_value());
        return x_.value();
    }

    bool LinearSystem::has_solving() const {
        return x_.has_value();
    }
}