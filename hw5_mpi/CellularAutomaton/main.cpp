#include <map>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <random>
#include <algorithm>

#include <mpi/mpi.h>

/**
 * @brief Преобразует состояния ячеек в соответствии с правилом.
 */
class CellularTransformer{
public:
    /**
     * @brief Заполняет таблицу преобразований.
     * @param rule_no Номер правила.
     */
    explicit CellularTransformer(uint8_t rule_no=110) {
        uint8_t state = 0x07;
        for (auto i = 0; i < 8; ++i){
            transformations_[{(state >> 2) & 1, (state >> 1) & 1, state & 1}] = static_cast<bool>((rule_no >> (7 - i)) & 1);
            state -= 1;
        }
    }

    /**
     * @brief Получить новое состояние ячейки.
     * @param left Состояние ячейки слева.
     * @param center Состояние данной ячейки.
     * @param right Состояние ячейки справа.
     * @return Новое состояние данной ячейки.
     */
    bool transform(bool left, bool center, bool right) {
        return transformations_[{left, center, right}];
    }
private:
    std::map<std::tuple<bool, bool, bool>, bool> transformations_;
};

/**
 * @brief Тип границ автомата
 */
enum BorderType {
    ZERO = 0,  // на границах нули
    ONE = 1,  // на границах единицы
    PERIODIC = 2  // границы замкнуты по кругу
};

/**
 * @brief Синхронно печатает состояние автомата в stdout.
 * @param rank Номер текущего процесса в MPI.
 * @param comm_size Всего процессов в MPI.
 * @param cells Ячейки, за которые отвечает данный процесс (с ячейками-призраками).
 * @param block_size Количество ячеек, за которые отвечает данный процесс (без ячеек-призраков).
 * @param status Указатель на MPI_STATUS.
 */
void print_all(int rank, int comm_size, bool* cells, size_t block_size, MPI_Status* status) {
    if (rank == 0) {  // начинаем слева
        for (size_t j = 1; j < block_size + 1; ++j) {
            std::cout << static_cast<int>(cells[j]) << " " << std::flush;
        }
        if (comm_size > 1) {  // если процесс только один
            MPI_Send(nullptr, 0, MPI_CXX_BOOL, rank + 1, 3, MPI_COMM_WORLD);
        } else {
            std::cout << std::endl << std::flush;
        }
    }
    else if (rank == comm_size - 1){  // конец ленты автомата
        MPI_Recv(nullptr, 0, MPI_CXX_BOOL, rank - 1, 3, MPI_COMM_WORLD, status);
        for (size_t j = 1; j < block_size + 1; ++j) {
            std::cout << static_cast<int>(cells[j]) << " " << std::flush;
        }
        std::cout << std::endl << std::flush;
    } else {  // середина ленты автомата
        MPI_Recv(nullptr, 0, MPI_CXX_BOOL, rank - 1, 3, MPI_COMM_WORLD, status);
        for (size_t j = 1; j < block_size + 1; ++j) {
            std::cout << static_cast<int>(cells[j]) << " " << std::flush;
        }
        MPI_Send(nullptr, 0, MPI_CXX_BOOL, rank + 1, 3, MPI_COMM_WORLD);
    }
}

