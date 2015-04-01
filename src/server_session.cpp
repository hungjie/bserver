#include "server_session.h"

#include <sstream>

#include "logic_deal_processing.h"

#include "peer_handle_map.h"

#include "log.h"

namespace push_logic
{

server_session::server_session(boost::asio::io_service& io_service)
    : session_base(io_service)
{

}

server_session::~server_session()
{

}

int server_session::open()
{
    std::stringstream ss;
    ss << "client:" << this->socket().remote_endpoint().address().to_string()
       << ":" << this->socket().remote_endpoint().port();

    this->id_ = ss.str();

    this->read_msg_.remote_id(this->id_);

    __PEER_MAP::get_mutable_instance().set(this->id_, shared_from_this());

    return 0;
}

int server_session::do_head()
{
    meet_you::head h;
    int res = this->read_msg_.decode_header(h);

    _SERVER_LOGIC_PROCESSING::get_const_instance().after_header_get(h, this->read_msg_);

    return res;
}

int server_session::do_body()
{
    /*
        std::string str;
        str.assign(this->read_msg_.body().data(), this->read_msg_.body().data() + this->read_msg_.body_length());
        std::cout << str << " , " << str.size() << std::endl;
    */
    _SERVER_LOGIC_PROCESSING::get_const_instance().after_body_get(this->read_msg_);

    return 0;
}

}
