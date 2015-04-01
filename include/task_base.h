#ifndef TASK_BASE_H
#define TASK_BASE_H

#include <deque>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/serialization/singleton.hpp>

#include "message_format.h"

#include "log.h"

namespace push_logic
{

class task_base
{
public:
    task_base();
    virtual ~task_base();

    void work();
    void push(message_format const& mf);

    void notifiy_all();
    void stop()
    {
        stop_ = true;
    }

protected:
    virtual void do_work(std::string const& id, meet_you::head const& h, const std::string & data) {};
    virtual void init_cache_map() {}
    virtual void full_queue_event(std::string & data, meet_you::head & h) {}

    bool try_pop(std::string & data, meet_you::head & h, std::string & id);
    void wait_and_pop(std::string & data, meet_you::head & h, std::string & id);

    std::deque<message_format> deque_;

private:
    boost::mutex mutex_;
    boost::condition_variable condition_;

    bool stop_;
};

}
#endif // TASK_BASE_H
