/*
*
* List with node(table name and hash-table)
*
*/
#ifndef LISTTABLE_HPP
#define LISTTABLE_HPP

#include "nodeTable.hpp"

class listTable
{
private:
    nodeTable* m_head;
public:
    listTable();
    ~listTable();
    uint8_t createTable(char* nameTable);
    uint8_t removeTable(char* nameTable);
    uint8_t insert(char* nameTable, void* key, void* value, unsigned int lenKey, unsigned int lenValue, uint64_t ttl_sec);
    uint8_t remove(char* nameTable, void* key, unsigned int lenKey);
    Node*     find(char* nameTable, void* key, unsigned int lenKey);
};

#endif // LISTTABLE_HPP
