#ifndef SESSION_TASK_H
#define SESSION_TASK_H

#include "task_base.h"

#include "configwrap.h"

#include "logic_deal_processing.h"

#include "mongo_client.h"

namespace push_logic
{

class session_task : public task_base
{
public:
    //typedef struct sigaction sigactionclass;
    session_task();
    virtual ~session_task();

    void get_session(std::string const& cid, std::map<std::string, meet_you::Sess> & sesss)
    {
        sess_cache_->get_session(cid, sesss);
    }

    std::string find_near(double lat, double long_, double max_distance, int start, int limit, std::string const& params, std::string& error)
    {
        if(!mongo_client_cache_.get())
            return "{\"error\":\"connetion error\"}";

        return mongo_client_cache_->find_near("service", "group_talk", lat, long_, max_distance, start, limit, params, error);
    }

    std::string find_by_city(std::string const& match, int start, int limit, std::string const& params, std::string& error)
    {
        if(!mongo_client_cache_.get())
            return "{\"error\":\"connetion error\"}";

        std::string city = "{\"city\":\"" + match + "\"}";
        return mongo_client_cache_->find_by_condition("service", "group_talk", city, start, limit, params, error);
    }

    void set_work_process(logic_deal_processing* work_process)
    {
        work_process_ = work_process;
    }

protected:
    void do_work(std::string const& id, meet_you::head const& h, const std::string & data);
    void init_cache_map();

    void full_queue_event(std::string & data, meet_you::head & h);

private:
    boost::thread_specific_ptr<meet_you::SessCache> sess_cache_;
    boost::thread_specific_ptr<meet_you::mongo_db::group_mongo_client> mongo_client_cache_;
    logic_deal_processing* work_process_;
};

typedef boost::serialization::singleton<session_task> __SESSION_TASK;

}

#endif // SESSION_TASK_H
