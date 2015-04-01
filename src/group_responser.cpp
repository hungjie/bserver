#include "group_responser.h"

#include <vector>
#include <map>

#include <exception>

#include "mongo_client.h"

#include "session_task.h"

using namespace net_service;
using namespace meet_you::mongo_db;

namespace push_logic
{

group_responser::group_responser(std::string const& sn)
    : sn_(sn)
{
    //ctor
}

group_responser::~group_responser()
{
    //dtor
}

int group_responser::response(std::string const& id, meet_you::head const& h, std::string const& data)
{
    return 0;
}

bool group_responser::parser_group_near_request(ServicePacket& packet_response, ServicePacket & packet, std::string & error)
{
    packet_response.set_type(28);

    GroupLBSNearRequest* groupLBSNearReq = packet.mutable_group_lbs_near_request();
    double lat = groupLBSNearReq->lat();
    double long_ = groupLBSNearReq->long_();

    std::string cid = groupLBSNearReq->cid();

    int start = groupLBSNearReq->start();

    int limit = groupLBSNearReq->limit();
    int max_distance = groupLBSNearReq->max_distance();

    std::string json = __SESSION_TASK::get_mutable_instance().find_near(lat, long_, max_distance, start, limit, "", error);

    GroupLBSNearResponse* groupLBSNearResp = packet_response.mutable_group_lbs_near_response();
    groupLBSNearResp->set_cid(cid);

    if(!error.empty())
    {
        groupLBSNearResp->set_status(-1);
        groupLBSNearResp->set_data(error);

        return false;
    }

    groupLBSNearResp->set_status(0);
    groupLBSNearResp->set_data(json);

    return true;
}

bool group_responser::parser_group_city_request(ServicePacket& packet_response, ServicePacket & packet, std::string & error)
{
    packet_response.set_type(30);

    GroupCityRequest* groupCityRequest = packet.mutable_group_city_request();
    int start = groupCityRequest->start();
    int limit = groupCityRequest->limit();
    std::string city = groupCityRequest->city();

    std::string cid = groupCityRequest->cid();

    std::string json = __SESSION_TASK::get_mutable_instance().find_by_city(city, start, limit, "", error);

    GroupCityResponse* groupCityResponse = packet_response.mutable_group_city_response();
    groupCityResponse->set_cid(cid);

    if(!error.empty())
    {
        groupCityResponse->set_status(-1);
        groupCityResponse->set_data(error);

        return false;
    }

    groupCityResponse->set_status(0);
    groupCityResponse->set_data(json);

    return true;
}

service_response_result group_responser::parser(std::string const& data, std::string & response)
{
    service_response_result res;

    ServicePacket packet;
    ServicePacket packet_response;

    bool status = packet.ParseFromString(data);

    if(!status)
    {
        PUSHLOG_WARN("server service package parser faild.<sn:"+sn_+">");
        res.status(-1);
        return res;
    }

    if(packet.type() != 27 || packet.type() != 29)
    {
        PUSHLOG_WARN("server service package type invalid.<sn:"+sn_+">");
        res.status(-1);
        return res;
    }

    std::string error;

    if(packet.type() == 27)
    {
        status = parser_group_near_request(packet_response, packet, error);
    }
    else if(packet.type() == 29)
    {
        status = parser_group_city_request(packet_response, packet, error);
    }

    if(!status)
    {
        res.status(-1);
        PUSHLOG_INFO("mongodb response <data: " + error + "><sn:" + sn_ + ">");
    }
/*
    python_call_base python_call;

    std::vector<double> addrs;
    addrs.push_back(lat);
    addrs.push_back(long_);

    std::string python_root = __CONFIGWRAP::get_const_instance().python_root();
*/
    packet_response.SerializeToString(&response);

    return res;
}

}
