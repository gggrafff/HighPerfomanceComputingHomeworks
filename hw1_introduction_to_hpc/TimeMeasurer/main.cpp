#include <iostream>
#include <chrono>

#include "Matrix.h"
#include "MatrixMultiplier.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Specify the size of the matrices.";
        return -1;
    }
    const size_t matrix_size = std::stoul(argv[1]);

    size_t experiments_count = 1;
    if (argc >= 3) {
        experiments_count = std::stoul(argv[2]);
    }

    matrix_library::Matrix a(matrix_size, matrix_size);
    a.initialize_randomly();
    matrix_library::Matrix b(matrix_size, matrix_size);
    b.initialize_randomly();

    std::vector<int64_t> durations_by_definition;
    for (int i = 0; i < experiments_count; ++i) {
        auto begin = std::chrono::steady_clock::now();
        auto c = matrix_library::MatrixMultiplier::multiplication_by_definition(a, b);
        auto end = std::chrono::steady_clock::now();
        auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        durations_by_definition.push_back(elapsed_us.count());
    }
    auto average_duration_by_definition = std::accumulate(durations_by_definition.begin(), durations_by_definition.end(), 0.0) / durations_by_definition.size();

    std::vector<int64_t> durations_blas;
    for (int i = 0; i < experiments_count; ++i) {
        auto begin = std::chrono::steady_clock::now();
        auto c = matrix_library::MatrixMultiplier::multiplication_cblas(a, b);
        auto end = std::chrono::steady_clock::now();
        auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        durations_blas.push_back(elapsed_us.count());
    }
    auto average_duration_blas = std::accumulate(durations_blas.begin(), durations_blas.end(), 0.0) / durations_blas.size();

    std::vector<int64_t> durations_strassen;
    for (int i = 0; i < experiments_count; ++i) {
        auto begin = std::chrono::steady_clock::now();
        auto c = matrix_library::MatrixMultiplier::multiplication_strassen(a, b);
        auto end = std::chrono::steady_clock::now();
        auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        durations_strassen.push_back(elapsed_us.count());
    }
    auto average_duration_strassen = std::accumulate(durations_strassen.begin(), durations_strassen.end(), 0.0) / durations_strassen.size();

    std::cout << experiments_count << " launches were carried out.\n";
    std::cout << "The multiplication by definition average duration of two " << matrix_size << "x" << matrix_size << " square matrices is " << static_cast<uint64_t>(average_duration_by_definition) << " us.\n";
    std::cout << "The multiplication with BLAS average duration of two " << matrix_size << "x" << matrix_size << " square matrices is " << static_cast<uint64_t>(average_duration_blas) << " us.\n";
    std::cout << "The multiplication with Strassen's algorithm average duration of two " << matrix_size << "x" << matrix_size << " square matrices is " << static_cast<uint64_t>(average_duration_strassen) << " us.\n";

    return 0;
}