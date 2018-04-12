#include "hashTable.hpp"

hashTable::hashTable(): m_queuePUB(nullptr)
{
    for(int i = 0; i < TABLE_SIZE; i++)
    {
        m_table[i] = nullptr;
    }
}

hashTable::~hashTable()
{
    for(int i = 0; i < TABLE_SIZE; i++)
    {
        if( m_table[i] != nullptr )
        {
            delete m_table[i];
        }
    }
}

bool hashTable::setQueuePUB(queuePUB* queuePUB)
{
    m_queuePUB = queuePUB;
    return true;
}

unsigned int hashTable::hash(void* key, unsigned int lenKey)
{
    unsigned int index = 0;

    for(unsigned int i = 0; i < lenKey; i++)
    {
        index += ((unsigned char*)key)[i];
    }

    index %= TABLE_SIZE;

    return index;
}

uint8_t hashTable::insert(char* nameTable, void* key, void* value, unsigned int lenKey, unsigned int lenValue, uint64_t ttl_sec)
{
    unsigned int index = hashTable::hash(key, lenKey);

    if( m_table[index] == nullptr )
    {
        m_table[index] = new linkedList;
        m_table[index]->setQueuePUB(m_queuePUB);
        m_table[index]->setNameTable(nameTable);
    }

    return m_table[index]->insert(key, value, lenKey, lenValue, ttl_sec);
}

uint8_t hashTable::remove(void* key, unsigned int lenKey)
{
    unsigned int index = hashTable::hash(key, lenKey);

    if( m_table[index] == nullptr )
    {
        return ITEM_NOT_EXIST;
    }

    return m_table[index]->remove(key, lenKey);
}

Node* hashTable::find(void* key, unsigned int lenKey)
{
    unsigned int index = hashTable::hash(key, lenKey);

    if( m_table[index] == nullptr )
    {
        return nullptr;
    }

    return m_table[index]->find(key, lenKey);
}
