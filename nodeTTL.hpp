/*
*
* Node whith table name, key and ttl;
*
*/
#ifndef NODETTL_HPP
#define NODETTL_HPP

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "define.h"

class nodeTTL
{
public:
    nodeTTL* m_next;
    char* m_nameTable;
    void* m_key;
    unsigned int m_lenKey;
    uint64_t m_ttl_sec;

    nodeTTL() : m_next(nullptr), m_nameTable(nullptr), m_key(nullptr), m_lenKey(0), m_ttl_sec(0) {}
    ~nodeTTL()
    {
        if( m_nameTable != nullptr )
            free(m_nameTable);
        if( m_key != nullptr )
            free(m_key);
    }
};

#endif // NODETTL_HPP
