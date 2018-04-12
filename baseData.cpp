#include "baseData.hpp"

baseData::baseData() : m_queuePUB(nullptr)
{
    for(int i = 0; i < TABLE_SIZE; i++)
    {
        m_table[i] = nullptr;
    }
    m_listTTL = new listSortedTTL;
}

baseData::~baseData()
{
    for(int i = 0; i < TABLE_SIZE; i++)
    {
        if( m_table[i] != nullptr )
        {
            delete m_table[i];
        }
    }
    delete m_listTTL;
}

unsigned int baseData::hash(char* nameTable)
{
    unsigned int index = 0;

    for(int i = 0; nameTable[i] != 0; i++)
    {
        index += (unsigned char)nameTable[i];
    }

    index %= TABLE_SIZE;

    return index;
}

bool baseData::setQueuePUB(queuePUB* queuePUB)
{
    m_queuePUB = queuePUB;
    return true;
}

uint8_t baseData::createTable(char* nameTable)
{
    unsigned int index = baseData::hash(nameTable);

    if( m_table[index] == nullptr )
    {
        m_table[index] = new listTable;
        m_table[index]->setQueuePUB(m_queuePUB);
    }

    return m_table[index]->createTable(nameTable);
}

uint8_t baseData::removeTable(char* nameTable)
{
    unsigned int index = baseData::hash(nameTable);

    if( m_table[index] == nullptr )
    {
        return TABLE_NOT_EXIST;
    }

    return m_table[index]->removeTable(nameTable);
}

uint8_t baseData::insert(char* nameTable, void* key, void* value, unsigned int lenKey, unsigned int lenValue, uint64_t ttl_sec)
{
    unsigned int index = baseData::hash(nameTable);

    if( m_table[index] == nullptr )
    {
        return TABLE_NOT_EXIST;
    }
    else
    {
        if( ttl_sec > 0 )
        {
            uint64_t calcTTL = time(0) + ttl_sec;

            if( m_table[index]->insert(nameTable, key, value, lenKey, lenValue, calcTTL) == UPDATED_PUB )
            {
                m_listTTL->push(nameTable, key, lenKey, calcTTL);
                return UPDATED_PUB;
            }
        }
        else
        {
            return m_table[index]->insert(nameTable, key, value, lenKey, lenValue, ttl_sec);
        }
    }
    return BD_INSERT_ERROR;
}

uint8_t baseData::remove(char* nameTable, void* key, unsigned int lenKey)
{
    unsigned int index = baseData::hash(nameTable);

    if( m_table[index] == nullptr )
    {
        return TABLE_NOT_EXIST;
    }

    return m_table[index]->remove(nameTable, key, lenKey);
}

Node* baseData::find(char* nameTable, void* key, unsigned int lenKey)
{
    unsigned int index = baseData::hash(nameTable);

    if( m_table[index] == nullptr )
    {
        return nullptr;
    }

    return m_table[index]->find(nameTable, key, lenKey);
}

uint8_t baseData::updateTTL()
{
    nodeTTL* nodeRemove = m_listTTL->top();

    if( nodeRemove != nullptr )
    {
        if( nodeRemove->m_ttl_sec <= time(0) )
        {
            Node* node = baseData::find(nodeRemove->m_nameTable, nodeRemove->m_key, nodeRemove->m_lenKey);

            if( node != nullptr )
            {
                if( nodeRemove->m_ttl_sec == node->m_ttl_sec )
                {
                    if( baseData::remove(nodeRemove->m_nameTable, nodeRemove->m_key, nodeRemove->m_lenKey) == DELETED_PUB )
                    {
                        m_listTTL->pop();
                        return DELETED_PUB;
                    }
                }
                m_listTTL->pop();
                return OK;
            }
            else
            {
                m_listTTL->pop();
                return OK;
            }
        }
        else
        {
            return TTL_UPDATE_DONE;
        }
    }
    else
    {
        return TTL_UPDATE_DONE;
    }
}
