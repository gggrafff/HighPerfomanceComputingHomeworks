#include <chrono>
#include <iomanip>

#include "Matrix.h"
#include "LinearSystem.h"
#include "JacobiSolver.h"


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
    if (argc < 2) {
        std::cout << "Specify the size of system.";
        return -1;
    }

    omp_set_dynamic(0);  // отключаем динамический выбор количества потоков

    linear_systems_library::LinearSystem system(std::stoul(argv[1]));

    std::cout << "Work in 1 thread without omp." << std::endl;
    omp_set_num_threads(1);
    openblas_set_num_threads(1);

    auto begin = std::chrono::steady_clock::now();
    auto x = linear_systems_library::JacobiSolver::solve(system);
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "x is calculated in " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;

    std::cout << "Work in " << omp_get_num_procs() << " thread with omp and openblas threading." << std::endl;
    omp_set_num_threads(omp_get_num_procs());
    openblas_set_num_threads(omp_get_num_procs());

    begin = std::chrono::steady_clock::now();
    auto x_omp = linear_systems_library::JacobiSolver::solve_omp(system);
    end = std::chrono::steady_clock::now();
    elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "x is calculated in " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
}
