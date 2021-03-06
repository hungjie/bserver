#ifndef PEER_HANDLE_SERVER_H
#define PEER_HANDLE_SERVER_H

#include "peer_base.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "server_session.h"

namespace push_logic
{

class peer_handle_server : peer_base
{
public:
    explicit peer_handle_server(const boost::asio::ip::tcp::endpoint& endpoint);
    virtual ~peer_handle_server();

    void run();
    void stop();
    void join();

protected:
    void start_accept();
    void handle_accept(server_session_ptr session,
                       const boost::system::error_code& error);

private:
    typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;

    std::vector<boost::shared_ptr<boost::thread> > threads_;

    size_t thread_pool_size_;

    work_ptr work_;
    //boost::asio::signal_set signals_;
};

}

#endif // PEER_HANDLE_SERVER_H
