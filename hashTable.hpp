/*
*
* Hash-table with lists(key, value and ttl).
*
*/
#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#define TABLE_SIZE 256

#include "linkedList.hpp"

class hashTable
{
private:
    linkedList* m_table[TABLE_SIZE];
    queuePUB* m_queuePUB;

    unsigned int hash(void* key, unsigned int lenKey);
public:
    hashTable();
    ~hashTable();
    bool setQueuePUB(queuePUB* queuePUB);
    uint8_t insert(char* nameTable, void* key, void* value, unsigned int lenKey, unsigned int lenValue, uint64_t ttl_sec);
    uint8_t remove(void* key, unsigned int lenKey);
    Node*     find(void* key, unsigned int lenKey);
};

#endif // HASHTABLE_HPP
