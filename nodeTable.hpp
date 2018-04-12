/*
*
* Node with table name and hash-table(key, value and ttl).
*
*/
#ifndef NODETABLE_HPP
#define NODETABLE_HPP

#include "hashTable.hpp"

class nodeTable
{
public:
    nodeTable* m_next;
    hashTable* m_table;
    char* m_name;
    nodeTable() : m_next(nullptr), m_table(nullptr), m_name(nullptr) {}
    ~nodeTable()
    {
        if( m_name != nullptr )
            free(m_name);
        if( m_table != nullptr )
            delete m_table;
    }
};

#endif // NODETABLE_HPP
