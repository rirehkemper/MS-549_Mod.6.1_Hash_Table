#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <functional>
#include <chrono>
#include <fstream>
#include <stdexcept>

// Utility functions for prime number calculations
bool isPrime(size_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (size_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

size_t nextPrime(size_t n) {
    while (!isPrime(n)) ++n;
    return n;
}

template <typename K, typename V>
class HashTable {
public:
    enum CollisionMethod { CHAINING, LINEAR_PROBING, QUADRATIC_PROBING, DOUBLE_HASHING };

    HashTable(size_t size = 100, CollisionMethod method = CHAINING);
    ~HashTable() {}

    void insert(const K& key, const V& value);
    bool retrieve(const K& key, V& value) const;
    bool remove(const K& key);
    void print() const;
    void measurePerformance(const std::string& methodName);

private:
    size_t hashFunction(const K& key) const;
    size_t probe(size_t index, size_t i, const K& key) const;
    void rehash();

    std::vector<std::list<std::pair<K, V>>> chainedTable; // For chaining
    std::vector<std::pair<K, V>> probedTable;             // For probing
    std::vector<bool> isOccupied;                         // Tracks occupied slots in probing
    size_t tableSize;
    size_t currentSize;
    CollisionMethod collisionMethod;
};

// Constructor
template <typename K, typename V>
HashTable<K, V>::HashTable(size_t size, CollisionMethod method)
    : tableSize(nextPrime(size)), currentSize(0), collisionMethod(method) {
    if (method == CHAINING) {
        chainedTable.resize(tableSize);
    }
    else {
        probedTable.resize(tableSize);
        isOccupied.resize(tableSize, false);
    }
}

// Hash function
template <typename K, typename V>
size_t HashTable<K, V>::hashFunction(const K& key) const {
    return std::hash<K>{}(key) % tableSize;
}

// Probing function
    //https://iq.opengenus.org/different-collision-resolution-techniques-in-hashing/
    // https://research.cs.vt.edu/AVresearch/hashing/collision.php
    //https://www.programiz.com/dsa/hash-table
template <typename K, typename V>
size_t HashTable<K, V>::probe(size_t index, size_t i, const K& key) const {

    if (i >= tableSize) {
        throw std::runtime_error("Hash table full: Unable to resolve collision with double hashing");
    }

    if (collisionMethod == LINEAR_PROBING) {
        return (index + i) % tableSize;
    }
    else if (collisionMethod == QUADRATIC_PROBING) {
        return (index + i * i) % tableSize;
    }
    else if (collisionMethod == DOUBLE_HASHING) {
        size_t step = 1 + (std::hash<K>{}(key) % (tableSize - 1));
        if (step == 0) step = 1; // Ensure step is non-zero
        return (index + step * i) % tableSize;
    }
    return index;
}

// Insert function
template <typename K, typename V>
void HashTable<K, V>::insert(const K& key, const V& value) {
    size_t index = hashFunction(key);
    if (collisionMethod == CHAINING) {
        chainedTable[index].emplace_back(key, value);
    }
    else {
        size_t i = 0;
        while (isOccupied[probe(index, i, key)]) {
            i++;
        }
        probedTable[probe(index, i, key)] = { key, value };
        isOccupied[probe(index, i, key)] = true;
    }
    currentSize++;
    if (currentSize > tableSize * 0.75) {
        rehash();
    }
}

// Retrieve function
template <typename K, typename V>
bool HashTable<K, V>::retrieve(const K& key, V& value) const {
    size_t index = hashFunction(key);
    if (collisionMethod == CHAINING) {
        for (const auto& pair : chainedTable[index]) {
            if (pair.first == key) {
                value = pair.second;
                return true;
            }
        }
    }
    else {
        size_t i = 0;
        while (isOccupied[probe(index, i, key)]) {
            if (probedTable[probe(index, i, key)].first == key) {
                value = probedTable[probe(index, i, key)].second;
                return true;
            }
            i++;
        }
    }
    return false;
}

// Remove function
template <typename K, typename V>
bool HashTable<K, V>::remove(const K& key) {
    size_t index = hashFunction(key);
    if (collisionMethod == CHAINING) {
        auto& bucket = chainedTable[index];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                bucket.erase(it);
                return true;
            }
        }
    }
    else {
        size_t i = 0;
        while (isOccupied[probe(index, i, key)]) {
            if (probedTable[probe(index, i, key)].first == key) {
                isOccupied[probe(index, i, key)] = false;
                return true;
            }
            i++;
        }
    }
    return false;
}

