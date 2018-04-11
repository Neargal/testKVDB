/*
*
* List with node(key, value and ttl).
*
*/
#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include "node.hpp"

class linkedList
{
private:
    Node* m_head;
public:
    linkedList();
    ~linkedList();
    uint8_t insert(void* key, void* value, unsigned int lenKey, unsigned int lenValue, uint64_t ttl_sec);
    uint8_t remove(void* key, unsigned int lenKey);
    Node*     find(void* key, unsigned int lenKey);
};

#endif // LINKEDLIST_HPP
