#include <cassert>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <omp.h>

/**
 * @brief Это кусок из тестов Eigen. Почему то к нему есть обращения в нетестовом коде eigen.
 */
namespace Eigen {
    static bool no_more_assert = false;
    struct eigen_assert_exception
    {
        eigen_assert_exception(void) {}
        ~eigen_assert_exception() { Eigen::no_more_assert = false; }
    };
}

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "gmm/gmm.h"

#include "Matrix.h"
#include "MatrixMultiplier.h"

/**
 * @brief Подготовка матрицы для метода простых итераций.
 * @param graph Граф связей страниц. Невзвешенный, направленный. Между страницами не более одной связи.
 * @return
 */
matrix_library::Matrix prepare_graph_for_iterations(matrix_library::Matrix graph) {
    // При расчете PageRank предполагается, что страницы без исходящих ссылок ссылаются на все остальные страницы коллекции.
    for(size_t i = 0;i < graph.get_column_count(); ++i){
        if (graph.column_is_zero(i)) {
            /**
             * Если случайный сёрфер перешёл на страницу, которая никуда не ссылается,
             * то он продолжает со случайной страницы, а PR этой страницы в равной степени передаётся всем другим.
             * Решает проблему 0 исходящих ссылок.
             */
            graph.add_to_column(i, 1.0f);
        }
    }
    graph.set_main_diagonal(0.0f);  // Ссылки на самого себя не учитываем.
    graph.normalize_columns();  // Считаем вероятности, с которой случайный сёрфер перейдёт по конкретной ссылке.

    return graph;
}

/**
 * @brief Поиск PR без демпфирования. Возможно вырожденное решение, потому что допустим нулевой PR.
 * @details Итерационный метод.
 * @param prepared_graph Матрица для метода простых итераций.
 * @return Вектор-столбец, содержащий PR страниц.
 */
matrix_library::Matrix naive_pagerank_iterations(const matrix_library::Matrix& prepared_graph) {
    /**
     * Задаём начальное приближение: одинаковые pr всех страниц.
     * Такое начальное приближение с большой вероятностью не должно приводить к вырожденному решению.
     */
    matrix_library::Matrix pr_current(prepared_graph.get_row_count(), 1);
    pr_current.add_to_column(0, 1.0f / static_cast<float>(prepared_graph.get_row_count()));
    matrix_library::Matrix pr_prev(prepared_graph.get_row_count(), 1);

    const float precision = 1e-5;

    while((pr_current - pr_prev).norm_inf() > precision) {
        pr_prev = pr_current;
        pr_current = matrix_library::MatrixMultiplier::multiplication_cblas(prepared_graph, pr_current);
    }

    return pr_current;
}

/**
 * @brief Поиск PR с демпфированием. Не допускает вырожденного решения.
 * @details Итерационный метод.
 * @param prepared_graph Матрица для метода простых итераций.
 * @param damping_factor Коэффициент демпфирования. Вероятность, с которой случайный сёрфер продолжит нажимать на ссылки.
 * @return Вектор-столбец, содержащий PR страниц.
 */
matrix_library::Matrix damping_pagerank_iterations(matrix_library::Matrix prepared_graph, float damping_factor=0.85) {
    /**
     * Задаём начальное приближение: одинаковые pr всех страниц.
     * Такое начальное приближение с большой вероятностью не должно приводить к вырожденному решению.
     */
    matrix_library::Matrix pr_current(prepared_graph.get_row_count(), 1);
    pr_current.add_to_column(0, 1.0f / static_cast<float>(prepared_graph.get_row_count()));
    matrix_library::Matrix pr_prev(prepared_graph.get_row_count(), 1);

    const float precision = 1e-5f;

    prepared_graph *= damping_factor;  // учёт демпфирования

    while((pr_current - pr_prev).norm_inf() > precision) {
        pr_prev = pr_current;
        pr_current = matrix_library::MatrixMultiplier::multiplication_cblas(prepared_graph, pr_current) + (1.0f - damping_factor) / static_cast<float>(prepared_graph.get_row_count());
    }

    return pr_current;
}

/**
 * @brief Конвертация matrix_library::Matrix в Eigen::MatrixXf.
 * @param matrix Матрица, которую нужно преобразовать.
 * @return Результат преобразования. Матрица для Eigen.
 */
