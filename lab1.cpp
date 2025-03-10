#include <iostream>
#include <boost/thread.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <vector>
#include <iomanip> // Для std::setw
/*
// Глобальный мьютекс для защиты вывода в консоль
std::mutex cout_mutex;

// Задание 1

// Рекурсивная функция для вычисления чисел Фибоначчи
long long fibonacciRecursive(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacciRecursive(n - 1) + fibonacciRecursive(n - 2);
}

// Функция, которую будет выполнять каждый поток
void workerThread(int n, int threadId) {
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Thread " << threadId << " started, calculating F(" << n << ")" << std::endl;
    }

    long long result = fibonacciRecursive(n);

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Thread " << threadId << " finished, F(" << n << ") = " << result << std::endl;
    }
}

int main() {
    int numThreads;
    int fibNumber;

    std::cout << "Enter the number of threads: ";
    std::cin >> numThreads;

    std::cout << "Enter the Fibonacci number to calculate: ";
    std::cin >> fibNumber;

    //Многопоточное вычисление
    std::cout << "\nMultithreaded calculation:" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<boost::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(boost::thread(workerThread, fibNumber, i + 1));
    }

    for (auto& thread : threads) {
        thread.join(); // Ожидаем завершения всех потоков
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Multithreaded execution time: " << duration.count() << " milliseconds" << std::endl;

    //Последовательное вычисление
    std::cout << "\nSequential calculation:" << std::endl;
    auto start_seq = std::chrono::high_resolution_clock::now();

    long long result_seq;

    for (int i = 0; i < numThreads; ++i) {
      result_seq = fibonacciRecursive(fibNumber); //Сохранение результата
    }

    auto end_seq = std::chrono::high_resolution_clock::now();
    auto duration_seq = std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq);
    std::cout << "Sequential result: " << result_seq << std::endl;
    std::cout << "Sequential execution time: " << duration_seq.count() << " milliseconds" << std::endl;

}*/


// Задание 2


const int NUM_INCREMENTS = 1000000;

// 1. Без синхронизации (демонстрация гонки данных)
void incrementUnsynchronized(long long& counter, int threadId) {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        counter++;
    }
    std::cout << "Thread " << threadId << " (Unsynchronized) finished." << std::endl;
}

// 2. С использованием atomic<int>
void incrementAtomic(std::atomic<long long>& counter, int threadId) {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        counter++;
    }
    std::cout << "Thread " << threadId << " (Atomic) finished." << std::endl;
}

// 3. С использованием mutex
void incrementMutex(long long& counter, std::mutex& mutex, int threadId) {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        std::lock_guard<std::mutex> lock(mutex); 
        counter++;                                   
    }                                               
    std::cout << "Thread " << threadId << " (Mutex) finished." << std::endl;
}

// Функция для измерения времени выполнения
template <typename Func>
long long measureExecutionTime(Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main() {
    std::vector<int> numThreadsToTest = {2, 4, 8};

    for (int numThreads : numThreadsToTest) {
        std::cout << "\n--- Testing with " << numThreads << " threads ---" << std::endl;

        // 1. Unsynchronized
        long long unsynchronizedCounter = 0;
        auto unsynchronizedFunc = [&]() {
            std::vector<std::thread> threads;
            for (int i = 0; i < numThreads; ++i) {
                threads.emplace_back(incrementUnsynchronized, std::ref(unsynchronizedCounter), i + 1);
            }
            for (auto& thread : threads) {
                thread.join();
            }
        };
        long long unsynchronizedTime = measureExecutionTime(unsynchronizedFunc);
        std::cout << "Unsynchronized Counter: " << std::setw(12) << unsynchronizedCounter << ", Time: " << std::setw(5) << unsynchronizedTime << " ms" << std::endl;

        // 2. Atomic
        std::atomic<long long> atomicCounter{0};
        auto atomicFunc = [&]() {
            std::vector<std::thread> threads;
            for (int i = 0; i < numThreads; ++i) {
                threads.emplace_back(incrementAtomic, std::ref(atomicCounter), i + 1);
            }
            for (auto& thread : threads) {
                thread.join();
            }
        };
        long long atomicTime = measureExecutionTime(atomicFunc);
        std::cout << "Atomic        Counter: " << std::setw(12) << atomicCounter << ", Time: " << std::setw(5) << atomicTime << " ms" << std::endl;

        // 3. Mutex
        long long mutexCounter = 0;
        std::mutex mutex;
        auto mutexFunc = [&]() {
            std::vector<std::thread> threads;
            for (int i = 0; i < numThreads; ++i) {
                threads.emplace_back(incrementMutex, std::ref(mutexCounter), std::ref(mutex), i + 1);
            }
            for (auto& thread : threads) {
                thread.join();
            }
        };
        long long mutexTime = measureExecutionTime(mutexFunc);
        std::cout << "Mutex         Counter: " << std::setw(12) << mutexCounter << ", Time: " << std::setw(5) << mutexTime << " ms" << std::endl;
    }

    return 0;
}