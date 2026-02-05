// bogo sort.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <thread>
#include <future>

bool check_sorted(int* arr, int size) {
    for (int i = 1; i < size; i++) {
        if (arr[i - 1] > arr[i]) {
            return false;
        }
    }
    return true;
}

void print_array(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}


int* sort_list(int* arr, int size, bool* kill) {
    int* copy = new int[size];
    memcpy(copy, arr, size * sizeof(int));
    while (!check_sorted(copy, size)) {
        if (*kill) {
            return nullptr;
        }
        std::random_shuffle(&copy[0], &copy[size]);
    }
    return copy;
}

int any_futures_ready(std::future<int*>* futures, int size) {
    for (int i = 0; i < size; i++) {
        if (futures[i].wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            return i;
        }
    }
    return -1;
}

int main()
{
    std::string input = "c";
    while (input != "x") {

        const int ARR_SIZE = 100;
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> rand(1, 100);

        int arr[ARR_SIZE];
        for (int i = 0; i < ARR_SIZE; i++) {
            arr[i] = rand(rng);
        }

        std::cout << "Unsorted array:" << std::endl;
        print_array(arr, ARR_SIZE);

        bool kill = false;
        const int CPUS = std::thread::hardware_concurrency();
        std::future<int*>* futures = new std::future<int*>[CPUS];
        for (int i = 0; i < CPUS; i++) {
            futures[i] = std::async(std::launch::async, sort_list, arr, ARR_SIZE, &kill);
        }

        while (true) {
            int res = any_futures_ready(futures, CPUS);
            if (res != -1) {
                int* sorted = futures[res].get();
                kill = true;
                std::cout << "A future was ready" << std::endl;
                std::cout << "Sorted array" << std::endl;
                print_array(sorted, ARR_SIZE);
                break;
            }
        } 
        
        std::cout << "Press x to end" << std::endl;
        std::cin >> input;
    }
}