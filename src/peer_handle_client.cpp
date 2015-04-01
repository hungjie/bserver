#include <sstream>

#include "peer_handle_client.h"

#include "peer_handle_map.h"

#include "../proto/connect_server.pb.h"

#include "log.h"

using namespace boost::asio;

using namespace meet_you;

namespace push_logic
{

peer_handle_client::peer_handle_client(boost::asio::io_service& io_service,
                                       ip::tcp::resolver::iterator endpoint_iterator)
    :session_(new client_session(io_service))
{
    boost::asio::ip::tcp::endpoint endpoint;
    endpoint = *endpoint_iterator;
    std::stringstream ss;
    ss << "start connect address : " << endpoint.address();

    PUSHLOG_TRACE(RFORMATE(ss.str()));

    boost::system::error_code error;
    boost::asio::connect(session_->socket(), endpoint_iterator, error);

    this->conected_ = this->session_->handle_connect(error);
}

peer_handle_client::peer_handle_client(boost::asio::io_service& io_service
                                       , boost::asio::ip::tcp::resolver::iterator endpoint_iterator
                                       , std::string const& server_id
                                       , std::string const& fix_id)
    :session_(new client_session(io_service))
{
    boost::asio::ip::tcp::endpoint endpoint;
    endpoint = *endpoint_iterator;
    std::stringstream ss;
    ss << RFORMATE("start connect fix address : " + server_id) << RFORMATE("id:" + fix_id);

    PUSHLOG_TRACE(ss.str());

    boost::system::error_code error;
    boost::asio::connect(session_->socket(), endpoint_iterator, error);

    this->conected_ = this->session_->handle_connect_fix_id(error, server_id, fix_id);
}

peer_handle_client::~peer_handle_client()
{
}

void peer_handle_client::registe()
{
    if(conected_ != 0)
    {
        PUSHLOG_WARN("can not start registe the connection is unreach!");
        return;
    }

    SvRegRequest srr;
    srr.set_service_type(600);

    std::string out;

    srr.SerializeToString(&out);

    meet_you::head hh;
    hh.cmd = 300;
    hh.len = sizeof(hh) + out.size();
    hh.magic = 0;

    msg_.encode_header(hh, out);
    msg_.body_combine();

    this->session_->post_write(msg_);
}

void peer_handle_client::write(message_format const& msg)
{
    this->session_->post_write(msg);
}

}
