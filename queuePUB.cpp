#include "queuePUB.hpp"


queuePUB::queuePUB() : m_head(nullptr), m_tail(nullptr), m_pub(nullptr) {}

queuePUB::~queuePUB()
{
    nodePUB* node = m_head;

    m_tail = nullptr;

    while( m_head != nullptr )
    {
        m_head = node->m_next;
        delete node;
        node = m_head;
    }
}

bool queuePUB::setSockPUB(void* pub)
{
    m_pub = pub;
    return true;
}

uint8_t queuePUB::sendPub(char* nameTable, void* key, unsigned int lenKey, uint8_t reason)
{
    zmq_msg_t message;

    zmq_msg_init_size(&message, strlen(nameTable));
    memcpy(zmq_msg_data(&message), nameTable, strlen(nameTable));
    zmq_msg_send(&message, m_pub, ZMQ_SNDMORE);
    zmq_msg_close(&message);

    zmq_msg_init_size(&message, 1);
    memcpy(zmq_msg_data(&message), &reason, 1);
    zmq_msg_send(&message, m_pub, ZMQ_SNDMORE);
    zmq_msg_close(&message);

    zmq_msg_init_size(&message, lenKey);
    memcpy(zmq_msg_data(&message), key, lenKey);
    zmq_msg_send(&message, m_pub, 0);
    zmq_msg_close(&message);

    return OK;
}

uint8_t queuePUB::push(char* nameTable, void* key, unsigned int lenKey, uint8_t reason)
{
    if( m_head == nullptr )
    {
        m_head = new nodePUB;
        m_head->m_next = nullptr;
        m_head->m_nameTable = (char*)malloc(strlen(nameTable));
        memcpy(m_head->m_nameTable, nameTable, strlen(nameTable));
        m_head->m_key = (void*)malloc(lenKey);
        memcpy(m_head->m_key, key, lenKey);
        m_head->m_lenKey = lenKey;
        m_head->m_reason = reason;
        m_tail = m_head;
        return OK;
    }

    if( m_tail != nullptr )
    {
        nodePUB* node = new nodePUB;
        node->m_next = nullptr;
        node->m_nameTable = (char*)malloc(strlen(nameTable));
        memcpy(node->m_nameTable, nameTable, strlen(nameTable));
        node->m_key = (void*)malloc(lenKey);
        memcpy(node->m_key, key, lenKey);
        node->m_lenKey = lenKey;
        node->m_reason = reason;
        m_tail->m_next = node;
        m_tail = node;
        return OK;
    }

    return ERROR_REP;
}

uint8_t queuePUB::pop()
{
    if( m_head == nullptr )
    {
        m_tail = nullptr;
        return PUB_UPDATE_DONE;
    }
    else
    {
        sendPub(m_head->m_nameTable, m_head->m_key, m_head->m_lenKey, m_head->m_reason);

        nodePUB* node = m_head;
        m_head = node->m_next;
        delete node;
        return OK;
    }

    return ERROR_REP;
}
