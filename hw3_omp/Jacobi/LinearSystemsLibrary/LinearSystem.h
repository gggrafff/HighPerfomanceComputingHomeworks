#ifndef HIGHPERFOMANCECOMPUTINGHOMEWORKS_LINEARSYSTEM_H
#define HIGHPERFOMANCECOMPUTINGHOMEWORKS_LINEARSYSTEM_H

#include <optional>

#include "Matrix.h"
#include "MatrixMultiplier.h"

/**
 * @brief Пространство имён, объединяющее работу с системами линейных уравнений.
 */
namespace linear_systems_library {

    /**
     * @brief Класс для хранения систем линейных уравнений.
     */
    class LinearSystem {
    public:
        /**
         * @brief Конструирует случайную СЛАУ, решение сразу известно.
         * @param size Размер системы.
         * @param diagonally_dominant True, если требуется обеспечить диагональное преобладание матрицы коэффициентов.
         */
        explicit LinearSystem(size_t size, bool diagonally_dominant=true);

        /**
         * @brief Получить матрицу коэффициентов.
         * @return Матрица коэффициентов. Неизменяемая ссылка.
         */
        const matrix_library::Matrix &get_A() const;

        /**
         * @brief Получить вектор-столбец свободных членов.
         * @return Свободные члены. Неизменяемая ссылка.
         */
        const matrix_library::Matrix &get_b() const;

        /**
         * @brief Получить решение системы, если ранее оно было сохранено в этом объекте.
         * @return Решение системы. Вектор-столбец. Неизменяемая ссылка.
         */
        const matrix_library::Matrix &get_x() const;

        /**
         * @brief Проверить, было ли в данном объекте записано решение системы ранее.
         * @return True, если решение сохранено ранее в этом объекте.
         */
        bool has_solving() const;

    private:
        matrix_library::Matrix A_;
        matrix_library::Matrix b_;
        std::optional<matrix_library::Matrix> x_;
    };
}

#endif //HIGHPERFOMANCECOMPUTINGHOMEWORKS_LINEARSYSTEM_H
