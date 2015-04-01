#include "task_base.h"

#include "configwrap.h"

namespace push_logic
{

task_base::task_base()
    : stop_(false)
{
}

task_base::~task_base()
{
    //dtor
}

void task_base::work()
{
    this->init_cache_map();

    try
    {
        while(1 && !stop_)
        {
            boost::this_thread::interruption_point();

            std::string data;
            meet_you::head h;
            std::string id;

            if(!this->try_pop(data, h, id))
            {
                this->wait_and_pop(data, h, id);
            }

            this->do_work(id, h, data);
        }
    }
    catch(std::exception const& e)
    {
        PUSHLOG_ERROR( e.what() );
    }
    catch(...)
    {
        PUSHLOG_ERROR( "unkonwn error task" );
    }
}

void task_base::push(message_format const& mf)
{
    boost::mutex::scoped_lock lock(mutex_);
    this->deque_.push_back(mf);
    lock.unlock();
    condition_.notify_one();
}

void task_base::notifiy_all()
{
    condition_.notify_all();
}

bool task_base::try_pop(std::string & data, meet_you::head & h, std::string & id)
{
    boost::mutex::scoped_lock lock(mutex_);

    if(this->deque_.empty())
    {
        return false;
    }

    data.assign(this->deque_.front().body().data(), this->deque_.front().body().data() + this->deque_.front().body_length());
    h = this->deque_.front().meet_head();

    if(this->deque_.size() > __CONFIGWRAP::get_const_instance().task_queue_max())
    {
        full_queue_event(data, h);
    }

    id = this->deque_.front().remote_id();

    this->deque_.pop_front();

    return true;
}

void task_base::wait_and_pop(std::string & data, meet_you::head & h, std::string & id)
{
    boost::mutex::scoped_lock lock(mutex_);

    while(deque_.empty() && !stop_)
    {
        //boost::this_thread::interruption_point();
        condition_.wait(lock);
        //boost::this_thread::interruption_point();
    }

    if(stop_)
    {
        throw std::runtime_error("stop thread!");
    }

    data.assign(deque_.front().body().data(), deque_.front().body().data() + deque_.front().body_length());
    h = deque_.front().meet_head();
    id = deque_.front().remote_id();

    this->deque_.pop_front();
}

}
