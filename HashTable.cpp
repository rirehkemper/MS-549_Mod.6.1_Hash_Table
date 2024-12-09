#include "HashTable.h"
#include <iostream>

template <typename K, typename V>
HashTable<K, V>::HashTable(size_t size, CollisionMethod method)
    : tableSize(size), currentSize(0), collisionMethod(method) {
    table.resize(tableSize);
    isOccupied.resize(tableSize, false);  // Initialize all slots as unoccupied
}

template <typename K, typename V>
HashTable<K, V>::~HashTable() {}

template <typename K, typename V>
size_t HashTable<K, V>::hashFunction(const K& key) const {
    return std::hash<K>{}(key) % tableSize;
}

template <typename K, typename V>
size_t HashTable<K, V>::probe(size_t index, size_t i) const {
    if (collisionMethod == LINEAR_PROBING) {
        return (index + i) % tableSize;
    }
    else if (collisionMethod == QUADRATIC_PROBING) {
        return (index + i * i) % tableSize;
    }
    return index;
}

template <typename K, typename V>
void HashTable<K, V>::insert(const K& key, const V& value) {
    size_t index = hashFunction(key);
    size_t i = 0;
    while (isOccupied[probe(index, i)]) {
        if (table[probe(index, i)].first == key) {
            // If the key is already in the table, update its value
            table[probe(index, i)].second = value;
            return;
        }
        i++;
    }

    table[probe(index, i)] = { key, value };
    isOccupied[probe(index, i)] = true;
    currentSize++;

    // Rehash if the table is 75% full
    if (currentSize > tableSize * 0.75) {
        rehash();
    }
}

template <typename K, typename V>
bool HashTable<K, V>::retrieve(const K& key, V& value) {
    size_t index = hashFunction(key);
    size_t i = 0;
    while (isOccupied[probe(index, i)]) {
        if (table[probe(index, i)].first == key) {
            value = table[probe(index, i)].second;
            return true;
        }
        i++;
    }
    return false;
}

template <typename K, typename V>
bool HashTable<K, V>::remove(const K& key) {
    size_t index = hashFunction(key);
    size_t i = 0;
    while (isOccupied[probe(index, i)]) {
        if (table[probe(index, i)].first == key) {
            isOccupied[probe(index, i)] = false;
            currentSize--;
            return true;
        }
        i++;
    }
    return false;
}

// Rehash function to resize the table when it becomes too full
template <typename K, typename V>
void HashTable<K, V>::rehash() {
    size_t oldSize = tableSize;
    tableSize *= 2; // Double the size of the table
    std::vector<std::pair<K, V>> oldTable = table;
    std::vector<bool> oldOccupied = isOccupied;

    table.clear();
    isOccupied.clear();
    table.resize(tableSize);
    isOccupied.resize(tableSize, false);

    currentSize = 0;

    // Reinsert all the elements
    for (size_t i = 0; i < oldSize; i++) {
        if (oldOccupied[i]) {
            insert(oldTable[i].first, oldTable[i].second);
        }
    }
}

// Print the hash table (for debugging purposes)
template <typename K, typename V>
void HashTable<K, V>::print() {
    for (size_t i = 0; i < tableSize; ++i) {
        if (isOccupied[i]) {
            std::cout << "[" << table[i].first << ": " << table[i].second << "] ";
        }
        else {
            std::cout << "[--] ";
        }
    }
    std::cout << std::endl;
}