Eigen::MatrixXf convert_my_matrix_to_eigen_matrix(const matrix_library::Matrix &matrix){
    Eigen::MatrixXf res(matrix.get_row_count(), matrix.get_column_count());
    for (size_t i = 0; i < matrix.get_row_count(); ++i) {
        for (size_t j = 0; j < matrix.get_column_count(); ++j) {
            res(i, j) = matrix.get_element(i, j);
        }
    }
    return res;
}

/**
 * @brief Конвертация matrix_library::Matrix в gmm::dense_matrix<float>.
 * @param matrix Матрица, которую нужно преобразовать.
 * @return Результат преобразования. Матрица для gmm++.
 */
gmm::dense_matrix<float> convert_my_matrix_to_gmm_matrix(const matrix_library::Matrix &matrix){
    gmm::dense_matrix<float> res(matrix.get_row_count(), matrix.get_column_count());
    gmm::clear(res);
    for (size_t i = 0; i < matrix.get_row_count(); ++i) {
        for (size_t j = 0; j < matrix.get_column_count(); ++j) {
            res(i, j) = matrix.get_element(i, j);
        }
    }
    return res;
}

/**
 * @brief Подготовка матрицы для строгого решения СЛАУ для поиска PR без демпфирования.
 * @param graph Граф связей страниц. Невзвешенный, направленный. Между страницами не более одной связи.
 * @return Матрица коэффициентов СЛАУ для поиска PR без демпфирования.
 */
Eigen::MatrixXf prepare_graph_for_eigen_naive(matrix_library::Matrix graph) {
    // При расчете PageRank предполагается, что страницы без исходящих ссылок ссылаются на все остальные страницы коллекции.
    for(size_t i = 0;i < graph.get_column_count(); ++i){
        if (graph.column_is_zero(i)) {
            /**
             * Если случайный сёрфер перешёл на страницу, которая никуда не ссылается,
             * то он продолжает со случайной страницы, а PR этой страницы в равной степени передаётся всем другим.
             * Решает проблему 0 исходящих ссылок.
             */
            graph.add_to_column(i, 1.0f);
        }
    }
    graph.set_main_diagonal(0.0f);  // Ссылки на самого себя не учитываем.
    graph.normalize_columns();  // Считаем вероятности, с которой случайный сёрфер перейдёт по конкретной ссылке.
    graph.set_main_diagonal(-1.0f);

    auto graph_eigen = convert_my_matrix_to_eigen_matrix(graph);

    return graph_eigen;
}

/**
 * @brief Подготовка матрицы для строгого решения СЛАУ для поиска PR c демпфированием.
 * @param graph Граф связей страниц. Невзвешенный, направленный. Между страницами не более одной связи.
 * @param damping_factor Коэффициент демпфирования. Вероятность, с которой случайный сёрфер продолжит нажимать на ссылки.
 * @return Матрица коэффициентов СЛАУ для поиска PR c демпфированием.
 */
matrix_library::Matrix prepare_graph_for_pr_damping(matrix_library::Matrix graph, float damping_factor=0.85) {
    // При расчете PageRank предполагается, что страницы без исходящих ссылок ссылаются на все остальные страницы коллекции.
    for(size_t i = 0;i < graph.get_column_count(); ++i){
        if (graph.column_is_zero(i)) {
            /**
             * Если случайный сёрфер перешёл на страницу, которая никуда не ссылается,
             * то он продолжает со случайной страницы, а PR этой страницы в равной степени передаётся всем другим.
             * Решает проблему 0 исходящих ссылок.
             */
            graph.add_to_column(i, 1.0f);
        }
    }
    graph.set_main_diagonal(0.0f);  // Ссылки на самого себя не учитываем.
    graph.normalize_columns();  // Считаем вероятности, с которой случайный сёрфер перейдёт по конкретной ссылке.
    graph *= damping_factor;  // учёт демпфирования
    graph.set_main_diagonal(-1.0f);

    return graph;
}

/**
 * @brief Подготовка матрицы для строгого решения СЛАУ с помощью Eigen для поиска PR c демпфированием.
 * @param graph Граф связей страниц. Невзвешенный, направленный. Между страницами не более одной связи.
 * @param damping_factor Коэффициент демпфирования. Вероятность, с которой случайный сёрфер продолжит нажимать на ссылки.
 * @return Матрица коэффициентов СЛАУ для поиска PR c демпфированием. Матрица для Eigen.
 */
