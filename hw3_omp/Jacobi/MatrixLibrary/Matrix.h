#ifndef HIGHPERFOMANCECOMPUTINGHOMEWORKS_MATRIX_H
#define HIGHPERFOMANCECOMPUTINGHOMEWORKS_MATRIX_H

#include <cassert>
#include <random>
#include <iostream>

namespace matrix_library {
    /**
     * @brief Класс, объединяющий работу с матрицами.
     * @details Создан для того, чтобы оградить пользователя от работы с сырыми указателями,
     * @details объединить методы для работы с матрицами в одном пространстве имён.
     */
    class Matrix {
    public:
        Matrix() = default;
        /**
         * @brief Конструктор для того чтобы сослаться на подматрицу существующей матрицы.
         * @param original Исходная матрица, в которой выделяем подматрицу.
         * @param row_count Количество строк подматрицы.
         * @param column_count Колицество столбцов подматрицы.
         * @param offset_x Смещение по горизонтали(в стандартной записи) подматрицы в исходной матрице.
         * @param offset_y Смещение по вертикали(в стандартной записи) подматрицы в исходной матрице.
         */
        Matrix(const Matrix& original, size_t row_count, size_t column_count, size_t offset_x = 0, size_t offset_y = 0);

        /**
         * @brief Конструктор, выделяющий память под матрицу и заполняющий матрицу нулями.
         * @param row_count Количество строк матрицы.
         * @param column_count Колицество столбцов матрицы.
         */
        Matrix(size_t row_count, size_t column_count);

        /**
         * @brief Деструктор. Освобождает память, если она была выделена в конструкторе.
         */
        ~Matrix();

        /**
         * @brief Конструктор копирования.
         * @details Выделяет память под новую матрицу и копирует элементы.
         * @param other Матрица, которую копируем.
         */
        Matrix(const Matrix& other);

        /**
         * @brief Конструктор перемещения.
         * @details Если другая матрица владеет памятью с элементами, то это владение будет передано.
         * @details Если другая матрица указывает на чужую память, то под новую матрицу будет выделена новая память.
         * @param other Матрица, копию которой создаём.
         */
        Matrix(Matrix&& other) noexcept;

        /**
         * @brief Оператор присваивания без передачи владения данными.
         * @param rhs Присваевоемое значение.
         * @return Ссылка на текущую матрицу.
         */
        Matrix& operator=(const Matrix& rhs);

        /**
         * @brief Оператор присваивания c передачей владения данными, если второй операнд ими владеет.
         * @param rhs Присваевоемое значение.
         * @return Ссылка на текущую матрицу.
         */
        Matrix& operator=(Matrix&& rhs) noexcept;

        /**
         * @brief Выбор элемента для записи значения.
         * @param row_index Индекс строки элемента в матрице.
         * @param column_index Индекс столбца элемента в матрице.
         * @return Ссылка на выбранный элемент.
         */
        inline float& get_element(size_t row_index, size_t column_index) {
            return data_[(offset_y_ + row_index) * original_column_count_ + offset_x_ + column_index];
        }

        /**
         * @brief Выбор элемента для чтения значения.
         * @param row_index Индекс строки элемента в матрице.
         * @param column_index Индекс столбца элемента в матрице.
         * @return Ссылка на выбранный элемент.
         */
        inline const float& get_element(size_t row_index, size_t column_index) const {
            return data_[(offset_y_ + row_index) * original_column_count_ + offset_x_ + column_index];
        }

        /**
         * @brief Оператор поэлементного суммирования матриц.
         * @param rhs Второе слагаемое.
         * @return Сумма матриц.
         */
        Matrix operator+(const Matrix &rhs) const;

        /**
         * @brief Оператор поэлементного вычитания матриц.
         * @param rhs Вычитаемое.
         * @return Разница матриц.
         */
        Matrix operator-(const Matrix &rhs) const;

        /**
         * @brief Оператор поэлементного прибавления другой матрицы к данной матрице.
         * @param rhs Второе слагаемое.
         * @return Ссылка на текущую матрицу, содержащую сумму матриц.
         */
        Matrix& operator+=(const Matrix& rhs);

        /**
         * @brief Оператор поэлементного вычитания другой матрицы из данной матрицы.
         * @param rhs Вычитаемое.
         * @return Ссылка на текущую матрицу, содержащую разницу матриц.
         */
        Matrix& operator-=(const Matrix& rhs);

        /**
         * @brief Заполняет матрицу случайными значениями.
         */
        void initialize_randomly();

        /**
         * @brief Заполняет матрицу нулями.
         */
        void initialize_zeros();

        /**
         * @brief Изменяет размер матрицы, дополняя нулями или обрезая края.
         * @details Только для матриц, которые владеют памятью.
         * @param new_row_count Новое количество строк.
         * @param new_column_count Новое количество столбцов.
         */
        void resize(size_t new_row_count, size_t new_column_count);

        /**
         * @brief Получить указатель на память, хранящую элементы матрицы.
         * @return Указатель на память с элементами матрицы.
         */
        float* get_data() const;

        /**
         * @brief Получить количество строк матрицы.
         * @return Количество строк матрицы.
         */
        size_t get_row_count() const;

        /**
         * @brief Получить количество столбцов матрицы.
         * @return Количество столбцов матрицы.
         */
        size_t get_column_count() const;

        /**
         * @brief Напечатать матрицу в поток вывода.
         * @param stream Поток вывода.
         */
        void print(std::ostream &stream = std::cout) const;

    private:
        /**
         * @brief Указатель на память, в которой хранятся элементы матрицы.
         * @details Память может быть выделена как в этом классе, так и вне его. Для разных целей разные конструкторы.
         */
        float *data_{nullptr};

        /**
         * @brief Количество строк матрицы.
         */
        size_t row_count_{0};
        /**
         * @brief Количество столбцов матрицы.
         */
        size_t column_count_{0};

        /**
         * @brief False, если матрица владеет памятью с элементами, true - если она на эту память только указывает.
         * @details Данный класс можно использовать двумя способами:
         * @details     Мы можем выделить память в конструкторе, освободить в деструкторе.
         * @details     Мы можем сослаться на уже хранящуюся в памяти подматрицу существующей матрицы.
         */
        bool is_pointer_{false};

        /**
         * @brief Смещение в строке(по столбцам / по горизонтали при стандартной записи матрицы), по которому лежат данные относительно указателя data_.
         * @details Если данная матрица является подматрицей другой матрицы, то нужно указать смещение в памяти, по которому расположена подматрица.
         */
        size_t offset_x_{0};
        /**
         * @brief Смещение в столбце(по строкам / по вертикали при стандартной записи матрицы), по которому лежат данные относительно указателя data_.
         * @details Если данная матрица является подматрицей другой матрицы, то нужно указать смещение в памяти, по которому расположена подматрица.
         */
        size_t offset_y_{0};

        /**
         * @brief Количество строк в оригинальной матрице по адресу data_.
         * @details Если данная матрица является подматрицей другой матрицы, то нужно знать исходное количество строк для верной адресации элементов.
         */
        size_t original_row_count_{0};

        /**
         * @brief Количество столбцов в оригинальной матрице по адресу data_.
         * @details Если данная матрица является подматрицей другой матрицы, то нужно знать исходное количество столбцов для верной адресации элементов.
         */
        size_t original_column_count_{0};
    };
}

#endif //HIGHPERFOMANCECOMPUTINGHOMEWORKS_MATRIX_H
