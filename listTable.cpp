#include "listTable.hpp"

listTable::listTable() : m_head(nullptr), m_queuePUB(nullptr) {}

listTable::~listTable()
{
    nodeTable* node = m_head;

    while( m_head != nullptr )
    {
        m_head = node->m_next;
        delete node;
        node = m_head;
    }
}

bool listTable::setQueuePUB(queuePUB* queuePUB)
{
    m_queuePUB = queuePUB;
}

uint8_t listTable::createTable(char* nameTable)
{
    if( m_head == nullptr )
    {
        m_head = new nodeTable;
        m_head->m_name = nameTable;
        m_head->m_next = nullptr;
        m_head->m_table = new hashTable;
        m_head->m_table->setQueuePUB(m_queuePUB);
        return OK;
    }
    else
    {
        nodeTable* node = m_head;
        nodeTable* nodePrev = m_head;
        bool collision = false;

        while( node != nullptr && !collision )
        {
            if( strcmp(node->m_name, nameTable) != 0 )
            {
                nodePrev = node;
                node = node->m_next;
                collision = false;
            }
            else
            {
                collision = true;
            }
        }

        if( collision )
        {
            return TABLE_ALREADY_EXIST;
        }
        else if( node == nullptr )
        {
            nodePrev->m_next = new nodeTable;
            nodePrev->m_next->m_name = nameTable;
            nodePrev->m_next->m_next = nullptr;
            nodePrev->m_next->m_table = new hashTable;
            nodePrev->m_table->setQueuePUB(m_queuePUB);
            return OK;
        }
    }
    return TABLE_CREATE_ERROR;
}

uint8_t listTable::removeTable(char* nameTable)
{
    if( m_head == nullptr )
    {
        return TABLE_NOT_EXIST;
    }
    else
    {
        nodeTable* node = m_head;
        nodeTable* nodePrev = nullptr;
        bool collision = false;

        while( node != nullptr && !collision )
        {
            if( strcmp(node->m_name, nameTable) != 0 )
            {
                nodePrev = node;
                node = node->m_next;
                collision = false;
            }
            else
            {
                collision = true;
            }
        }

        if( collision )
        {
            if( nodePrev != nullptr )
            {
                nodePrev->m_next = node->m_next;
                delete node;
                return OK;
            }
            else if( node != nullptr )
            {
                if( node->m_next == nullptr )
                {
                    m_head = nullptr;
                    delete node;
                    return OK;
                }
            }
        }
        else
        {
            return TABLE_NOT_EXIST;
        }
    }
    return TABLE_CREATE_ERROR;
}

uint8_t listTable::insert(char* nameTable, void* key, void* value, unsigned int lenKey, unsigned int lenValue, uint64_t ttl_sec)
{
    if( m_head == nullptr )
    {
        return TABLE_NOT_EXIST;
    }
    else
    {
        nodeTable* node = m_head;
        bool collision = false;

        while( node != nullptr && !collision )
        {
            if( strcmp(node->m_name, nameTable) != 0 )
            {
                node = node->m_next;
                collision = false;
            }
            else
            {
                collision = true;
            }
        }

        if( collision )
        {
            return node->m_table->insert(nameTable, key, value, lenKey, lenValue, ttl_sec);
        }
        else
        {
            return TABLE_NOT_EXIST;
        }
    }
    return TABLE_INSERT_ERROR;
}

uint8_t listTable::remove(char* nameTable, void* key, unsigned int lenKey)
{
    if( m_head == nullptr )
        return TABLE_NOT_EXIST;
    else
    {
        nodeTable* node = m_head;
        bool collision = false;

        while( node != nullptr && !collision )
        {
            if( strcmp(node->m_name, nameTable) != 0 )
            {
                node = node->m_next;
                collision = false;
            }
            else
            {
                collision = true;
            }
        }

        if( collision )
        {
            if( node != nullptr )
            {
                return node->m_table->remove(key, lenKey);
            }
        }
        else
        {
            return TABLE_NOT_EXIST;
        }
    }
    return TABLE_REMOVE_ERROR;
}

Node* listTable::find(char* nameTable, void* key, unsigned int lenKey)
{
    if( m_head == nullptr )
        return nullptr;
    else
    {
        nodeTable* node = m_head;
        bool collision = false;

        while( node != nullptr && !collision )
        {
            if( strcmp(node->m_name, nameTable) != 0 )
            {
                node = node->m_next;
                collision = false;
            }
            else
            {
                collision = true;
            }
        }

        if( collision )
        {
            if( node != nullptr )
            {
                return node->m_table->find(key, lenKey);
            }
        }
    }
    return nullptr;
}