Eigen::MatrixXf prepare_graph_for_eigen_damping(const matrix_library::Matrix& graph, float damping_factor=0.85) {
    auto graph_eigen = convert_my_matrix_to_eigen_matrix(prepare_graph_for_pr_damping(graph, damping_factor));
    return graph_eigen;
}

/**
 * @brief Подготовка матрицы для строгого решения СЛАУ с помощью gmm++ для поиска PR c демпфированием.
 * @param graph Граф связей страниц. Невзвешенный, направленный. Между страницами не более одной связи.
 * @param damping_factor Коэффициент демпфирования. Вероятность, с которой случайный сёрфер продолжит нажимать на ссылки.
 * @return Матрица коэффициентов СЛАУ для поиска PR c демпфированием. Матрица для gmm++.
 */
gmm::dense_matrix<float> prepare_graph_for_gmm_damping(const matrix_library::Matrix& graph, float damping_factor=0.85) {
    auto graph_gmm = convert_my_matrix_to_gmm_matrix(prepare_graph_for_pr_damping(graph, damping_factor));
    return graph_gmm;
}

/**
 * @brief Поиск строгого решения СЛАУ для PR без демпфирования. Допускает вырожденное решение.
 * @param prepared_graph Матрица коэффициентов СЛАУ.
 * @return Вектор-столбец, содержащий PR страниц.
 */
Eigen::VectorXf naive_pagerank_eigen(const Eigen::MatrixXf& prepared_graph) {
    Eigen::VectorXf rigth(prepared_graph.rows());
    for(size_t i = 0;i < prepared_graph.rows(); ++i){
        rigth(i) = 0.0f;
    }

    Eigen::VectorXf pr = prepared_graph.householderQr().solve(rigth);

    return pr;
}

/**
 * @brief Поиск строгого решения СЛАУ для PR с демпфированием. Не допускает вырожденное решение.
 * @details Решение с помощью Eigen.
 * @param prepared_graph Матрица коэффициентов СЛАУ.
 * @param damping_factor Коэффициент демпфирования. Вероятность, с которой случайный сёрфер продолжит нажимать на ссылки.
 * @return Вектор-столбец, содержащий PR страниц.
 */
Eigen::VectorXf damping_pagerank_eigen(const Eigen::MatrixXf& prepared_graph, float damping_factor=0.85) {
    Eigen::VectorXf rigth(prepared_graph.rows());
    for(size_t i = 0;i < prepared_graph.rows(); ++i){
        rigth(i) = -(1.0f - damping_factor) / static_cast<float>(prepared_graph.rows());
    }

    Eigen::VectorXf pr = prepared_graph.householderQr().solve(rigth);

    /*double relative_error = (prepared_graph * pr - rigth).norm() / rigth.norm(); // norm() is L2 norm
    std::cout << "The relative error is:\n" << relative_error << std::endl;*/

    return pr;
}

/**
 * @brief Поиск строгого решения СЛАУ для PR с демпфированием. Не допускает вырожденное решение.
 * @details Решение с помощью gmm++.
 * @param prepared_graph Матрица коэффициентов СЛАУ.
 * @param damping_factor Коэффициент демпфирования. Вероятность, с которой случайный сёрфер продолжит нажимать на ссылки.
 * @return Вектор-столбец, содержащий PR страниц.
 */
