#include <iostream>
#include <chrono>
#include <iomanip>
#include <random>
#include <algorithm>

#include <mpi/mpi.h>

/**
 * @brief Генерирует случайную строку заданной длины.
 * @param seed Начальное состояние генератора.
 * @param length Длина строки.
 * @return Случайная строка.
 */
std::string generate_random_string(int seed, size_t length=7) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis('a', 'z');

    std::string result;
    while (result.size() < length) {
        result += dis(gen);
    }
    return result;
}

/**
 * @brief Для хранения списка известных узлов.
 */
using NodesList = std::vector<std::pair<std::string, int>>;

/**
 * @brief Сериализация списка узлов.
 * @param nodes Список узлов.
 * @return Массив байт, содержащий список узлов.
 */
std::string serialize(const NodesList& nodes) {
    std::stringstream ss;
    for (auto& node: nodes) {
        ss << node.first;
        ss << '\x00';
        ss.write(reinterpret_cast<const char *>(&node.second), sizeof(int));
    }
    return ss.str();
}

/**
 * @brief Десериализация списка узлов.
 * @param data Массив байт со списком узлов.
 * @param length Длина массива байт.
 * @return Список узлов.
 */
NodesList deserialize(const char* data, size_t length) {
    NodesList result;
    size_t shift = 0;
    while (shift < length) {
        std::string name(data + shift);
        shift += name.size() + 1;
        int node_id;
        node_id = *reinterpret_cast<const int*>(data + shift);
        shift += sizeof(int);
        result.emplace_back(name, node_id);
    }
    return result;
}

int main(int argc, char *argv[]) {
    int rank, comm_size;

    MPI_Init(nullptr, nullptr);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Status status;

    // Сгенерируем имя для каждого процесса.
    std::random_device rd;
    const size_t length_name = 7;
    auto my_name = generate_random_string((rd() & 0xFFFFFFF0) | (rank + 1), length_name);

    // Подготовим распределение для генерирования номера следующего процесса.
    std::mt19937 rank_gen((rd() & 0xFFFFFFF0) | (rank + 1));
    std::uniform_int_distribution<> rank_dis(0, comm_size - 1);

    if (rank == 0) {
        // Начинаем знакомство.

        NodesList nodes;

        // Добавляем себя в список
        auto next_node = rank_dis(rank_gen);
        nodes.emplace_back(my_name, rank);
        auto serialized = serialize(nodes);

        if (nodes.size() == comm_size) {
            // Если знакомиться не с кем.
            for (auto& node: nodes) {
                std::cout << node.first + " " + std::to_string(node.second) + "\n";
            }
            MPI_Finalize();
            return 0;
        }

        if (next_node == rank) {
            ++next_node;  // Сами с собой не знакомимся.
        }

        MPI_Ssend(serialized.c_str(), serialized.size(), MPI_CHAR, next_node, 0, MPI_COMM_WORLD);
    }
    else {
        // Проверяем, не пришли ли данные и создаём буффер.
        int buf_size;
        MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_CHAR, &buf_size);

        // Получаем данные и десериализуем
        auto buf = new char[buf_size];
        MPI_Recv(buf, buf_size, MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
        auto nodes = deserialize(buf, buf_size);
        delete[] buf;

        // Добавляем себя в список знакомств
        nodes.emplace_back(my_name, rank);
        auto serialized = serialize(nodes);

        if (nodes.size() == comm_size) {
            // Если знакомиться больше не с кем
            for (auto& node: nodes) {
                std::cout << node.first + " " + std::to_string(node.second) + "\n";
            }
        } else {
            // Если познакомились ещё не со всеми, то ищем, кому передадим эстафету
            auto next_node = rank_dis(rank_gen);
            // Очень неэффективный способ получить уникальное случайное число (только для маленького количества процессов)
            while(std::find_if(nodes.begin(), nodes.end(), [&next_node](auto& node){
                return node.second == next_node;
            }) != nodes.end()) {
                next_node = rank_dis(rank_gen);
            }
            // Продолжаем знакомства
            MPI_Ssend(serialized.c_str(), serialized.size(), MPI_CHAR, next_node, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}