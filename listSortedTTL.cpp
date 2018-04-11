#include "listSortedTTL.hpp"

listSortedTTL::listSortedTTL() : m_head(nullptr) {}

listSortedTTL::~listSortedTTL()
{
    nodeTTL* node = m_head;

    while( m_head != nullptr )
    {
        m_head = node->m_next;
        delete node;
        node = m_head;
    }
}

nodeTTL* listSortedTTL::top()
{
    return m_head;
}

bool listSortedTTL::push(char* nameTable, void* key, unsigned int lenKey, unsigned long long int ttl_sec)
{
    if( m_head == nullptr )
    {
        m_head = new nodeTTL;

        m_head->m_nameTable = (char*)malloc(strlen(nameTable));
        memcpy(m_head->m_nameTable, nameTable, strlen(nameTable));
        m_head->m_key = (void*)malloc(lenKey);
        memcpy(m_head->m_key, key, lenKey);
        m_head->m_lenKey = lenKey;
        m_head->m_ttl_sec = ttl_sec;
        m_head->m_next = nullptr;
        return true;
    }
    else
    {
        if( m_head->m_next != nullptr )
        {
            if( m_head->m_ttl_sec >= ttl_sec )
            {
                nodeTTL* node = new nodeTTL;

                node->m_nameTable = (char*)malloc(strlen(nameTable));
                memcpy(node->m_nameTable, nameTable, strlen(nameTable));
                node->m_key = (void*)malloc(lenKey);
                memcpy(node->m_key, key, lenKey);
                node->m_lenKey = lenKey;
                node->m_ttl_sec = ttl_sec;
                node->m_next = m_head;
                m_head = node;
                return true;
            }
            else
            {
                nodeTTL* node = m_head;

                while( node->m_next != nullptr )
                {
                    if( node->m_next->m_ttl_sec >= ttl_sec )
                    {
                        break;
                    }
                    node = node->m_next;
                }

                nodeTTL* nodeNext = node->m_next;

                node->m_next = new nodeTTL;
                node->m_next->m_nameTable = (char*)malloc(strlen(nameTable));
                memcpy(node->m_next->m_nameTable, nameTable, strlen(nameTable));
                node->m_next->m_key = (void*)malloc(lenKey);
                memcpy(node->m_next->m_key, key, lenKey);
                node->m_next->m_lenKey = lenKey;
                node->m_next->m_ttl_sec = ttl_sec;
                node->m_next->m_next = nodeNext;
                return true;
            }
        }
        else
        {
            if( m_head->m_ttl_sec >= ttl_sec )
            {
                nodeTTL* node = new nodeTTL;

                node->m_nameTable = (char*)malloc(strlen(nameTable));
                memcpy(node->m_nameTable, nameTable, strlen(nameTable));
                node->m_key = (void*)malloc(lenKey);
                memcpy(node->m_key, key, lenKey);
                node->m_lenKey = lenKey;
                node->m_ttl_sec = ttl_sec;
                node->m_next = m_head;
                m_head = node;
                return true;
            }
            else
            {
                m_head->m_next = new nodeTTL;

                m_head->m_next->m_nameTable = (char*)malloc(strlen(nameTable));
                memcpy(m_head->m_next->m_nameTable, nameTable, strlen(nameTable));
                m_head->m_next->m_key = (void*)malloc(lenKey);
                memcpy(m_head->m_next->m_key, key, lenKey);
                m_head->m_next->m_lenKey = lenKey;
                m_head->m_next->m_ttl_sec = ttl_sec;
                m_head->m_next->m_next = nullptr;
                return true;
            }
        }
    }
}

bool listSortedTTL::pop()
{
    if( m_head == nullptr )
    {
        return false;
    }
    else
    {
        nodeTTL* node = m_head;
        m_head = node->m_next;
        delete node;
        return true;
    }
    return false;
}
