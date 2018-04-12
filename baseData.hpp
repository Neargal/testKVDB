/*
*
* Hash-table with lists(table name and hash-table) and sorted list ttl;
*
*/
#ifndef BASEDATA_HPP
#define BASEDATA_HPP

#include "listTable.hpp"
#include "listSortedTTL.hpp"

class baseData
{
private:
    listTable* m_table[TABLE_SIZE];
    listSortedTTL* m_listTTL;
    queuePUB* m_queuePUB;

    unsigned int hash(char* nameTable);
public:
    baseData();
    ~baseData();
    bool setQueuePUB(queuePUB* queuePUB);
    uint8_t createTable(char* nameTable);
    uint8_t removeTable(char* nameTable);
    uint8_t insert(char* nameTable, void* key, void* value, unsigned int lenKey, unsigned int lenValue, uint64_t ttl_sec);
    uint8_t remove(char* nameTable, void* key, unsigned int lenKey);
    Node*     find(char* nameTable, void* key, unsigned int lenKey);
    uint8_t updateTTL();
};

#endif // BASEDATA_HPP
