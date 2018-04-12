#ifndef NODEPUB_HPP
#define NODEPUB_HPP

#include <zmq.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "define.h"

class nodePUB
{
public:
    nodePUB* m_next;
    char* m_nameTable;
    void* m_key;
    unsigned int m_lenKey;
    uint8_t m_reason;

    nodePUB() : m_next(nullptr), m_nameTable(nullptr), m_key(nullptr), m_lenKey(0), m_reason(0) {}
    ~nodePUB()
    {
        if( m_nameTable != nullptr )
        {
            free(m_nameTable);
        }
        if( m_key != nullptr )
        {
            free(m_key);
        }
    }
};

#endif // NODEPUB_HPP
