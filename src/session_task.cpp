#include "session_task.h"

#include <exception>

#include <boost/thread.hpp>
#include <boost/make_shared.hpp>

#include "peer_handle_map.h"

namespace push_logic
{

session_task::session_task()
{
    work_process_ = 0;
}

session_task::~session_task()
{

}

void redis_log(std::string const& log)
{
    //std::cout << log << std::endl;
    REDISLOG(log);
}

void session_task::init_cache_map()
{
    meet_you::SessCache* sess_cache = new meet_you::SessCache();
    if(sess_cache->init(&__CONFIGWRAP::get_mutable_instance().zk(), __CONFIGWRAP::get_const_instance().session_path(), __CONFIGWRAP::get_const_instance().session_path_bak()) != 0)
    {
        PUSHLOG_ERROR("zookeeper session init error");
    }
    else
        sess_cache_.reset(sess_cache);

    std::vector<std::pair<std::string, int> > hosts = __CONFIGWRAP::get_const_instance().mongo_hosts();
    if(hosts.size() != 0)
    {
        meet_you::mongo_db::group_mongo_client* mongo_client_cache = new meet_you::mongo_db::group_mongo_client(hosts);

        std::string error;
        bool connected = mongo_client_cache->connect("service", "service", "3UaQifyTY3w&**#U", error);

        if(!connected)
        {
            PUSHLOG_ERROR("mongo hosts connect error!!!");
            return;
        }

        mongo_client_cache_.reset(mongo_client_cache);
    }
    else
    {
        PUSHLOG_ERROR("mongo hosts empty!!!");
    }
}

void session_task::do_work(std::string const& id, meet_you::head const& h, const std::string & data)
{
    if(work_process_)
    {
        work_process_->do_work(id, h, data);
    }
}

void session_task::full_queue_event(std::string & data, meet_you::head & h)
{
    h.cmd = 1010;
}

}
