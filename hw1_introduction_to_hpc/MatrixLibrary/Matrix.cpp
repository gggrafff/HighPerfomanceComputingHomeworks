#include "Matrix.h"

namespace matrix_library {

    Matrix::Matrix(const Matrix& original, size_t row_count, size_t column_count, size_t offset_x, size_t offset_y) :
            data_(original.data_),
            row_count_(row_count),
            column_count_(column_count),
            is_pointer_(true),
            offset_x_(original.offset_x_ + offset_x),
            offset_y_(original.offset_y_ + offset_y),
            original_row_count_(original.original_row_count_),
            original_column_count_(original.original_column_count_){
        assert(offset_x + column_count_ <= original_column_count_);
        assert(offset_y + row_count_ <= original_row_count_);
    }

    Matrix::Matrix(size_t row_count, size_t column_count) :
            data_(new float[row_count * column_count]),
            row_count_(row_count),
            column_count_(column_count),
            is_pointer_(false),
            offset_x_(0),
            offset_y_(0),
            original_row_count_(row_count),
            original_column_count_(column_count){
        initialize_zeros();
    }

    Matrix::~Matrix() {
        if (!is_pointer_){
            delete[] data_;
        }
    }

    Matrix::Matrix(const Matrix &other) {
        is_pointer_ = false;
        row_count_ = other.row_count_;
        column_count_ = other.column_count_;
        original_row_count_ = row_count_;
        original_column_count_ = column_count_;
        data_ = new float[row_count_ * column_count_];

        for (size_t i = 0; i < row_count_; ++i) {
            for (size_t j = 0; j < column_count_; ++j) {
                get_element(i, j) = other.get_element(i, j);
            }
        }
    }

    Matrix::Matrix(Matrix&& other) noexcept {
        is_pointer_ = false;
        row_count_ = other.row_count_;
        column_count_ = other.column_count_;
        original_row_count_ = row_count_;
        original_column_count_ = column_count_;

        if (!other.is_pointer_) {
            data_ = other.data_;
            other.data_ = nullptr;
        } else {
            data_ = new float[row_count_ * column_count_];

            for (size_t i = 0; i < row_count_; ++i) {
                for (size_t j = 0; j < column_count_; ++j) {
                    get_element(i, j) = other.get_element(i, j);
                }
            }
        }
    }

    Matrix &Matrix::operator=(const Matrix &rhs) {
        if (this == &rhs){
            return *this;
        }
        if (!is_pointer_) {
            delete[] data_;
            row_count_ = rhs.row_count_;
            column_count_ = rhs.column_count_;
            original_row_count_ = row_count_;
            original_column_count_ = column_count_;
            data_ = new float[row_count_ * column_count_];
            for (size_t i = 0; i < row_count_; ++i) {
                for (size_t j = 0; j < column_count_; ++j) {
                    get_element(i, j) = rhs.get_element(i, j);
                }
            }
        } else {
            assert(row_count_ == rhs.row_count_);
            assert(column_count_ == rhs.column_count_);

            for (size_t i = 0; i < row_count_; ++i) {
                for (size_t j = 0; j < column_count_; ++j) {
                    get_element(i, j) = rhs.get_element(i, j);
                }
            }
        }
        return *this;
    }

    Matrix &Matrix::operator=(Matrix&& rhs) noexcept {
        if (this == &rhs){
            return *this;
        }
        if (!is_pointer_ && !rhs.is_pointer_) {
            delete[] data_;
            row_count_ = rhs.row_count_;
            column_count_ = rhs.column_count_;
            original_row_count_ = row_count_;
            original_column_count_ = column_count_;
            data_ = rhs.data_;
            rhs.data_ = nullptr;
        } else {
            assert(row_count_ == rhs.row_count_);
            assert(column_count_ == rhs.column_count_);

            for (size_t i = 0; i < row_count_; ++i) {
                for (size_t j = 0; j < column_count_; ++j) {
                    get_element(i, j) = rhs.get_element(i, j);
                }
            }
        }
        return *this;
    }

    Matrix Matrix::operator+(const Matrix &rhs) const {
        assert(row_count_ == rhs.row_count_);
        assert(column_count_ == rhs.column_count_);

        Matrix result(row_count_, column_count_);
        for (size_t i = 0; i < row_count_; ++i) {
            for (size_t j = 0; j < column_count_; ++j) {
                result.get_element(i, j) = get_element(i, j) + rhs.get_element(i, j);
            }
        }

        return result;
    }

    Matrix Matrix::operator-(const Matrix &rhs) const {
        assert(row_count_ == rhs.row_count_);
        assert(column_count_ == rhs.column_count_);

        Matrix result(row_count_, column_count_);
        for (size_t i = 0; i < row_count_; ++i) {
            for (size_t j = 0; j < column_count_; ++j) {
                result.get_element(i, j) = get_element(i, j) - rhs.get_element(i, j);
            }
        }

        return result;
    }

    Matrix &Matrix::operator+=(const Matrix &rhs) {
        assert(row_count_ == rhs.row_count_);
        assert(column_count_ == rhs.column_count_);

        for (size_t i = 0; i < row_count_; ++i) {
            for (size_t j = 0; j < column_count_; ++j) {
                get_element(i, j) += rhs.get_element(i, j);
            }
        }

        return *this;
    }

    Matrix &Matrix::operator-=(const Matrix &rhs) {
        assert(row_count_ == rhs.row_count_);
        assert(column_count_ == rhs.column_count_);

        for (size_t i = 0; i < row_count_; ++i) {
            for (size_t j = 0; j < column_count_; ++j) {
                get_element(i, j) -= rhs.get_element(i, j);
            }
        }

        return *this;
    }

    void Matrix::initialize_randomly() {
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);

        for (size_t i = 0; i < row_count_; ++i) {
            for (size_t j = 0; j < column_count_; ++j) {
                get_element(i, j) = dis(gen);
            }
        }
    }

    void Matrix::initialize_zeros() {
        for (int i = 0; i < row_count_; ++i) {
            for (int j = 0; j < column_count_; ++j) {
                get_element(i, j) = 0.0f;
            }
        }
    }

    void Matrix::resize(size_t new_row_count, size_t new_column_count) {
        assert(!is_pointer_);

        if ((new_row_count == row_count_) && (new_column_count == column_count_)) {
            return;
        }

        float* new_data = new float[new_row_count * new_column_count];
        for (size_t i = 0; i < new_row_count; ++i) {
            for (size_t j = 0; j < new_column_count; ++j) {
                if ((i < row_count_) && (j < column_count_)) {
                    new_data[i * new_column_count + j] = get_element(i, j);
                } else {
                    new_data[i * new_column_count + j] = 0.0f;
                }
            }
        }

        delete[] data_;
        data_ = new_data;

        row_count_ = new_row_count;
        column_count_ = new_column_count;
        original_row_count_ = row_count_;
        original_column_count_ = column_count_;
    }

    float* Matrix::get_data() const {
        return data_;
    }

    size_t Matrix::get_row_count() const {
        return row_count_;
    }

    size_t Matrix::get_column_count() const {
        return column_count_;
    }

    void Matrix::print(std::ostream &stream) const {
        stream << std::endl;
        for (size_t i = 0; i < row_count_; ++i) {
            for (size_t j = 0; j < column_count_; ++j) {
                stream << get_element(i, j) << " ";
            }
            stream << std::endl;
        }
        stream << std::endl;
    }
}