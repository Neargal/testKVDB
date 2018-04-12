#ifndef QUEUEPUB_HPP
#define QUEUEPUB_HPP

#include "nodePUB.hpp"

class queuePUB
{
private:
    nodePUB* m_head;
    nodePUB* m_tail;
    void* m_pub;

    uint8_t sendPub(char* nameTable, void* key, unsigned int lenKey, uint8_t reason);
public:
    queuePUB();
    ~queuePUB();
    bool setSockPUB(void* pub);
    uint8_t push(char* nameTable, void* key, unsigned int lenKey, uint8_t reason);
    uint8_t pop();
};

#endif // QUEUEPUB_HPP
