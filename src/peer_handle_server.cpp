#include "peer_handle_server.h"

#include "log.h"

#include "io_service_provide.h"

using namespace boost::asio::ip;

namespace push_logic
{

peer_handle_server::peer_handle_server(const tcp::endpoint& endpoint)
    : acceptor_(io_service_)
    , thread_pool_size_(4)
    , work_(new boost::asio::io_service::work(io_service_))
    //, signals_(io_service_)
{
    //*
    acceptor_.open(endpoint.protocol());

    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    //boost::asio::socket_base::enable_connection_aborted option(true);
    //acceptor_.set_option(option);

    acceptor_.non_blocking(true);

    acceptor_.bind(endpoint);

    boost::system::error_code error;
    int listen_size = 10240;
    acceptor_.listen(listen_size, error);
    //*/

    std::cout << "listen size:" << listen_size << std::endl;
    std::cout << "server pool size:" <<thread_pool_size_ << std::endl;

    this->start_accept();
    /*
        signals_.add(SIGINT);
        signals_.add(SIGSEGV);
        signals_.add(SIGABRT);
        signals_.add(SIGPIPE);
        signals_.add(SIGFPE);
        //signals_.add(SIGTERM);
    #if defined(SIGQUIT)
        signals_.add(SIGQUIT);
    #endif // defined
        signals_.async_wait(boost::bind(&peer_handle_server::stop, this));
    */
}

peer_handle_server::~peer_handle_server()
{

}

void peer_handle_server::stop()
{
    /*
        std::stringstream ss;
        dump_stack(ss);
        PUSHLOG_DEBUG( ss.str() );
    */
    this->io_service_.stop();

    //log4cplus::Logger::getRoot().shutdown();

    __IO_SERVICE_PROVIDE::get_mutable_instance().stop();
}

void peer_handle_server::start_accept()
{
    server_session_ptr session_ptr(new server_session(__IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service()));
    acceptor_.async_accept(session_ptr->socket(),
                           boost::bind(&peer_handle_server::handle_accept, this, session_ptr,
                                       boost::asio::placeholders::error));
}

void peer_handle_server::handle_accept(server_session_ptr session,
                                       const boost::system::error_code& error)
{
    NETLOG("handle accept");

    if(!error)
    {
        session->start();
    }
    else
    {
        NETLOG(error.message());
    }

    this->start_accept();
}

void peer_handle_server::run()
{
    for (std::size_t i = 0; i < thread_pool_size_; ++i)
    {
        boost::shared_ptr<boost::thread> thread(new boost::thread(
                boost::bind(&boost::asio::io_service::run, &io_service_)));
        threads_.push_back(thread);
    }
}

void peer_handle_server::join()
{
    for (std::size_t i = 0; i < threads_.size(); ++i)
        threads_[i]->join();
}

}
