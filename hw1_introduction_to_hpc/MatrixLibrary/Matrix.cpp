#include "Matrix.h"

namespace matrix_library {

    Matrix::Matrix(float *data, size_t row_count, size_t column_count) :
            data_(data),
            row_count_(row_count),
            column_count_(column_count),
            is_pointer_(true) {}

    Matrix::Matrix(size_t row_count, size_t column_count) :
            data_(new float[row_count * column_count]),
            row_count_(row_count),
            column_count_(column_count),
            is_pointer_(false) {
        for (int i = 0; i < row_count_; ++i) {
            for (int j = 0; j < column_count_; ++j) {
                data_[i * column_count_ + j] = 0.0f;
            }
        }
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
        data_ = new float[row_count_ * column_count_];

        for (int i = 0; i < row_count_; ++i) {
            for (int j = 0; j < column_count_; ++j) {
                data_[i * column_count_ + j] = other.data_[i * column_count_ + j];
            }
        }
    }

    Matrix::Matrix(Matrix &&other) {
        if (!other.is_pointer_) {
            is_pointer_ = false;
            row_count_ = other.row_count_;
            column_count_ = other.column_count_;
            data_ = other.data_;
            other.data_ = nullptr;
        } else {
            is_pointer_ = false;
            row_count_ = other.row_count_;
            column_count_ = other.column_count_;
            data_ = new float[row_count_ * column_count_];

            for (int i = 0; i < row_count_; ++i) {
                for (int j = 0; j < column_count_; ++j) {
                    data_[i * column_count_ + j] = other.data_[i * column_count_ + j];
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
            data_ = new float[row_count_ * column_count_];
            for (int i = 0; i < row_count_; ++i) {
                for (int j = 0; j < column_count_; ++j) {
                    data_[i * column_count_ + j] = rhs.data_[i * column_count_ + j];
                }
            }
        } else {
            assert(row_count_ == rhs.row_count_);
            assert(column_count_ == rhs.column_count_);

            for (int i = 0; i < row_count_; ++i) {
                for (int j = 0; j < column_count_; ++j) {
                    data_[i * column_count_ + j] = rhs.data_[i * column_count_ + j];
                }
            }
        }
        return *this;
    }

    Matrix &Matrix::operator=(Matrix &&rhs) {
        if (this == &rhs){
            return *this;
        }
        if (!is_pointer_ && !rhs.is_pointer_) {
            delete[] data_;
            row_count_ = rhs.row_count_;
            column_count_ = rhs.column_count_;
            data_ = rhs.data_;
            rhs.data_ = nullptr;
        } else {
            assert(row_count_ == rhs.row_count_);
            assert(column_count_ == rhs.column_count_);

            for (int i = 0; i < row_count_; ++i) {
                for (int j = 0; j < column_count_; ++j) {
                    data_[i * column_count_ + j] = rhs.data_[i * column_count_ + j];
                }
            }
        }
        return *this;
    }

    float &Matrix::operator[](size_t element_index) {
        return data_[element_index];
    }

    const float &Matrix::operator[](size_t element_index) const {
        return data_[element_index];
    }

    Matrix Matrix::operator+(const Matrix &rhs) const {
        assert(row_count_ == rhs.row_count_);
        assert(column_count_ == rhs.column_count_);

        Matrix result(row_count_, column_count_);
        for (int i = 0; i < row_count_; ++i) {
            for (int j = 0; j < column_count_; ++j) {
                result[i * column_count_ + j] = data_[i * column_count_ + j] + rhs[i * column_count_ + j];
            }
        }

        return result;
    }

    Matrix Matrix::operator-(const Matrix &rhs) const {
        assert(row_count_ == rhs.row_count_);
        assert(column_count_ == rhs.column_count_);

        Matrix result(row_count_, column_count_);
        for (int i = 0; i < row_count_; ++i) {
            for (int j = 0; j < column_count_; ++j) {
                result[i * column_count_ + j] = data_[i * column_count_ + j] - rhs[i * column_count_ + j];
            }
        }

        return result;
    }

    Matrix &Matrix::operator+=(const Matrix &rhs) {
        assert(row_count_ == rhs.row_count_);
        assert(column_count_ == rhs.column_count_);

        for (int i = 0; i < row_count_; ++i) {
            for (int j = 0; j < column_count_; ++j) {
                data_[i * column_count_ + j] += rhs[i * column_count_ + j];
            }
        }

        return *this;
    }

    Matrix &Matrix::operator-=(const Matrix &rhs) {
        assert(row_count_ == rhs.row_count_);
        assert(column_count_ == rhs.column_count_);

        for (int i = 0; i < row_count_; ++i) {
            for (int j = 0; j < column_count_; ++j) {
                data_[i * column_count_ + j] -= rhs[i * column_count_ + j];
            }
        }

        return *this;
    }

    void Matrix::initialize_randomly() {
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<> dis(0.0, 1.0);

        for (int i = 0; i < row_count_; ++i) {
            for (int j = 0; j < column_count_; ++j) {
                data_[i * column_count_ + j] = dis(gen);
            }
        }
    }

    void Matrix::resize(size_t new_row_count, size_t new_column_count) {
        assert(!is_pointer_);

        if ((new_row_count == row_count_) && (new_column_count == column_count_)) {
            return;
        }

        float* new_data = new float[new_row_count * new_column_count];
        for (int i = 0; i < new_row_count; ++i) {
            for (int j = 0; j < new_column_count; ++j) {
                if ((i < row_count_) && (j < column_count_)) {
                    new_data[i * new_column_count + j] = data_[i * column_count_ + j];
                } else {
                    new_data[i * new_column_count + j] = 0.0f;
                }
            }
        }

        delete[] data_;
        data_ = new_data;

        row_count_ = new_row_count;
        column_count_ = new_column_count;
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
        for (int i = 0; i < row_count_; ++i) {
            for (int j = 0; j < column_count_; ++j) {
                stream << data_[i * column_count_ + j] << " ";
            }
            stream << std::endl;
        }
        stream << std::endl;
    }
}