std::vector<float> damping_pagerank_gmm(const gmm::dense_matrix<float>& prepared_graph, float damping_factor=0.85) {
    std::vector<float> rigth(prepared_graph.nrows());
    for(size_t i = 0;i < prepared_graph.nrows(); ++i){
        rigth[i] = -(1.0f - damping_factor) / static_cast<float>(prepared_graph.nrows());
    }

    std::vector<float> pr(prepared_graph.nrows());
    gmm::lu_solve(prepared_graph, pr, rigth);

    return pr;
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
Генерирует случайный граф связей между страницами и рассчитывает PageRank разными способами.
Использование:
PageRank [-n <количество_узлов>] [-p <вероятность_ребра>] [-v]
Параметр -n - это количество узлов в графе.
Параметр -p - это вероятность того, что между двумя узлами появляется ребро.
Параметр -v - если нужно напечатать матрицы на экран.
Все параметры необязательные.
Параметры по умолчанию:
PageRank -n 8 -p 0.3
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
    Eigen::setNbThreads(0);

    std::cout << "Везде далее коэффициент демпфирования равен 0.85." << std::endl;

    matrix_library::Matrix graph(nodes_count, nodes_count);
    graph.initialize_random_directed_unweighted_graph(edge_probability);
    if (need_print) {
        std::cout << "Исходный граф ссылок: " << std::endl;
        graph.print();
    }

    std::cout << "Численное решение СЛАУ(метод простых итераций)." << std::endl;

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
    auto naive_pr_iterations = naive_pagerank_iterations(prepared_graph_iterations);
    end = std::chrono::steady_clock::now();
    elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Решение методом простых итераций для упрощённого pr заняло " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
    if (need_print) {
        std::cout << "Результат метода простых итераций для упрощённого pr: " << std::endl;
        naive_pr_iterations.print();
    }
    begin = std::chrono::steady_clock::now();
    auto damping_pr_iterations = damping_pagerank_iterations(prepared_graph_iterations);
    end = std::chrono::steady_clock::now();
    elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Решение методом простых итераций для демпингованного pr заняло " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
    if (need_print) {
        std::cout << "Результат метода простых итераций для демпингованного pr: " << std::endl;
        damping_pr_iterations.print();
    }

    std::cout << "Строгое решение СЛАУ." << std::endl;
    begin = std::chrono::steady_clock::now();
    auto prepared_graph_eigen = prepare_graph_for_eigen_naive(graph);
    end = std::chrono::steady_clock::now();
    elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Подготовка матрицы для строгого решения СЛАУ упрощённого pr заняла " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
    if (need_print) {
        std::cout << "Матрица коэффициентов линейной системы для упрощённого PR: " << std::endl;
        std::cout << prepared_graph_eigen << std::endl << std::endl;
    }
    begin = std::chrono::steady_clock::now();
    auto naive_pr_eigen = naive_pagerank_eigen(prepared_graph_eigen);
    end = std::chrono::steady_clock::now();
    elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Решение СЛАУ для упрощённого pr с помощью eigen заняло " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
    if (need_print) {
        std::cout << "Результат решения СЛАУ eigen для упрощённого pr: " << std::endl;
        std::cout << naive_pr_eigen << std::endl << std::endl;
        std::cout << "Видим, что решение вырожденное, это недостаток упрощённого метода рассчёта pr. \n"
                     "В демпингованном pr для обхода этой проблемы даже страницы, на которые никто не ссылается, имеют ненулевой pr. " << std::endl;
    }

    begin = std::chrono::steady_clock::now();
    auto prepared_graph_eigen_damping = prepare_graph_for_eigen_damping(graph);
    end = std::chrono::steady_clock::now();
    elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Подготовка матрицы для строгого решения СЛАУ демпингованного pr заняла " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
    if (need_print) {
        std::cout << "Матрица коэффициентов линейной системы для демпингованного PR: " << std::endl;
        std::cout << prepared_graph_eigen_damping << std::endl << std::endl;
    }
    begin = std::chrono::steady_clock::now();
    auto damping_pr_eigen = damping_pagerank_eigen(prepared_graph_eigen_damping);
    end = std::chrono::steady_clock::now();
    elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Решение СЛАУ для демпингованного pr с помощью eigen заняло " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
    if (need_print) {
        std::cout << "Результат решения СЛАУ eigen для демпингованного pr: " << std::endl;
        std::cout << damping_pr_eigen << std::endl << std::endl;
    }
    auto prepared_graph_gmm = prepare_graph_for_gmm_damping(graph);
    begin = std::chrono::steady_clock::now();
    auto damping_pr_gmm = damping_pagerank_gmm(prepared_graph_gmm);
    end = std::chrono::steady_clock::now();
    elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Решение СЛАУ для демпингованного pr с помощью gmm++ заняло " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
    if (need_print) {
        std::cout << "Результат решения СЛАУ gmm для демпингованного pr: " << std::endl;
        for(auto num: damping_pr_gmm) {
            std::cout << num << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << "Все результаты получены при работе с плотными матрицами. \n"
                 "Как выяснили в задаче BinPower, работа с разреженными матрицами становится выгодной только на размерах матриц, которые проблематично умножать на домашнем ПК.\n"
                 "Поэтому результаты для разреженных матриц не приводим." << std::endl;
}