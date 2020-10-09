#include <cassert>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <omp.h>

#include "gmm/gmm.h"

#include "Matrix.h"
#include "MatrixMultiplier.h"

matrix_library::Matrix binpow_cblas(matrix_library::Matrix matrix, uint64_t power) {
    assert(matrix.get_row_count() == matrix.get_column_count());

    matrix_library::Matrix res(matrix.get_row_count(), matrix.get_row_count());
    res.initialize_ones_diagonal();

    while (power) {
        if (power & 1) {
            res = matrix_library::MatrixMultiplier::multiplication_cblas(res, matrix);
        }
        matrix = matrix_library::MatrixMultiplier::multiplication_cblas(matrix, matrix);
        power >>= 1;
    }
    return res;
}

gmm::row_matrix<gmm::wsvector<float>> binpow_sparse_gmm(gmm::row_matrix<gmm::wsvector<float>> matrix, uint64_t power) {
    assert(matrix.nrows() == matrix.ncols());

    gmm::row_matrix<gmm::wsvector<float>> res(matrix.nrows(), matrix.nrows());
    for (size_t i = 0; i < matrix.nrows(); ++i) {
        res(i, i) = 1.0f;
    }
    gmm::clean(res, 1e-12);

    while (power) {
        if (power & 1) {
            gmm::mult(res, matrix, res);
            gmm::clean(res, 1e-12);
        }
        gmm::mult(matrix, matrix, matrix);
        gmm::clean(matrix, 1e-12);
        power >>= 1;
    }
    return res;
}

gmm::row_matrix<gmm::wsvector<float>> convert_my_matrix_to_gmm_csc_matrix(const matrix_library::Matrix& matrix){
    gmm::row_matrix<gmm::wsvector<float>> res(matrix.get_row_count(), matrix.get_column_count());
    for (size_t i = 0; i < matrix.get_row_count(); ++i) {
        for (size_t j = 0; j < matrix.get_column_count(); ++j) {
            res(i, j) = matrix.get_element(i, j);
        }
    }

    gmm::clean(res, 1e-12);

    return res;
}

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
    uint64_t steps_count = 4;
    float edge_probability = 0.2f;
    bool need_print = false;
    for(int i = 1; i < argc; ++i) {
        std::string parameter_name(argv[i]);
        if (parameter_name == "-h") {
            std::cout << R"str(
Возводит случайную матрицу смежности направленного неориентированного графа в указанную степень.
Может использоваться для поиска количества путей заданной длины в графе.
Использование:
BinPower [-n <количество_узлов>] [-s <длина_пути>] [-p <вероятность_ребра>] [-v]
Параметр -n - это количество узлов в графе.
Парамет -s - это количество шагов по графу, которое мы сделаем из каждой вершины.
Параметр -p - это вероятность того, что между двумя узлами появляется ребро.
Параметр -v - если нужно напечатать матрицы на экран.
Все параметры необязательные.
Параметры по умолчанию:
BinPower -n 8 -s 4 -p 0.2
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
        if (parameter_name == "-s") {
            steps_count = std::stoul(argv[++i]);
            continue;
        }
    }

    matrix_library::Matrix graph(nodes_count, nodes_count);
    graph.initialize_random_directed_unweighted_graph(edge_probability);
    if (need_print) {
        graph.print();
    }

    std::cout << "Умножаем с помощью openblas." << std::endl;

    std::cout << "Количество используемых ядер: " << openblas_get_num_procs() << std::endl;
    openblas_set_num_threads(openblas_get_num_procs());

    auto begin = std::chrono::steady_clock::now();
    auto after_several_steps = binpow_cblas(graph, steps_count);
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);

    if (need_print) {
        after_several_steps.print();
    }

    std::cout << "Возведение матрицы в степень заняло " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl << std::endl;

    std::cout << "Умножаем с помощью GMM++ (для разреженных матриц)." << std::endl;
    //std::cout << "Количество используемых ядер: " << omp_get_num_procs() << std::endl;
    //omp_set_num_threads(omp_get_num_procs());

    auto graph_eigen = convert_my_matrix_to_gmm_csc_matrix(graph);
    begin = std::chrono::steady_clock::now();
    auto after_several_steps_gmm = binpow_sparse_gmm(graph_eigen, steps_count);
    end = std::chrono::steady_clock::now();
    elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);

    if (need_print) {
        std::cout << after_several_steps_gmm << std::endl;
    }

    std::cout << "Возведение матрицы в степень заняло " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;

    return 0;
}