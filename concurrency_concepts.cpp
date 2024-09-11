//
// Created by benjamin on 06.09.24.
//

#include <iostream>
#include <vector>
#include <thread>

void process_vector(std::vector<int> &vector, int numberOfThreads, int threadID) {
    for (int i = threadID; i < vector.size(); i += numberOfThreads) {
        std::cout << vector[i] << std::endl;
    }
}

int main() {
    std::vector<int> numbers;
    for (int i = 0; i < 100; ++i) {
        numbers.push_back(i);
    }
    int numberOfThreads = 12;
    std::vector<std::thread> threads;
    for (int i = 0; i < numberOfThreads; ++i) {
        threads.emplace_back(process_vector, std::ref(numbers), numberOfThreads, i);
    };
    for (auto &thread: threads) {
        thread.join();
    }
}