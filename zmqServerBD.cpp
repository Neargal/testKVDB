#include "zmqServerBD.hpp"

zmqServerBD::zmqServerBD() : m_request(0), m_reply(0), m_nameTable(nullptr), m_key(nullptr), m_value(nullptr), m_lenKey(0), m_lenValue(0), m_ttl_sec(0)
{
    m_context = zmq_ctx_new();

	m_rep = zmq_socket(m_context, ZMQ_REP);
	m_pub = zmq_socket(m_context, ZMQ_PUB);

	zmq_bind(m_rep, "ipc:///kvdb.api.rep");
	zmq_bind(m_pub, "ipc:///kvdb.api.pub");
}

zmqServerBD::~zmqServerBD()
{
    if( m_nameTable != nullptr )
        free(m_nameTable);

    if( m_key != nullptr )
        free(m_key);

    if( m_value != nullptr )
        free(m_value);

	zmq_close(m_rep);
	zmq_close(m_pub);
	zmq_ctx_destroy(m_context);
}

bool zmqServerBD::errRep(uint8_t reason)
{
    zmq_msg_t message;

    uint8_t reply = ERROR_REP;
    zmq_msg_init_size(&message, 1);
    memcpy(zmq_msg_data(&message), &reply, 1);
    zmq_msg_send(&message, m_rep, ZMQ_SNDMORE);
    zmq_msg_close(&message);

    zmq_msg_init_size(&message, 1);
    memcpy(zmq_msg_data(&message), &reason, 1);
    zmq_msg_send(&message, m_rep, 0);
    zmq_msg_close(&message);

    return true;
}

bool zmqServerBD::sendPub(uint8_t reason)
{
    zmq_msg_t message;

    zmq_msg_init_size(&message, strlen(m_nameTable));
    memcpy(zmq_msg_data(&message), m_nameTable, strlen(m_nameTable));
    zmq_msg_send(&message, m_pub, ZMQ_SNDMORE);
    zmq_msg_close(&message);

    zmq_msg_init_size(&message, 1);
    memcpy(zmq_msg_data(&message), &reason, 1);
    zmq_msg_send(&message, m_pub, ZMQ_SNDMORE);
    zmq_msg_close(&message);

    zmq_msg_init_size(&message, m_lenKey);
    memcpy(zmq_msg_data(&message), m_key, m_lenKey);
    zmq_msg_send(&message, m_pub, 0);
    zmq_msg_close(&message);

    return true;
}

