#include "zmqServerBD.hpp"

zmqServerBD::zmqServerBD() : m_more(0), m_more_size(sizeof(m_more)), m_request(0), m_reply(0), m_nameTable(nullptr), m_key(nullptr), m_value(nullptr), m_lenKey(0), m_lenValue(0), m_ttl_sec(0)
{
    m_context = zmq_ctx_new();

	m_rep = zmq_socket(m_context, ZMQ_REP);
	m_pub = zmq_socket(m_context, ZMQ_PUB);

	zmq_bind(m_rep, SOCK_REP_EP);
	zmq_bind(m_pub, SOCK_PUB_EP);

	m_queuePUB.setSockPUB(m_pub);
	m_baseData.setQueuePUB(&m_queuePUB);
}

zmqServerBD::~zmqServerBD()
{
	zmq_close(m_rep);
	zmq_close(m_pub);
	zmq_ctx_destroy(m_context);
}

void zmqServerBD::freeN()
{
    free(m_nameTable);
    m_nameTable = nullptr;
}

void zmqServerBD::freeNK()
{
    free(m_nameTable);
    m_nameTable = nullptr;
    free(m_key);
    m_key = nullptr;
}

void zmqServerBD::freeNKV()
{
    free(m_nameTable);
    m_nameTable = nullptr;
    free(m_key);
    m_key = nullptr;
    free(m_value);
    m_value = nullptr;
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

bool zmqServerBD::okRep()
{
    zmq_msg_t message;

    uint8_t reply = OK_REP;
    zmq_msg_init_size(&message, 1);
    memcpy(zmq_msg_data(&message), &m_reply, 1);
    zmq_msg_send(&message, m_rep, 0);
    zmq_msg_close(&message);

    return true;
}

bool zmqServerBD::reqTableName()
{
    zmq_msg_t message;

    zmq_msg_init(&message);
    int size = zmq_msg_recv(&message, m_rep, 0);
    if( size + 1 >= MAX_SIZE_TABLE )
    {
        errRep(TOO_BIG_ARG);
        return false;
    }
    m_nameTable = (char*)malloc(size + 1);
    memcpy(m_nameTable, zmq_msg_data(&message), size);
    m_nameTable[size] = 0;
    zmq_getsockopt(m_rep, ZMQ_RCVMORE, &m_more, &m_more_size);
    zmq_msg_close(&message);

    return true;
}

bool zmqServerBD::reqKey()
{
    zmq_msg_t message;

    zmq_msg_init(&message);
    m_lenKey = zmq_msg_recv(&message, m_rep, 0);
    if( m_lenKey > MAX_SIZE_KEY )
    {
        errRep(TOO_BIG_ARG);
        return false;
    }
    m_key = (void*)malloc(m_lenKey);
    memcpy(m_key, zmq_msg_data(&message), m_lenKey);
    zmq_getsockopt(m_rep, ZMQ_RCVMORE, &m_more, &m_more_size);
    zmq_msg_close(&message);

    return true;
}

bool zmqServerBD::reqValue()
{
    zmq_msg_t message;

    zmq_msg_init(&message);
    m_lenValue = zmq_msg_recv(&message, m_rep, 0);
    if( m_lenValue > MAX_SIZE_VALUE )
    {
        errRep(TOO_BIG_ARG);
        return false;
    }
    m_value = (void*)malloc(m_lenValue);
    memcpy(m_value, zmq_msg_data(&message), m_lenValue);
    zmq_getsockopt(m_rep, ZMQ_RCVMORE, &m_more, &m_more_size);
    zmq_msg_close(&message);

    return true;
}

bool zmqServerBD::reqCreateTable()
{
    zmq_msg_t message;

    if( m_more )
    {
        if( !reqTableName() )
        {
            return false;
        }
    }
    else
    {
        errRep(NOT_ENOUGH_ARG);
        return false;
    }
    if( !m_more )
    {
        m_reply = m_baseData.createTable(m_nameTable);
        if( m_reply == OK )
        {
            okRep();
            //printf("Create table done: [%s]\n", m_nameTable);
            return true;
        }
        else
        {
            errRep(m_reply);
            freeN();
            return false;
        }
    }
    else
    {
        errRep(TOO_MUCH_ARG);
        freeN();
        return false;
    }

    return false;
}

bool zmqServerBD::reqDeleteTable()
{
    zmq_msg_t message;

    if( m_more )
    {
        if( !reqTableName() )
        {
            return false;
        }
    }
    else
    {
        errRep(NOT_ENOUGH_ARG);
        return false;
    }
    if( !m_more )
    {
        m_reply = m_baseData.removeTable(m_nameTable);

        if( m_reply == OK )
        {
            okRep();
            //printf("Delete table done: [%s]\n", m_nameTable);
            freeN();
            return true;
        }
        else
        {
            errRep(m_reply);
            freeN();
            return false;
        }
    }
    else
    {
        errRep(TOO_MUCH_ARG);
        freeN();
        return false;
    }

    return false;
}

bool zmqServerBD::reqUpdate()
{
    zmq_msg_t message;

    if( m_more )
    {
        if( !reqTableName() )
        {
            return false;
        }
    }
    else
    {
        errRep(NOT_ENOUGH_ARG);
        return false;
    }
    if( m_more )
    {
        if( !reqKey() )
        {
            freeN();
            return false;
        }
    }
    else
    {
        errRep(NOT_ENOUGH_ARG);
        freeN();
        return false;
    }
    if( m_more )
    {
        if( !reqValue() )
        {

            freeNK();
            return false;
        }
    }
    if( m_more )
    {
        zmq_msg_init(&message);
        int size = zmq_msg_recv(&message, m_rep, 0);
        if( size != MAX_SIZE_TTL )
        {
            errRep(TOO_BIG_ARG);
            return false;
        }
        memcpy(&m_ttl_sec, zmq_msg_data(&message), size);
        zmq_getsockopt(m_rep, ZMQ_RCVMORE, &m_more, &m_more_size);
        zmq_msg_close(&message);
    }
    else
    {
        m_ttl_sec = 0;
    }
    if( !m_more )
    {
        m_reply = m_baseData.insert(m_nameTable, m_key, m_value, m_lenKey, m_lenValue, m_ttl_sec);

        if( m_reply == UPDATED_PUB )
        {
            okRep();
            //printf("Update key done in [%s]\n", m_nameTable);
            freeN();
            return true;
        }
        else
        {
            errRep(m_reply);
            freeNKV();
            return false;
        }
    }
    else
    {
        errRep(TOO_MUCH_ARG);
        freeNKV();
        return false;
    }

    return false;
}

bool zmqServerBD::reqDelete()
{
    zmq_msg_t message;

    if( m_more )
    {
        if( !reqTableName() )
        {
            return false;
        }
    }
    else
    {
        errRep(NOT_ENOUGH_ARG);
        return false;
    }
    if( m_more )
    {
        if( !reqKey() )
        {
            freeN();
            return false;
        }
    }
    else
    {
        errRep(NOT_ENOUGH_ARG);
        freeN();
        return false;
    }
    if( !m_more )
    {
        m_reply = m_baseData.remove(m_nameTable, m_key, m_lenKey);

        if( m_reply == DELETED_PUB )
        {
            okRep();
            //printf("Delete key done in [%s]\n", m_nameTable);
            freeNK();
            return true;
        }
        else
        {
            errRep(m_reply);
            freeNK();
            return false;
        }
    }
    else
    {
        errRep(TOO_MUCH_ARG);
        freeNK();
        return false;
    }

    return false;
}

bool zmqServerBD::reqGet()
{
    zmq_msg_t message;

    if( m_more )
    {
        reqTableName();
    }
    else
    {
        errRep(NOT_ENOUGH_ARG);
        freeN();
        return false;
    }
    if( m_more )
    {
        if( !reqKey() )
        {
            freeN();
            return false;
        }
    }
    else
    {
        errRep(NOT_ENOUGH_ARG);
        freeN();
        return false;
    }
    if( !m_more )
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

            freeNK();
            return true;
        }
        else
        {
            errRep(m_reply);
            freeNK();
            return false;
        }
    }
    else
    {
        errRep(TOO_MUCH_ARG);
        freeNK();
        return false;
    }

    return false;
}

