#include "hashTable.hpp"

hashTable::hashTable()
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

uint8_t hashTable::insert(void* key, void* value, unsigned int lenKey, unsigned int lenValue, uint64_t ttl_sec)
{
    unsigned int index = hashTable::hash(key, lenKey);

    if( m_table[index] == nullptr )
    {
        m_table[index] = new linkedList;
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