void zmqServerBD::run()
{
    zmq_pollitem_t m_items[2] = {
    { m_rep, 0, ZMQ_POLLIN, 0 },
    { m_pub, 0, ZMQ_POLLOUT, 0 }
    };

    while(1)
	{
        int more = 0;

        zmq_poll(m_items, 2, 0);

        if( m_items[0].revents & ZMQ_POLLIN )
        {
            while(1)
            {
                zmq_msg_t message;
                zmq_msg_init(&message);
                int size = zmq_msg_recv(&message, m_rep, 0);
                if( sizeof(uint8_t) == size )
                {
                    memcpy(&m_request, zmq_msg_data(&message), size);
                }
                else
                {
                    errRep(TOO_BIG_ARG);
                    more = 0;
                    break;
                }
                size_t more_size = sizeof(more);
                zmq_getsockopt(m_rep, ZMQ_RCVMORE, &more, &more_size);
                zmq_msg_close(&message);

                switch(m_request)
                {
                    case CREATE_TABLE_REQ:
                    {
                        if( more )
                        {
                            zmq_msg_init(&message);
                            int size = zmq_msg_recv(&message, m_rep, 0);
                            if( size + 1 >= 255 )
                            {
                                errRep(TOO_BIG_ARG);
                                more = 0;
                                break;
                            }
                            m_nameTable = (char*)malloc(size + 1);
                            memcpy(m_nameTable, zmq_msg_data(&message), size);
                            m_nameTable[size] = 0;
                            zmq_getsockopt(m_rep, ZMQ_RCVMORE, &more, &more_size);
                            zmq_msg_close(&message);
                        }
                        else
                        {
                            errRep(NOT_ENOUGH_ARG);
                            more = 0;
                            break;
                        }
                        if( !more )
                        {
                            m_reply = m_baseData.createTable(m_nameTable);

                            if( m_reply == OK )
                            {
                                m_reply = OK_REP;
                                zmq_msg_init_size(&message, 1);
                                memcpy(zmq_msg_data(&message), &m_reply, 1);
                                zmq_msg_send(&message, m_rep, 0);
                                zmq_msg_close(&message);
                                printf("Create table done: [%s]\n", m_nameTable);
                            }
                            else
                            {
                                errRep(m_reply);
                                free(m_nameTable);
                                m_nameTable = nullptr;
                            }
                        }
                        else
                        {
                            errRep(TOO_MUCH_ARG);
                            free(m_nameTable);
                            m_nameTable = nullptr;
                        }
                        more = 0;
                        break;
                    }
                    case DELETE_TABLE_REQ:
                    {
                        if( more )
                        {
                            zmq_msg_init(&message);
                            int size = zmq_msg_recv(&message, m_rep, 0);
                            if( size + 1 >= 255 )
                            {
                                errRep(TOO_BIG_ARG);
                                more = 0;
                                break;
                            }
                            m_nameTable = (char*)malloc(size + 1);
                            memcpy(m_nameTable, zmq_msg_data(&message), size);
                            m_nameTable[size] = 0;
                            zmq_getsockopt(m_rep, ZMQ_RCVMORE, &more, &more_size);
                            zmq_msg_close(&message);
                        }
                        else
                        {
                            errRep(NOT_ENOUGH_ARG);
                            more = 0;
                            break;
                        }
                        if( !more )
                        {
                            m_reply = m_baseData.removeTable(m_nameTable);

                            if( m_reply == OK )
                            {
                                m_reply = OK_REP;
                                zmq_msg_init_size(&message, 1);
                                memcpy(zmq_msg_data(&message), &m_reply, 1);
                                zmq_msg_send(&message, m_rep, 0);
                                zmq_msg_close(&message);
                                printf("Delete table done: [%s]\n", m_nameTable);
                            }
                            else
                            {
                                errRep(m_reply);
                                free(m_nameTable);
                                m_nameTable = nullptr;
                            }
                        }
                        else
                        {
                            errRep(TOO_MUCH_ARG);
                            free(m_nameTable);
                            m_nameTable = nullptr;
                        }
                        more = 0;
                        break;
                    }
                    case UPDATE_REQ:
                    {
                        if( more )
                        {
                            zmq_msg_init(&message);
                            int size = zmq_msg_recv(&message, m_rep, 0);
                            if( size + 1 >= 255 )
                            {
                                errRep(TOO_BIG_ARG);
                                more = 0;
                                break;
                            }
                            m_nameTable = (char*)malloc(size + 1);
                            memcpy(m_nameTable, zmq_msg_data(&message), size);
                            m_nameTable[size] = 0;
                            zmq_getsockopt(m_rep, ZMQ_RCVMORE, &more, &more_size);
                            zmq_msg_close(&message);
                        }
                        else
                        {
                            errRep(NOT_ENOUGH_ARG);
                            more = 0;
                            break;
                        }
                        if( more )
                        {
                            zmq_msg_init(&message);
                            m_lenKey = zmq_msg_recv(&message, m_rep, 0);
                            if( m_lenKey > sizeof(uint64_t) )
                            {
                                errRep(TOO_BIG_ARG);
                                free(m_nameTable);
                                m_nameTable = nullptr;
                                more = 0;
                                break;
                            }
                            m_key = (void*)malloc(m_lenKey);
                            memcpy(m_key, zmq_msg_data(&message), m_lenKey);
                            zmq_getsockopt(m_rep, ZMQ_RCVMORE, &more, &more_size);
                            zmq_msg_close(&message);
                        }
                        else
                        {
                            errRep(NOT_ENOUGH_ARG);
                            free(m_nameTable);
                            m_nameTable = nullptr;
                            more = 0;
                            break;
                        }
                        if( more )
                        {
                            zmq_msg_init(&message);
                            m_lenValue = zmq_msg_recv(&message, m_rep, 0);
                            if( m_lenValue > 1024 )
                            {
                                errRep(TOO_BIG_ARG);
                                free(m_nameTable);
                                m_nameTable = nullptr;
                                free(m_key);
                                m_key = nullptr;
                                more = 0;
                                break;
                            }
                            m_value = (void*)malloc(m_lenValue);
                            memcpy(m_value, zmq_msg_data(&message), m_lenValue);
                            zmq_getsockopt(m_rep, ZMQ_RCVMORE, &more, &more_size);
                            zmq_msg_close(&message);
                        }
                        if( more )
                        {
                            zmq_msg_init(&message);
                            int size = zmq_msg_recv(&message, m_rep, 0);
                            if( size > sizeof(uint64_t) )
                            {
                                errRep(TOO_BIG_ARG);
                                more = 0;
                                break;
                            }
                            memcpy(&m_ttl_sec, zmq_msg_data(&message), sizeof(uint64_t));
                            zmq_getsockopt(m_rep, ZMQ_RCVMORE, &more, &more_size);
                            zmq_msg_close(&message);
                        }
                        else
                        {
                            m_ttl_sec = 0;
                        }
                        if( !more )
                        {
                            m_reply = m_baseData.insert(m_nameTable, m_key, m_value, m_lenKey, m_lenValue, m_ttl_sec);

                            if( m_reply == UPDATED_PUB )
                            {
                                m_reply = OK_REP;
                                zmq_msg_init_size(&message, 1);
                                memcpy(zmq_msg_data(&message), &m_reply, 1);
                                zmq_msg_send(&message, m_rep, 0);
                                zmq_msg_close(&message);
                                printf("Update key done in [%s]\n", m_nameTable);

                                sendPub(UPDATED_PUB);
                            }
                            else
                            {
                                errRep(m_reply);
                                free(m_nameTable);
                                m_nameTable = nullptr;
                                free(m_key);
                                m_key = nullptr;
                                free(m_value);
                                m_value = nullptr;
                            }
                        }
                        else
                        {
                            errRep(TOO_MUCH_ARG);
                            free(m_nameTable);
                            m_nameTable = nullptr;
                            free(m_key);
                            m_key = nullptr;
                            free(m_value);
                            m_value = nullptr;
                        }
                        more = 0;
                        break;
                    }
                    case DELETE_REQ:
                    {
                        if( more )
                        {
                            zmq_msg_init(&message);
                            int size = zmq_msg_recv(&message, m_rep, 0);
                            if( size + 1 >= 255 )
                            {
                                errRep(TOO_BIG_ARG);
                                more = 0;
                                break;
                            }
                            m_nameTable = (char*)malloc(size + 1);
                            memcpy(m_nameTable, zmq_msg_data(&message), size);
                            m_nameTable[size] = 0;
                            zmq_getsockopt(m_rep, ZMQ_RCVMORE, &more, &more_size);
                            zmq_msg_close(&message);
                        }
                        else
                        {
                            errRep(NOT_ENOUGH_ARG);
                            more = 0;
                            break;
                        }
                        if( more )
                        {
                            zmq_msg_init(&message);
                            m_lenKey = zmq_msg_recv(&message, m_rep, 0);
                            if( m_lenKey > sizeof(uint64_t) )
                            {
                                errRep(TOO_BIG_ARG);
                                free(m_nameTable);
                                m_nameTable = nullptr;
                                more = 0;
                                break;
                            }
                            m_key = (void*)malloc(m_lenKey);
                            memcpy(m_key, zmq_msg_data(&message), m_lenKey);
                            zmq_getsockopt(m_rep, ZMQ_RCVMORE, &more, &more_size);
                            zmq_msg_close(&message);
                        }
                        else
                        {
                            errRep(NOT_ENOUGH_ARG);
                            free(m_nameTable);
                            m_nameTable = nullptr;
                            more = 0;
                            break;
                        }
                        if( !more )
                        {
                            m_reply = m_baseData.remove(m_nameTable, m_key, m_lenKey);

                            if( m_reply == DELETED_PUB )
                            {
                                m_reply = OK_REP;
                                zmq_msg_init_size(&message, 1);
                                memcpy(zmq_msg_data(&message), &m_reply, 1);
                                zmq_msg_send(&message, m_rep, 0);
                                zmq_msg_close(&message);
                                printf("Delete key done in [%s]\n", m_nameTable);

                                sendPub(DELETED_PUB);
                            }
                            else
                            {
                                errRep(m_reply);
                                free(m_nameTable);
                                m_nameTable = nullptr;
                                free(m_key);
                                m_key = nullptr;
                            }
                        }
                        else
                        {
                            errRep(TOO_MUCH_ARG);
                            free(m_nameTable);
                            m_nameTable = nullptr;
                            free(m_key);
                            m_key = nullptr;
                        }
                        more = 0;
                        break;
                    }
                    case GET_REQ:
                    {
                        if( more )
                        {
                            zmq_msg_init(&message);
                            int size = zmq_msg_recv(&message, m_rep, 0);
                            if( size + 1 >= 255 )
                            {
                                errRep(TOO_BIG_ARG);
                                more = 0;
                                break;
                            }
                            m_nameTable = (char*)malloc(size + 1);
                            memcpy(m_nameTable, zmq_msg_data(&message), size);
                            m_nameTable[size] = 0;
                            zmq_getsockopt(m_rep, ZMQ_RCVMORE, &more, &more_size);
                            zmq_msg_close(&message);
                        }
                        else
                        {
                            errRep(NOT_ENOUGH_ARG);
                            free(m_nameTable);
                            m_nameTable = nullptr;
                            more = 0;
                            break;
                        }
                        if( more )
                        {
                            zmq_msg_init(&message);
                            m_lenKey = zmq_msg_recv(&message, m_rep, 0);
                            if( m_lenKey > sizeof(uint64_t) )
                            {
                                errRep(TOO_BIG_ARG);
                                free(m_nameTable);
                                m_nameTable = nullptr;
                                more = 0;
                                break;
                            }
                            m_key = (void*)malloc(m_lenKey);
                            memcpy(m_key, zmq_msg_data(&message), m_lenKey);
                            zmq_getsockopt(m_rep, ZMQ_RCVMORE, &more, &more_size);
                            zmq_msg_close(&message);
                        }
                        else
                        {
                            errRep(NOT_ENOUGH_ARG);
                            free(m_nameTable);
                            m_nameTable = nullptr;
                            more = 0;
                            break;
                        }
                        if( !more )
                        {
                            Node* node = m_baseData.find(m_nameTable, m_key, m_lenKey);
                            if( node != nullptr )
                            {
                                m_reply = OK_REP;
                                zmq_msg_init_size(&message, 1);
                                memcpy(zmq_msg_data(&message), &m_reply, 1);
                                zmq_msg_send(&message, m_rep, ZMQ_SNDMORE);
                                zmq_msg_close(&message);

                                zmq_msg_init_size(&message, node->m_lenValue);
                                memcpy(zmq_msg_data(&message), node->m_value, node->m_lenValue);
                                zmq_msg_send(&message, m_rep, 0);
                                zmq_msg_close(&message);
                            }
                            else
                            {
                                errRep(m_reply);
                                free(m_nameTable);
                                m_nameTable = nullptr;
                                free(m_key);
                                m_key = nullptr;
                            }
                        }
                        else
                        {
                            errRep(TOO_MUCH_ARG);
                            free(m_nameTable);
                            m_nameTable = nullptr;
                            free(m_key);
                            m_key = nullptr;
                        }
                        more = 0;
                        break;
                    }
                    default:
                    {
                        more = 0;
                        break;
                    }
                }

                if( !more )
                    break; // Last message part
            }
        }
        if( m_items[1].revents & ZMQ_POLLOUT )
        {
            //NOTHING
        }
        while( m_baseData.updateTTL() != TTL_UPDATE_DONE ) {}
    }
}
