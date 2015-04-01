#include "session_base.h"

#include "peer_handle_map.h"

#include "log.h"

namespace push_logic
{

session_base::session_base(boost::asio::io_service& io_service)
    : socket_(io_service)
    , connected_(false)
    , io_service_(io_service)
    , strand_(io_service_)
    , inter_lock_(false)
{
}

session_base::~session_base()
{
}

void session_base::start()
{
    int status = this->open();

    if(status == -1)
    {
        this->close();
        return;
    }

    this->connected_ = true;

    boost::asio::async_read(socket_,
                            boost::asio::buffer(this->read_msg_.head().data(), message_format::message_head_length),
                            strand_.wrap(boost::bind(
                                    &session_base::handle_read_header, shared_from_this(),
                                    boost::asio::placeholders::error)));
}

int session_base::open()
{
    return 0;
}

int session_base::handle_connect(boost::system::error_code const& error)
{
    if(!error)
    {
        this->start();
    }
    else
    {
        NETLOG( error.message() + "<" + id_ + ">" );

        this->close();

        return -1;
    }

    return 0;
}

int session_base::close()
{
    boost::system::error_code error;
    this->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);

    this->on_close();

    return 0;
}

int session_base::on_close()
{
    __PEER_MAP::get_mutable_instance().erase(this->id_);
    return 0;
}

int session_base::write(const message_format& msg)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    this->write_msgs_.push_back(msg);

    std::stringstream ss;

    if (!is_inter_guard_locked())
    {
        if(!inter_guard_lock())
        {
            NETLOG( "already locked" );
            return 0;
        }

        //ss << "start write, connected " << "msg:" << msg.length() << "left:" << this->write_msgs_.size() << std::endl;
        //PUSHLOG_TRACE( ss.str() );

        boost::asio::async_write(socket_,
                                 boost::asio::buffer(this->write_msgs_.front().body().data(),
                                         this->write_msgs_.front().length()),
                                 boost::bind(&session_base::handle_write, shared_from_this(),
                                             boost::asio::placeholders::error));
    }

    return 0;
}

void session_base::handle_write(boost::system::error_code const& error)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    if(!error)
    {
        this->write_msgs_.pop_front();
        if(!this->write_msgs_.empty())
        {
            std::stringstream ss;
            ss << "start write, connected " << "msg:" << this->write_msgs_.front().length() << "left:" << this->write_msgs_.size();

            NETLOG( ss.str() );

            boost::asio::async_write(socket_,
                                     boost::asio::buffer(this->write_msgs_.front().body().data(),
                                             this->write_msgs_.front().length()),
                                     boost::bind(&session_base::handle_write, shared_from_this(),
                                                 boost::asio::placeholders::error));
        }
        else
        {
            inter_guard_release();
        }
    }
    else
    {
        this->close();
        NETLOG( error.message() + "<" + id_ + ">" );
    }
}

void session_base::handle_read_header(boost::system::error_code const& error)
{
    if(!error)
    {
        int logic_status = this->do_head();

        if(logic_status == 0)
        {
            if(read_msg_.body_length() == 0)
            {
                this->handle_read_body(error);
                return;
            }

            boost::asio::async_read(this->socket_,
                                    boost::asio::buffer(read_msg_.body().data(), read_msg_.body_length()),
                                    strand_.wrap(boost::bind(&session_base::handle_read_body, shared_from_this(),
                                                 boost::asio::placeholders::error)));
        }
        else if(logic_status == -1)
        {
            this->close();
            NETLOG( "decode header error!" );
        }
    }
    else
    {
        this->close();
        NETLOG( error.message() + "<" + id_ + ">");
    }

}

void session_base::handle_read_body(boost::system::error_code const& error)
{
    if(!error)
    {
        int logic_status = this->do_body();

        this->read_msg_flush();

        if(logic_status == 0)
        {
            boost::asio::async_read(this->socket_,
                                    boost::asio::buffer(this->read_msg_.head().data(), message_format::message_head_length),
                                    strand_.wrap(boost::bind(&session_base::handle_read_header,
                                                 shared_from_this(),
                                                 boost::asio::placeholders::error)));

            return;
        }
    }
    else
    {
        this->close();
        NETLOG( error.message() + "<" + id_ + ">" );
    }

}

}
