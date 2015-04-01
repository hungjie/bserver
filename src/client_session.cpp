#include "client_session.h"

#include <sstream>

#include "logic_deal_processing.h"

#include "peer_handle_map.h"

#include "peer_handle_client.h"

#include "io_service_provide.h"
#include "log.h"

using namespace boost::asio::ip;

namespace push_logic
{

client_session::client_session(boost::asio::io_service& io_service, bool is_auto_reconnect)
    : session_base(io_service)
    , fix_connect_(false)
    , is_auto_reconnect_(is_auto_reconnect)
{
}

client_session::~client_session()
{
    //__PEER_MAP_CLIENT::get_mutable_instance().erase(this->server_id_, this->id_);
}

int client_session::on_close()
{
    std::stringstream stemp;
    stemp << "<" << "erase:" + this->id_ << ">";
    NETLOG( stemp.str() );
    __PEER_MAP_CLIENT::get_mutable_instance().erase(this->server_id_, this->id_);

    if(is_auto_reconnect_ && !this->server_id_.empty() && !this->id_.empty())
    {
        std::vector<std::string> split;
        boost::algorithm::split (split, server_id_, boost::algorithm::is_any_of (":"));

        if(split.size() != 3)
        {
            NETLOG(RFORMATE("error reconnection info:" + server_id_));
            return 0;
        }

        NETLOG(RFORMATE("start reconnect to server:" + server_id_));

        boost::asio::io_service& io = __IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service();
        tcp::resolver resolver(io);
        tcp::resolver::query query(split[1].c_str(), split[2].c_str());
        tcp::resolver::iterator iterator = resolver.resolve(query);

        boost::shared_ptr<peer_handle_client> c(new peer_handle_client(io, iterator, server_id_, id_));
        c->registe();
    }

    return 0;
}

int client_session::handle_connect_fix_id(boost::system::error_code const& error
        , std::string const& server_id
        , std::string const& remote_id)
{
    this->fix_connect_ = true;

    this->id_ = remote_id;
    this->server_id_ = server_id;

    this->read_msg_.remote_id(this->id_);
    this->read_msg_.server_id(this->server_id_);

    NETLOG(RFORMATE("start save session:" + this->id_));

    return this->handle_connect(error);
}

int client_session::open()
{
    if(this->fix_connect_)
    {
        int status = __PEER_MAP_CLIENT::get_mutable_instance().fix_set(this->server_id_, this->id_, shared_from_this());
        return status;
    }

    std::stringstream ss;
    ss << "server:" << this->socket().remote_endpoint().address().to_string()
       << ":" << this->socket().remote_endpoint().port();

    this->server_id_ = ss.str();

    this->id_ = __PEER_MAP_CLIENT::get_mutable_instance().set(this->server_id_, shared_from_this());

    if(this->id_.empty())
    {
        NETLOG(RFORMATE("empty map id, max client for server_id:" + this->server_id_));
        return -1;
    }

    NETLOG(this->id_);

    this->read_msg_.remote_id(this->id_);
    this->read_msg_.server_id(this->server_id_);

    return 0;
}

int client_session::do_head()
{
    meet_you::head h;
    int res = this->read_msg_.decode_header(h);

    _CLIENT_LOGIC_PROCESSING::get_const_instance().after_header_get(h, this->read_msg_);

    return res;
}

int client_session::do_body()
{
    _CLIENT_LOGIC_PROCESSING::get_const_instance().after_body_get(this->read_msg_);

    return 0;
}

}