int main(int argc, char *argv[]) {
    int rank, comm_size;

    MPI_Init(nullptr, nullptr);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Status status;

    // Задаём параметры автомата
    uint8_t rule_no = 110;
    BorderType border = ZERO;
    size_t total_size = 20;
    size_t block_size = total_size / comm_size;
    if (rank == comm_size - 1) {
        block_size = total_size - block_size * rank;
    }
    uint64_t iterations_count = 10;
    bool verbose = true;

    for(int i = 1; i < argc; ++i) {
        std::string parameter_name(argv[i]);
        if (parameter_name == "-h") {
            std::cout << R"str(
Клеточный автомат Вольфрама с конечной лентой.
Использование:
CellularAutomaton [-r <номер_правила>] [-i <количество_итераций>] [-l <размер_автомата>] [-b <0|1|2>] [-v <0|1>]
Параметр -r - это номер правила для автомата в соответсвии с кодом Вольфрама.
Параметр -i - количество пересчёта состояний автомата.
Параметр -l - длина ленты автомата (размер состояния, количество клеток).
Параметр -b - тип границы: 0 - границы заполняются нулями, 1 - границы заполняются единицами, 2 - границы замкнуты периодически.
Параметр -v - нужно ли печатать состояния: 1 - нужно, 0 - не нужно.
Все параметры необязательные.
Параметры по умолчанию:
CellularAutomaton -r 110 -i 10 -l 20 -b 0 -v 1
)str" << std::endl;
            return 0;
        }
        if (parameter_name == "-r"){
            rule_no = static_cast<uint8_t>(std::stoi(argv[++i]));
            continue;
        }
        if (parameter_name == "-i") {
            iterations_count = std::stoul(argv[++i]);
            continue;
        }
        if (parameter_name == "-l") {
            total_size = std::stoul(argv[++i]);
            block_size = total_size / comm_size;
            if (rank == comm_size - 1) {
                block_size = total_size - block_size * rank;
            }
            continue;
        }
        if (parameter_name == "-b") {
            border = static_cast<BorderType>(std::stoi(argv[++i]));
            continue;
        }
        if (parameter_name == "-v") {
            verbose = static_cast<bool>(std::stoi(argv[++i]));
            continue;
        }
    }

    // Инициализируем преобразователь состояний.
    CellularTransformer transformer(rule_no);

    // Заполняем ячейки случайно
    std::random_device rd;
    std::mt19937 gen((rd() & 0xFFFFFFF0) | (rank + 1));
    std::uniform_int_distribution<> dis(0, 1);

    auto cells = new bool[block_size + 2];
    for (size_t i = 1; i < block_size + 1; ++i){
        cells[i] = static_cast<bool>(dis(gen));
    }

    if (verbose) {  // печать состояния автомата
        MPI_Barrier(MPI_COMM_WORLD);
        print_all(rank, comm_size, cells, block_size, &status);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    for (uint64_t i = 0; i < iterations_count; ++i) {
        // Получаем значение ячейки слева
        MPI_Request receiving_left;
        if (rank == 0) {
            switch (border) {
                default:
                case ZERO:
                    cells[0] = false;
                    break;
                case ONE:
                    cells[0] = true;
                    break;
                case PERIODIC:
                    MPI_Irecv(cells, 1, MPI_CXX_BOOL, comm_size - 1, 0, MPI_COMM_WORLD, &receiving_left);
                    break;
            }
        } else {
            MPI_Irecv(cells, 1, MPI_CXX_BOOL, rank - 1, 0, MPI_COMM_WORLD, &receiving_left);
        }
        if (rank == comm_size - 1) {
            if (border == PERIODIC) {
                MPI_Ssend(cells + block_size, 1, MPI_CXX_BOOL, 0, 0, MPI_COMM_WORLD);
            }
        } else {
            MPI_Ssend(cells + block_size, 1, MPI_CXX_BOOL, rank + 1, 0, MPI_COMM_WORLD);
        }

        // Получаем значение ячейки справа
        MPI_Request receiving_right;
        if (rank == comm_size - 1) {
            switch (border) {
                default:
                case ZERO:
                    cells[block_size + 1] = false;
                    break;
                case ONE:
                    cells[block_size + 1] = true;
                    break;
                case PERIODIC:
                    MPI_Irecv(cells + block_size + 1, 1, MPI_CXX_BOOL, 0, 1, MPI_COMM_WORLD, &receiving_left);
                    break;
            }
        } else {
            MPI_Irecv(cells + block_size + 1, 1, MPI_CXX_BOOL, rank + 1, 1, MPI_COMM_WORLD, &receiving_left);
        }
        if (rank == 0) {
            if (border == PERIODIC) {
                MPI_Ssend(cells + 1, 1, MPI_CXX_BOOL, comm_size - 1, 1, MPI_COMM_WORLD);
            }
        } else {
            MPI_Ssend(cells + 1, 1, MPI_CXX_BOOL, rank - 1, 1, MPI_COMM_WORLD);
        }

        // Обновляем состояние
        auto new_cells = new bool[block_size + 2];
        for (size_t j = 1; j < block_size + 1; ++j) {
            new_cells[j] = transformer.transform(cells[j - 1], cells[j], cells[j + 1]);
        }
        for (size_t j = 1; j < block_size + 1; ++j) {
            cells[j] = new_cells[j];
        }
        delete[] new_cells;

        if (verbose) {  // печать состояния автомата
            MPI_Barrier(MPI_COMM_WORLD);
            print_all(rank, comm_size, cells, block_size, &status);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    delete[] cells;
    MPI_Finalize();
    return 0;
}