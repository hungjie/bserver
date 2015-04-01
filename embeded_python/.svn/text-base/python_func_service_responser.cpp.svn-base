#include "python_func_service_responser.h"

#include <vector>
#include <map>

#include <exception>

#include "python_call_base.h"

#include "proto/ServicePacket.pb.h"

using namespace embeded_python;
using namespace net_service;

namespace push_logic
{

python_func_service_responser::python_func_service_responser(std::string const& sn)
    : sn_(sn)
{
    //ctor
}

python_func_service_responser::~python_func_service_responser()
{
    //dtor
}

int python_func_service_responser::response(std::string const& id, meet_you::head const& h, std::string const& data)
{
    task_responser::response(id, h, data);
    return 0;
}

service_response_result python_func_service_responser::parser(std::string const& data, std::string & response)
{
    service_response_result res;

    ServicePacket packet;
    ServicePacket packet_response;

    bool status = packet.ParseFromString(data);

    if(!status)
    {
        PUSHLOG_WARN("server service package parser faild.<sn:" + sn_ + ">");
        return res;
    }

    if(packet.type() != 31)
    {
        PUSHLOG_WARN("server service package type invalid.<sn:" + sn_ + ">");
        return res;
    }

    packet_response.set_type(32);

    PythonCoreRequest* pythonCoreRequest = packet.mutable_python_core_request();

    std::string cid = pythonCoreRequest->cid();

    std::string json_request = pythonCoreRequest->json_request();

    std::string module_name = pythonCoreRequest->module_name();
    std::string function_name = pythonCoreRequest->function_name();

    python_call_base python_call;

    std::string python_root = __CONFIGWRAP::get_const_instance().python_root();

    PythonCoreResponse* pythonCoreResponse = packet_response.mutable_python_core_response();
    pythonCoreResponse->set_cid(cid);

    try{

        std::string json = python_call.call<std::string, std::string>(python_root, module_name, function_name, json_request);

        pythonCoreResponse->set_status(1);
        pythonCoreResponse->set_json_response(json);
    }
    catch(exception const& e)
    {
        std::string error = e.what();
        replace_all(error, "\r", "");
        replace_all(error, "\n", "");

        PUSHLOG_ERROR("python call throw <error:" + error + "><sn:" + sn_ + ">");

        pythonCoreResponse->set_status(0);

        pythonCoreResponse->set_json_response("{\"error\":\"service exception!\", \"code\":\"" + error + "\"}");
    }

    packet_response.SerializeToString(&response);

    return res;
}

}
