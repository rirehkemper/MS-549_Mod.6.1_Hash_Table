/* Allen Rehkemper 
MS549 Mod 6.1 Hash Table and Collisions
Dr. Coddington
12/08/2024
This project reviewed the development of hash tables and collision handling methods. I originally anticipated that
this project would take 3.5 hours, however I had some issues getting the HashTable.cpp file to work with the HashTable.h
file, this and some independent research increased my project time by about 2 hours. */

#include "HashTable.h"
#include <iostream>
#include <string>

int main() {
    size_t tableSize = 100;

    while (true) {
        // Display menu for collision handling methods
        std::cout << "\nChoose a collision handling method:\n";
        std::cout << "1. Separate Chaining\n";
        std::cout << "2. Linear Probing\n";
        std::cout << "3. Quadratic Probing\n";
        std::cout << "4. Double Hashing\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter your choice (1-5): ";

        int choice;
        std::cin >> choice;

        // Exit condition
        if (choice == 5) {
            std::cout << "Exiting program. Goodbye!\n";
            break;
        }

        // Validate choice
        while (choice < 1 || choice > 5) {
            std::cout << "Invalid choice. Please enter a number between 1 and 5: ";
            std::cin >> choice;
        }

        // Map choice to CollisionMethod
        HashTable<std::string, int>::CollisionMethod method;
        switch (choice) {
        case 1: method = HashTable<std::string, int>::CHAINING; break;
        case 2: method = HashTable<std::string, int>::LINEAR_PROBING; break;
        case 3: method = HashTable<std::string, int>::QUADRATIC_PROBING; break;
        case 4: method = HashTable<std::string, int>::DOUBLE_HASHING; break;
        }

        // Inform user of the selected method
        std::cout << "Selected collision handling method: ";
        std::string methodName;
        switch (method) {
        case HashTable<std::string, int>::CHAINING:
            methodName = "Separate Chaining";
            std::cout << methodName << "\n";
            break;
        case HashTable<std::string, int>::LINEAR_PROBING:
            methodName = "Linear Probing";
            std::cout << methodName << "\n";
            break;
        case HashTable<std::string, int>::QUADRATIC_PROBING:
            methodName = "Quadratic Probing";
            std::cout << methodName << "\n";
            break;
        case HashTable<std::string, int>::DOUBLE_HASHING:
            methodName = "Double Hashing";
            std::cout << methodName << "\n";
            break;
        }

        // Create the hash table with the chosen method
        HashTable<std::string, int> hashTable(tableSize, method);

        // Measure performance with the chosen method
        hashTable.measurePerformance(methodName);
    }

    return 0;
}
