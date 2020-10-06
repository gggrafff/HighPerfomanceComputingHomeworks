#include <random>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <tuple>
#include <vector>
#include <cassert>

#include <omp.h>

/**
 * @brief Генерирует зашумлённые наблюдения линейного закона. y = a * x + b + noise
 * @details Переменная x инкрементится, начиная с нуля.
 * @tparam DistributionType Тип данных, описывающий распределение шума. Ожидается какой-либо из стандартных типов.
 * @param a Коэффициент прямой при первой степени x.
 * @param b Свободный коэффициент прямой.
 * @param noise_distribution Распределение шума.
 * @param points_count Количество наблюдений.
 * @return Кортеж векторов (x, y). Зашумлённые наблюдения.
 */
template <typename DistributionType>
std::tuple<std::vector<double>, std::vector<double>> generate_linear_dependence_with_noise(double a, double b, DistributionType noise_distribution, size_t points_count=100) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<double> x(points_count);
    std::iota (std::begin(x), std::end(x), 0);

    std::vector<double> y;
    y.reserve(points_count);
    for(auto sample: x) {
        y.push_back(a * sample + b + noise_distribution(gen));
    }

    return {x, y};
}

/**
 * @brief Метод наименьших квадратов. Поиск коэффициентов прямой по зашумлённым наблюдениям.
 * @param x Вектор точек, в которых производились наблюдения.
 * @param y Наблюдаемые значения функции с шумом.
 * @return Кортеж (a, b) оценок коэффициентов прямой y = a * x + b
 */
std::tuple<double, double> find_linear_coefficients(const std::vector<double>& x, const std::vector<double>& y) {
    assert(x.size() == y.size());

    double sum_x = 0.0;
    double sum_y = 0.0;
    double sum_x_squares = 0.0;
    double sum_prod_x_y = 0.0;

#pragma omp parallel default(none) shared(x, y, sum_x, sum_y, sum_x_squares, sum_prod_x_y)
    {
#pragma omp for reduction(+:sum_x) nowait
        for (size_t i = 0; i < x.size(); ++i) {
            sum_x += x[i];
        }
#pragma omp for reduction(+:sum_y) nowait
        for (size_t i = 0; i < y.size(); ++i) {
            sum_y += y[i];
        }
#pragma omp for reduction(+:sum_x_squares) nowait
        for (size_t i = 0; i < x.size(); ++i) {
            sum_x_squares += x[i] * x[i];
        }
#pragma omp for reduction(+:sum_prod_x_y) nowait
        for (size_t i = 0; i < x.size(); ++i) {
            sum_prod_x_y += x[i] * y[i];
        }
#pragma omp barrier
    }

    double square_of_sum_x = sum_x * sum_x;

    double a_estimate = (static_cast<double>(x.size()) * sum_prod_x_y - sum_x * sum_y) / (static_cast<double>(x.size()) * sum_x_squares - square_of_sum_x);
    double b_estimate = (sum_y - a_estimate * sum_x) / static_cast<double>(x.size());

    return {a_estimate, b_estimate};
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
    if (argc < 6) {
        std::cout << "Specify all parameters: path_to_program a b mean_noise std_noise points_count.";
        return -1;
    }

    omp_set_dynamic(0);  // отключаем динамический выбор количества потоков
    std::cout << "Work in " << omp_get_num_procs() << " thread with omp." << std::endl;
    omp_set_num_threads(omp_get_num_procs());

    auto [x, y] = generate_linear_dependence_with_noise(std::stod(argv[1]), std::stod(argv[2]), std::normal_distribution(std::stod(argv[3]), std::stod(argv[4])), std::stoul(argv[5]));

    auto begin = std::chrono::steady_clock::now();
    auto [a_estimate, b_estimate] = find_linear_coefficients(x, y);
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Calculated in " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
    std::cout << "a = " << a_estimate << std::endl;
    std::cout << "b = " << b_estimate << std::endl;
}