// Rehashing - not be be confused with fighting again.
template <typename K, typename V>
void HashTable<K, V>::rehash() {
    std::cout << "Rehashing... Current table size: " << tableSize << "\n";
    size_t oldSize = tableSize;
    tableSize = nextPrime(tableSize * 2);
    currentSize = 0;

    if (collisionMethod == CHAINING) {
        auto oldTable = std::move(chainedTable);
        chainedTable.resize(tableSize);

        for (const auto& bucket : oldTable) {
            for (const auto& pair : bucket) {
                insert(pair.first, pair.second);
            }
        }
    }
    else {
        auto oldTable = std::move(probedTable);
        auto oldOccupied = std::move(isOccupied);

        probedTable.resize(tableSize);
        isOccupied.resize(tableSize, false);

        for (size_t i = 0; i < oldSize; ++i) {
            if (oldOccupied[i]) {
                insert(oldTable[i].first, oldTable[i].second);
            }
        }
    }
    std::cout << "Rehash complete. New table size: " << tableSize << "\n";
}

// Print function
template <typename K, typename V>
void HashTable<K, V>::print() const {
    if (collisionMethod == CHAINING) {
        for (size_t i = 0; i < tableSize; ++i) {
            std::cout << "Bucket " << i << ": ";
            for (const auto& pair : chainedTable[i]) {
                std::cout << "[" << pair.first << ": " << pair.second << "] ";
            }
            std::cout << "\n";
        }
    }
    else {
        for (size_t i = 0; i < tableSize; ++i) {
            if (isOccupied[i]) {
                std::cout << "[" << probedTable[i].first << ": " << probedTable[i].second << "] ";
            }
            else {
                std::cout << "[--] ";
            }
        }
        std::cout << "\n";
    }
}

// Measure performance
template <typename K, typename V>
void HashTable<K, V>::measurePerformance(const std::string& methodName) {
    std::ofstream outFile("performance_results.csv", std::ios::app);
    if (!outFile) {
        std::cerr << "Error: Could not open performance_results.csv for writing.\n";
        return;
    }

    outFile << "\nCollision Handling Method: " << methodName << "\n";
    outFile << "Operation,Size,Time(ms)\n";
    std::cout << "Operation\tSize\tTime (ms)\n";

    std::vector<size_t> sizes = { 100, 1000, 10000 };
    for (size_t size : sizes) {
        std::vector<std::pair<K, V>> testData;
        for (size_t i = 0; i < size; ++i) {
            testData.emplace_back(std::to_string(i), static_cast<int>(i));
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (const auto& pair : testData) {
            insert(pair.first, pair.second);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto insertTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        outFile << "Insert," << size << "," << insertTime << "\n";

        start = std::chrono::high_resolution_clock::now();
        for (const auto& pair : testData) {
            V temp;
            retrieve(pair.first, temp);
        }
        end = std::chrono::high_resolution_clock::now();
        auto retrieveTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        outFile << "Retrieve," << size << "," << retrieveTime << "\n";

        start = std::chrono::high_resolution_clock::now();
        for (const auto& pair : testData) {
            remove(pair.first);
        }
        end = std::chrono::high_resolution_clock::now();
        auto removeTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        outFile << "Remove," << size << "," << removeTime << "\n";
    }

    outFile.close();
    std::cout << "Performance results saved to performance_results.csv.\n";
}
