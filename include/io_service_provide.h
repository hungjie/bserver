#ifndef IO_SERVICE_PROVIDE_H_INCLUDED
#define IO_SERVICE_PROVIDE_H_INCLUDED

#include <vector>

#include <boost/serialization/singleton.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "session_task.h"
#include "configwrap.h"

namespace push_logic
{
class io_service_provide : public boost::noncopyable
{
public:
    io_service_provide()
        : next_io_service_(0)
    {
        std::size_t pool_size = __CONFIGWRAP::get_const_instance().io_pool_size();

        if(pool_size == 0)
        {
            //throw std::runtime_error("io_service_pool size is 0");
            pool_size = boost::thread::hardware_concurrency();
        }

        //pool_size = 100;

        std::cout << "io_service pool size:" << pool_size << std::endl;

        for (std::size_t i = 0; i < pool_size; ++i)
        {
            io_service_ptr io_service(new boost::asio::io_service);
            work_ptr work(new boost::asio::io_service::work(*io_service));
            io_services_.push_back(io_service);
            work_.push_back(work);
        }
    }

    ~io_service_provide() {}

    void run()
    {
        for (std::size_t i = 0; i < io_services_.size(); ++i)
        {
            /*
            boost::shared_ptr<boost::thread> thread(new boost::thread(
                    boost::bind(&boost::asio::io_service::run, io_services_[i])));
            threads.push_back(thread);
            */
            //for(int cc = 0; cc < 4; ++cc)
            io_service_cc_.create_thread(boost::bind(&boost::asio::io_service::run, io_services_[i]));
        }

        /*
        for (std::size_t i = 0; i < threads.size(); ++i)
            threads[i]->join();
        */
    }

    /// Stop all io_service objects in the pool.
    void stop()
    {
        for (std::size_t i = 0; i < io_services_.size(); ++i)
            io_services_[i]->stop();

        __SESSION_TASK::get_mutable_instance().stop();
        __SESSION_TASK::get_mutable_instance().notifiy_all();

        task_.interrupt_all();

        task_.join_all();

        io_service_cc_.join_all();
    }

    /// Get an io_service to use.
    boost::asio::io_service& get_io_service()
    {
        boost::asio::io_service& io_service = *io_services_[next_io_service_];

        ++next_io_service_;
        if (next_io_service_ == io_services_.size())
            next_io_service_ = 0;

        return io_service;
    }

    boost::thread_group & work()
    {
        return this->task_;
    }

private:
    typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
    typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;
    //std::vector<boost::shared_ptr<boost::thread> > threads;

    /// The pool of io_services.
    std::vector<io_service_ptr> io_services_;

    /// The work that keeps the io_services running.
    std::vector<work_ptr> work_;

    /// The next io_service to use for a connection.
    std::size_t next_io_service_;

    boost::thread_group io_service_cc_;
    boost::thread_group task_;
};

typedef boost::serialization::singleton<io_service_provide> __IO_SERVICE_PROVIDE;

}

#endif // IO_SERVICE_PROVIDE_H_INCLUDED
