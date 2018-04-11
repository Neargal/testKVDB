/*
*
* List with node(table name, key and ttl).
*
*/
#ifndef LISTSORTEDTTL_HPP
#define LISTSORTEDTTL_HPP

#include "nodeTTL.hpp"

class listSortedTTL
{
private:
    nodeTTL* m_head;
public:
    listSortedTTL();
    ~listSortedTTL();
    nodeTTL* top();
    bool push(char* nameTable, void* key, unsigned int lenKey, uint64_t ttl_sec);
    bool pop();
};

#endif // LISTSORTEDTTL_HPP
