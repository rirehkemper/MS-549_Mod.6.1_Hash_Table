#include "HashTable.h"

template <typename K, typename V>
HashTable<K, V>::HashTable(size_t size, CollisionMethod method)
    : tableSize(size), currentSize(0), collisionMethod(method) {
    if (method == CHAINING) {
        chainedTable.resize(tableSize);
    }
    else {
        probedTable.resize(tableSize);
        isOccupied.resize(tableSize, false);
    }
}

template <typename K, typename V>
HashTable<K, V>::~HashTable() {}

template <typename K, typename V>
size_t HashTable<K, V>::hashFunction(const K& key) const {
    return std::hash<K>{}(key) % tableSize;
}

template <typename K, typename V>
size_t HashTable<K, V>::probe(size_t index, size_t i, const K& key) const {
    if (collisionMethod == LINEAR_PROBING) {
        return (index + i) % tableSize;
    }
    else if (collisionMethod == QUADRATIC_PROBING) {
        return (index + i * i) % tableSize;
    }
    else if (collisionMethod == DOUBLE_HASHING) {
        size_t step = 1 + (std::hash<K>{}(key) % (tableSize - 1));
        return (index + step * i) % tableSize;
    }
    return index;
}

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
}

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

template <typename K, typename V>
void HashTable<K, V>::print() const {
    if (collisionMethod == CHAINING) {
        for (size_t i = 0; i < tableSize; ++i) {
            cout << "Bucket " << i << ": ";
            for (const auto& pair : chainedTable[i]) {
                cout << "[" << pair.first << ": " << pair.second << "] ";
            }
            cout << endl;
        }
    }
    else {
        for (size_t i = 0; i < tableSize; ++i) {
            if (isOccupied[i]) {
                cout << "[" << probedTable[i].first << ": " << probedTable[i].second << "] ";
            }
            else {
                cout << "[--] ";
            }
        }
        cout << endl;
    }
}

template <typename K, typename V>
void HashTable<K, V>::measurePerformance() {
    vector<size_t> sizes = { 100, 1000, 10000 };
    ofstream outFile("performance_results.csv");
    outFile << "Operation,Size,Time(ns)\n";

    for (size_t size : sizes) {
        vector<pair<K, V>> testData;
        for (size_t i = 0; i < size; ++i) {
            testData.emplace_back(to_string(i), i);
        }

        auto start = chrono::high_resolution_clock::now();
        for (const auto& [key, value] : testData) {
            insert(key, value);
        }
        auto end = chrono::high_resolution_clock::now();
        outFile << "Insert," << size << "," << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << "\n";

        start = chrono::high_resolution_clock::now();
        for (const auto& [key, _] : testData) {
            V temp;
            retrieve(key, temp);
        }
        end = chrono::high_resolution_clock::now();
        outFile << "Retrieve," << size << "," << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << "\n";

        start = chrono::high_resolution_clock::now();
        for (const auto& [key, _] : testData) {
            remove(key);
        }
        end = chrono::high_resolution_clock::now();
        outFile << "Remove," << size << "," << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << "\n";
    }
    outFile.close();
}
