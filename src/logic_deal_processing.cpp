#include "logic_deal_processing.h"

#include <exception>

#include <boost/thread.hpp>
#include <boost/make_shared.hpp>

#include "session_task.h"

#include "log.h"

#include "task_responser.h"

#include "server_service_responser.h"
#include "service_package_responser.h"

#include "httpclient/server_service_response_responser.h"
#include "httpclient/service_package_busy_responser.h"

#include "embeded_python/python_func_service_responser.h"

#include "group_service_request_responser.h"

#include "group_responser.h"

using namespace net_service;

namespace push_logic
{

logic_deal_processing::logic_deal_processing()
{
    __SESSION_TASK::get_mutable_instance().set_work_process(this);
}

logic_deal_processing::~logic_deal_processing()
{
}

void logic_deal_processing::after_header_get(meet_you::head const& h, message_format const& message) const
{

}

void logic_deal_processing::after_body_get(message_format const& message) const
{
    if(message.meet_head().cmd == 1)
    {
        return ;
    }

    __SESSION_TASK::get_mutable_instance().push(message);
}

void logic_deal_processing::do_work(std::string const& id, meet_you::head const& h, const std::string & data) const
{
    std::stringstream ss;
    ss << "get task <client_id:" << id.c_str() << "><cmd:"<< h.cmd << "><magic:" << h.magic << "><len:"<< h.len << ">";
    PUSHLOG_INFO( ss.str());

    boost::shared_ptr<task_responser> responser;
    {

    }

    if(h.cmd == 200)
    {
        responser = boost::make_shared<server_service_responser>();
    }
    else if(h.cmd == 6666)
    {
        responser = boost::make_shared<server_service_response_responser>();
    }
    else if(h.cmd == 7818)
    {
        responser = boost::shared_ptr<task_responser>(new group_service_request_responser<>());
    }
    else if(h.cmd == 8000)
    {
        responser = boost::shared_ptr<task_responser>(new group_service_request_responser<python_func_service_responser>());
    }
    else if(h.cmd == 1010)
    {
        //responser = boost::make_shared<python_func_service_responser>();
        //*
        std::cout << "server busy !!!" << std::endl;
        responser = boost::make_shared<service_package_busy_responser>();
        //*/
    }
    else
    {
        return;
    }

    try
    {
        std::string r_data;
        service_response_result result = responser->parser(data, r_data);

        if(result.async())
        {
            return;
        }

        responser->response(id, h, r_data);
    }
    catch(std::exception const& e)
    {
        PUSHLOG_ERROR( e.what() );
    }
    catch(...)
    {
        PUSHLOG_ERROR( "unkonwn error dotask" );
    }
}

void server_logic_processing::after_body_get(message_format const& message) const
{
    logic_deal_processing::after_body_get(message);
}

void client_logic_processing::after_body_get(message_format const& message) const
{
    logic_deal_processing::after_body_get(message);
}

}
