#include <cassert>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <functional>
#include <omp.h>

#include "Matrix.h"
#include "MatrixMultiplier.h"

/**
 * @brief Подготовка матрицы для метода простых итераций.
 * @param graph Граф связей страниц. Невзвешенный, направленный. Между объектами не более одной связи.
 * @return Матрица для поиска SimRank методом простых итераций.
 */
matrix_library::Matrix prepare_graph_for_iterations(matrix_library::Matrix graph) {
    graph.set_main_diagonal(0.0f);
    graph.normalize_columns();

    return graph;
}

/**
 * @brief Поиск SimRank.
 * @details Итерационный метод. https://en.wikipedia.org/wiki/SimRank
 * @details На самом деле можно ограничить строго количество итераций, но я произвожу итерации, пока метод не сойдётся гарантированно.
 * @param prepared_graph Матрица для метода простых итераций.
 * @return Матрица, содержащая SimRank объектов.
 */
matrix_library::Matrix simrank_iterations(const matrix_library::Matrix& prepared_graph, float damping=0.8f) {
    matrix_library::Matrix sr_current(prepared_graph.get_row_count(), prepared_graph.get_row_count());
    sr_current.set_main_diagonal(1.0f);
    matrix_library::Matrix sr_prev(prepared_graph.get_row_count(), prepared_graph.get_row_count());

    const float precision = 1e-5;

    auto prepared_graph_transposed = prepared_graph.transpose();
    /**
     * Для краткости обозначений функцию умножения обзовём mult.
     */
    std::function<matrix_library::Matrix(const matrix_library::Matrix &lhs, const matrix_library::Matrix &rhs)> mult(matrix_library::MatrixMultiplier::multiplication_cblas);

    while((sr_current - sr_prev).norm_inf() > precision) {
        sr_prev = sr_current;
        sr_current = mult(mult(prepared_graph_transposed, sr_current), prepared_graph)*damping;
        sr_current.set_main_diagonal(1.0f);
    }

    return sr_current;
}

/**
 * @brief Конвертирует количество микросекунд в человекопонятный формат.
 * @param duration_us Длительность в микросекундах.
 * @return Строка с человекопонятной записью.
 */
std::string convert_us_to_human_readable(uint64_t duration_us){
    if (duration_us < 1e3) {
        return std::to_string(duration_us) + " us";
    }

    std::stringstream ss;
    ss << std::fixed;
    ss << std::setprecision(2);
    if (duration_us < 1e6) {
        ss << static_cast<double>(duration_us) / 1e3 << " ms";
        return ss.str();
    }
    if (duration_us < 1e9) {
        ss << static_cast<double>(duration_us) / 1e6 << " s";
        return ss.str();
    }
    if (duration_us < 1e9 * 60) {
        ss << static_cast<double>(duration_us) / 1e6 / 60 << " min";
        return ss.str();
    }

    return std::to_string(duration_us) + " us";
}

int main(int argc, char *argv[]) {
    size_t nodes_count = 8;
    float edge_probability = 0.3f;
    bool need_print = false;

    for(int i = 1; i < argc; ++i) {
        std::string parameter_name(argv[i]);
        if (parameter_name == "-h") {
            std::cout << R"str(
Генерирует случайный граф связей между объектами и рассчитывает SimRank итерационным методом.
Использование:
SimRank [-n <количество_узлов>] [-p <вероятность_ребра>] [-v]
Параметр -n - это количество узлов в графе.
Параметр -p - это вероятность того, что между двумя узлами появляется ребро.
Параметр -v - если нужно напечатать матрицы на экран.
Все параметры необязательные.
Параметры по умолчанию:
SimRank -n 8 -p 0.3
)str" << std::endl;
            return 0;
        }
        if (parameter_name == "-v"){
            need_print = true;
            continue;
        }
        if (parameter_name == "-n") {
            nodes_count = std::stoul(argv[++i]);
            continue;
        }
        if (parameter_name == "-p") {
            edge_probability = std::stof(argv[++i]);
            continue;
        }
    }

    std::cout << "Устанавливаем количество потоков для omp и openblas." << std::endl;
    std::cout << "Количество используемых ядер: " << omp_get_num_procs() << std::endl << std::endl;
    openblas_set_num_threads(openblas_get_num_procs());
    omp_set_dynamic(0);
    omp_set_num_threads(omp_get_num_procs());

    matrix_library::Matrix graph(nodes_count, nodes_count);
    graph.initialize_random_directed_unweighted_graph(edge_probability);
    if (need_print) {
        std::cout << "Исходный граф ссылок: " << std::endl;
        graph.print();
    }

    std::cout << "Численное решение (метод простых итераций)." << std::endl;

    auto begin = std::chrono::steady_clock::now();
    auto prepared_graph_iterations = prepare_graph_for_iterations(graph);
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Подготовка матрицы для метода простых итераций заняла " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
    if (need_print) {
        std::cout << "Матрица для метода простых итераций: " << std::endl;
        prepared_graph_iterations.print();
    }
    begin = std::chrono::steady_clock::now();
    auto simrank = simrank_iterations(prepared_graph_iterations);
    end = std::chrono::steady_clock::now();
    elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Решение методом простых итераций заняло " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
    if (need_print) {
        std::cout << "Результат метода простых итераций: " << std::endl;
        simrank.print();
    }
    return 0;
}