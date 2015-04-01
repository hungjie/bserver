#ifndef PEER_HANDLE_CLIENT_H
#define PEER_HANDLE_CLIENT_H

#include <deque>

#include "peer_base.h"
#include "message_format.h"

#include "client_session.h"


namespace push_logic
{

class client_session;

class peer_handle_client : public peer_base
{
public:
    peer_handle_client(boost::asio::io_service& io_service,
                       boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    peer_handle_client(boost::asio::io_service& io_service
                       , boost::asio::ip::tcp::resolver::iterator endpoint_iterator
                       , std::string const& remote_id
                       , std::string const& fix_id);

    virtual ~peer_handle_client();

    void registe();

    void write(message_format const& msg);

private:
    client_session_ptr session_;
    message_format msg_;

    int conected_;
};

}

#endif // PEER_HANDLE_CLIENT_H
