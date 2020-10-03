#include <random>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <omp.h>

/**
 * @brief Класс для промежуточной потоковой записи перед отправкой в shared поток.
 */
class ParallelStream{
public:
    ParallelStream() = default;
    template <class T>
    ParallelStream& operator<<(const T& inData){
        stream_ << inData;
        return *this;
    }
    std::string str() const{
        return stream_.str();
    }
private:
    std::stringstream stream_;
};

/**
 * @brief Вычисление числа Пи методом Монте-Карло.
 * @details Вычисление производится параллельно. Используется OMP.
 * @details Для выбора количества потоков нужно вызвать omp_set_num_threads или установить переменную окружения OMP_NUM_THREADS до вызова этой функции.
 * @param points_count Количество точек, которые генерируются для оценки числа Пи.
 * @param verbose True, если требуется вывод в консоль отладочных сообщений, иначе - false.
 * @return Оценённое значение числа Пи.
 */
float calculate_pi_with_monte_carlo(size_t points_count=100, bool verbose=false) {
    /**
     * Случайным образом кидаете точку в единичный квадрат.
     * В этот же квадрат вписан круг.
     * Если точка попала в круг, увеличиваете счетчик.
     * Затем находите отношение точек, попавших в круг к общему числу точек.
     * Зная площади квадрата и круга, находите приблизительно число pi.
     */

    size_t in_circle_points = 0;  // общее количество попавших в круг точек

#pragma omp parallel default(none) shared(points_count, in_circle_points, verbose, std::cout)
    {
#pragma omp single
        {  // вывод отладочной информации о количестве запущенных потоков.
            if (verbose) {
                std::cout << "Num of OMP threads: " << omp_get_num_threads() << std::endl;
            }
        }

        // Инициализируем генератор случайных чисел. Seed в каждом потоке должен быть разный.
        std::random_device rd;
        uint32_t seed = rd();
        seed = (seed & 0xFFFFFFF0) | (omp_get_thread_num() + 1);  // привязываем seed к номеру потока
        std::mt19937 gen(seed);
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

        /**
         * Чтобы реже делать блокировки при обращении к in_circle_points,
         * я в каждом потоке отдельно подсчитываю это значение в local_in_circle_points,
         * а потом только единожды суммирую с общим.
         */
        size_t local_in_circle_points = 0;
#pragma omp for schedule(static)
        for (size_t i = 0; i < points_count; ++i) {
            float x = dis(gen);
            float y = dis(gen);
            if (verbose){  // вывод отладочной информации о сгенерированной точке.
                std::cout << (ParallelStream() << "Thread №" << omp_get_thread_num() << " generated point (" << x << ";" << y <<"). \n").str();
            }
            if (x * x + y * y <= 1) {  // проверка, принадлежат ли точки кругу
                ++local_in_circle_points;
            }
        }
#pragma omp atomic
        in_circle_points += local_in_circle_points;
    }

    // рассчитаем pi
    const float total_square = 4.0f;
    float square_relation_monte_carlo = static_cast<float >(points_count) / static_cast<float >(in_circle_points);
    float pi = total_square / square_relation_monte_carlo;

    return pi;
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
    if (argc < 2) {
        std::cout << "Specify the point count.";
        return -1;
    }
    const size_t points_count = std::stoul(argv[1]);  // первым параметром передано количество точек

    bool verbose = false;
    if((argc > 2) && (std::string(argv[2]) == "verbose")) {  // опциональный второй параметр - нужны ли отладочные сообщения
        verbose = true;
    }

    if (verbose){  // вывод отладочной информации о количестве доступных CPU.
        std::cout << "Num of CPU: " << omp_get_num_procs() << std::endl;
    }
    omp_set_dynamic(0);  // отключаем динамический выбор количества потоков
    /**
     * Выбрать количество потоков можно:
     *  вызовом функции omp_set_num_threads(4);
     *  установкой переменной окружения export OMP_NUM_THREADS=4
     */

    auto begin = std::chrono::steady_clock::now();
    float pi = calculate_pi_with_monte_carlo(points_count, verbose);
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Pi is calculated by " << points_count << " points in " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
    std::cout << "Pi approximately equal " << pi << std::endl;

    return 0;
}