/*
*
* ZMQ server.
*
*/
#ifndef ZMQSERVERBD_HPP
#define ZMQSERVERBD_HPP

#include <zmq.h>

#include "baseData.hpp"
#include "queuePUB.hpp"

class zmqServerBD
{
private:
	void* m_context;
	void* m_rep;
	void* m_pub;

    uint8_t m_request;
    uint8_t m_reply;
    char* m_nameTable;
    void* m_key;
    void* m_value;
    unsigned int m_lenKey;
    unsigned int m_lenValue;
    uint64_t m_ttl_sec;

	baseData m_baseData;
	queuePUB m_queuePUB;

	bool errRep(uint8_t reason);
public:
    zmqServerBD();
    ~zmqServerBD();
    void run();
};

#endif // ZMQSERVERBD_HPP
