#include <random>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <omp.h>

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

float calculate_pi_with_monte_carlo(size_t points_count=100, bool verbose=false) {
    size_t in_circle_points = 0;

#pragma omp parallel default(none) shared(points_count, in_circle_points, verbose, std::cout)
    {
#pragma omp single
        {
            if (verbose) {
                std::cout << "Num of OMP threads: " << omp_get_num_threads() << std::endl;
            }
        }

        std::random_device rd;
        uint32_t seed = rd();
        seed = (seed & 0xFFFFFFF0) | (omp_get_thread_num() + 1);
        std::mt19937 gen(seed);
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

        size_t local_in_circle_points = 0;
#pragma omp for schedule(static)
        for (size_t i = 0; i < points_count; ++i) {
            float x = dis(gen);
            float y = dis(gen);
            if (verbose){
                std::cout << (ParallelStream() << "Thread №" << omp_get_thread_num() << " generated point (" << x << ";" << y <<"). \n").str();
            }
            if (x * x + y * y <= 1) {
                ++local_in_circle_points;
            }
        }
#pragma omp atomic
        in_circle_points += local_in_circle_points;
    }

    const float total_square = 4.0f;
    float square_relation_monte_carlo = static_cast<float >(points_count) / static_cast<float >(in_circle_points);
    float pi = total_square / square_relation_monte_carlo;

    return pi;
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
    if (argc < 2) {
        std::cout << "Specify the point count.";
        return -1;
    }
    const size_t points_count = std::stoul(argv[1]);

    bool verbose = false;
    if((argc > 2) && (std::string(argv[2]) == "verbose")) {
        verbose = true;
    }

    if (verbose){
        std::cout << "Num of CPU: " << omp_get_num_procs() << std::endl;
    }
    omp_set_dynamic(0);
    // omp_set_num_threads(4);
    // export OMP_NUM_THREADS=4

    auto begin = std::chrono::steady_clock::now();
    float pi = calculate_pi_with_monte_carlo(points_count, verbose);
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Pi is calculated by " << points_count << " points in " << convert_us_to_human_readable(elapsed_us.count()) << "." << std::endl;
    std::cout << "Pi approximately equal " << pi << std::endl;

    return 0;
}