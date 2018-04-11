/*
*
* ZMQ server.
*
*/
#ifndef ZMQSERVERBD_HPP
#define ZMQSERVERBD_HPP

#include <zmq.h>

#include "baseData.hpp"

class zmqServerBD
{
private:
	void* m_context;
	void* m_rep;
	void* m_pub;
    zmq_msg_t m_message;

    uint8_t m_request;
    uint8_t m_reply;
    char* m_nameTable;
    void* m_key;
    void* m_value;
    unsigned int m_lenKey;
    unsigned int m_lenValue;
    uint64_t m_ttl_sec;

	baseData m_baseData;

	bool errRep(uint8_t reason);
	bool sendPub(uint8_t reason);
public:
    zmqServerBD();
    ~zmqServerBD();
    void run();
};

#endif // ZMQSERVERBD_HPP
