#include "linkedList.hpp"

linkedList::linkedList() : m_head(nullptr), m_name(nullptr), m_queuePUB(nullptr) {}

linkedList::~linkedList()
{
    Node* node = m_head;

    while( m_head != nullptr )
    {
        m_queuePUB->push(m_name, m_head->m_key, m_head->m_lenKey, DELETED_PUB);
        m_head = node->m_next;
        delete node;
        node = m_head;
    }
    free(m_name);
}

bool linkedList::setQueuePUB(queuePUB* queuePUB)
{
    m_queuePUB = queuePUB;
    return true;
}

bool linkedList::setNameTable(char* nameTable)
{
    m_name = (char*)malloc(strlen(nameTable) + 1);
    memcpy(m_name, nameTable, strlen(nameTable));
    m_name[strlen(nameTable)] = 0;
    return true;
}

uint8_t linkedList::insert(void* key, void* value, unsigned int lenKey, unsigned int lenValue, uint64_t ttl_sec)
{
    if( m_head == nullptr )
    {
        m_head = new Node;
        m_head->m_key = key;
        m_head->m_value = value;
        m_head->m_lenKey = lenKey;
        m_head->m_lenValue = lenValue;
        m_head->m_ttl_sec = ttl_sec;
        m_head->m_next = nullptr;

        m_queuePUB->push(m_name, key, lenKey, UPDATED_PUB);

        return UPDATED_PUB;
    }
    else
    {
        Node* node = m_head;
        Node* nodePrev = m_head;
        bool collision = false;

        while( node != nullptr && !collision )
        {
            for(unsigned int i = 0; i < lenKey; i++)
            {
                if( ((char*)node->m_key)[i] != ((char*)key)[i] || node->m_lenKey != lenKey )
                {
                    collision = false;
                    break;
                }
                collision = true;
            }

            if( !collision )
            {
                nodePrev = node;
                node = node->m_next;
            }
        }

        if( collision )
        {
            free(node->m_value);
            node->m_value = value;
            node->m_lenValue = lenValue;
            node->m_ttl_sec = ttl_sec;

            m_queuePUB->push(m_name, key, lenKey, UPDATED_PUB);

            return UPDATED_PUB;
        }
        else if( node == nullptr )
        {
            nodePrev->m_next = new Node;
            nodePrev->m_next->m_key = key;
            nodePrev->m_next->m_value = value;
            nodePrev->m_next->m_lenKey = lenKey;
            nodePrev->m_next->m_lenValue = lenValue;
            nodePrev->m_next->m_ttl_sec = ttl_sec;
            nodePrev->m_next->m_next = nullptr;

            m_queuePUB->push(m_name, key, lenKey, UPDATED_PUB);

            return UPDATED_PUB;
        }
    }
    return ITEM_CREATE_ERROR;
}

uint8_t linkedList::remove(void* key, unsigned int lenKey)
{
    if( m_head == nullptr )
        return ITEM_NOT_EXIST;
    else
    {
        Node* node = m_head;
        Node* nodePrev = nullptr;
        bool collision = false;

        while( node != nullptr && !collision )
        {
            for(unsigned int i = 0; i < lenKey; i++)
            {
                if( ((char*)node->m_key)[i] != ((char*)key)[i] || node->m_lenKey != lenKey )
                {
                    collision = false;
                    break;
                }
                collision = true;
            }

            if( !collision )
            {
                nodePrev = node;
                node = node->m_next;
            }
        }

        if( collision )
        {
            if( nodePrev != nullptr )
            {
                nodePrev->m_next = node->m_next;
                delete node;
                return DELETED_PUB;
            }
            else if( node != nullptr )
            {
                if( node->m_next == nullptr )
                {
                    m_head = nullptr;
                    delete node;

                    m_queuePUB->push(m_name, key, lenKey, DELETED_PUB);

                    return DELETED_PUB;
                }
            }
        }
        else
        {
            return ITEM_NOT_EXIST;
        }
    }
    return ITEM_REMOVE_ERROR;
}

Node* linkedList::find(void* key, unsigned int lenKey)
{
    if( m_head == nullptr )
        return nullptr;
    else
    {
        Node* node = m_head;
        bool collision = false;

        while( node != nullptr && !collision )
        {
            for(unsigned int i = 0; i < lenKey; i++)
            {
                if( ((char*)node->m_key)[i] != ((char*)key)[i] || node->m_lenKey != lenKey )
                {
                    collision = false;
                    break;
                }
                collision = true;
            }

            if( !collision )
            {
                node = node->m_next;
            }
        }

        if( collision )
        {
            return node;
        }
    }
    return nullptr;
}