void zmqServerBD::run()
{
    zmq_pollitem_t m_items[2] = {
    { m_rep, 0, ZMQ_POLLIN, 0 },
    { m_pub, 0, ZMQ_POLLOUT, 0 }
    };

    while(1)
	{
        zmq_poll(m_items, 2, 0);

        if( m_items[0].revents & ZMQ_POLLIN )
        {
            while(1)
            {
                zmq_msg_t message;
                zmq_msg_init(&message);
                int size = zmq_msg_recv(&message, m_rep, 0);
                if( MAX_SIZE_REQ == size )
                {
                    memcpy(&m_request, zmq_msg_data(&message), size);
                }
                else
                {
                    errRep(TOO_BIG_ARG);
                    m_more = 0;
                    break;
                }
                zmq_getsockopt(m_rep, ZMQ_RCVMORE, &m_more, &m_more_size);
                zmq_msg_close(&message);

                switch(m_request)
                {
                    case CREATE_TABLE_REQ:
                    {
                        reqCreateTable();
                        m_more = 0;
                        break;
                    }
                    case DELETE_TABLE_REQ:
                    {
                        reqDeleteTable();
                        m_more = 0;
                        break;
                    }
                    case UPDATE_REQ:
                    {
                        reqUpdate();
                        m_more = 0;
                        break;
                    }
                    case DELETE_REQ:
                    {
                        reqDelete();
                        m_more = 0;
                        break;
                    }
                    case GET_REQ:
                    {
                        reqGet();
                        m_more = 0;
                        break;
                    }
                    default:
                    {
                        m_more = 0;
                        break;
                    }
                }

                if( !m_more )
                    break;
            }
        }
        if( m_items[1].revents & ZMQ_POLLOUT )
        {
            while( m_queuePUB.pop() != PUB_UPDATE_DONE ) {}
        }
        while( m_baseData.updateTTL() != TTL_UPDATE_DONE ) {}
    }
}
