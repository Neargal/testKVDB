/*
*
* Node with key, value and ttl.
*
*/
#ifndef NODE_HPP
#define NODE_HPP

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "define.h"

class Node
{
public:
    Node* m_next;
    void* m_key;
    void* m_value;
    unsigned int m_lenKey;
    unsigned int m_lenValue;
    uint64_t m_ttl_sec;

    Node() : m_next(nullptr), m_key(nullptr), m_value(nullptr), m_lenKey(0), m_lenValue(0), m_ttl_sec(0) {}
    ~Node()
    {
        if( m_key != nullptr )
            free(m_key);
        if( m_value != nullptr )
            free(m_value);
    }
};

#endif // NODE_HPP
