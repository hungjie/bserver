#ifndef SESSION_BASE_H
#define SESSION_BASE_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "message_format.h"

namespace push_logic
{

class session_base : public boost::enable_shared_from_this<session_base>
{
public:
    session_base(boost::asio::io_service& io_service);
    virtual ~session_base();

    virtual int write(const message_format& msg);
    virtual void start();
    virtual int close();

    message_format & get_message_format()
    {
        return this->read_msg_;
    }

    const message_format & get_message_format() const
    {
        return this->read_msg_;
    }

    boost::asio::ip::tcp::socket& socket()
    {
        return this->socket_;
    }

    void post_write(const message_format& msg)
    {
        io_service_.post(boost::bind(&session_base::write, shared_from_this(), msg));
    }

    virtual int handle_connect(boost::system::error_code const& error);

protected:
    virtual void handle_read_header(boost::system::error_code const& error);
    virtual void handle_read_body(boost::system::error_code const& error);
    virtual void handle_write(boost::system::error_code const& error);

    virtual int on_close();

    virtual int do_head() = 0;
    virtual int do_body() = 0;

    virtual int open();

    void read_msg_flush()
    {
        //this->read_msg_.flush();
    }

    message_format read_msg_;
    std::deque<message_format> write_msgs_;

    boost::asio::ip::tcp::socket socket_;

    std::string id_;

private:
    boost::mutex mutex_;
    boost::mutex mutex_inter_;

    bool inter_guard_lock()
    {
        //boost::mutex::scoped_lock lock(this->mutex_inter_);
        if(!this->inter_lock_)
        {
            this->inter_lock_ = true;
            return this->inter_lock_;
        }

        return false;
    }

    void inter_guard_release()
    {
        //boost::mutex::scoped_lock lock(this->mutex_inter_);
        this->inter_lock_ = false;
    }

    bool is_inter_guard_locked()
    {
        //boost::mutex::scoped_lock lock(this->mutex_inter_);
        return this->inter_lock_;
    }

    bool connected_;
    boost::asio::io_service& io_service_;
    boost::asio::strand strand_;

    bool inter_lock_;
};

}

#endif // SESSION_BASE_H
