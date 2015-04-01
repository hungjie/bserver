#include "service_package_responser.h"

#include "curl_base.h"

#include "configwrap.h"

#include "service_request_json_data.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::gregorian;
using namespace boost::local_time;
using namespace boost::posix_time;

using namespace meet_you;
using namespace Httpclient;
using namespace net_service;

namespace push_logic
{

service_package_responser::service_package_responser(std::string const& sn)
    : sn_(sn)
{
    //ctor
}

service_package_responser::~service_package_responser()
{
    //dtor
}

int service_package_responser::response(std::string const& id, meet_you::head const& h, std::string const& data)
{
    return 0;
}

void service_package_responser::do_reply_request(ServicePacket& packet_response, ServicePacket& packet
    , std::string& str_url, std::string& authorization, std::string& request_data)
{
    packet_response.set_type(22);

    ReplyRequest rr = packet.reply_request();
    str_url = __CONFIGWRAP::get_const_instance().reply_domain();

    std::string topic_id = rr.topic_id();
    std::string sign = rr.sign();
    std::string v = rr.v();
    std::string platform = rr.platform();
    std::string device_id = rr.device_id();

    authorization = rr.authorization();

    ReplyResponse* rer = packet_response.mutable_reply_response();
    rer->set_cid(rr.cid());
    rer->set_type(rr.type());

    request_data = rr.data();

    str_url += "/" + topic_id + "/reviews?" + "sign="+ sign +"&v=" + v + "&platform=" + platform + "&device_id=" + device_id;

    PUSHLOG_DEBUG("service request to http post info <url:"+str_url + ">" + "<data:"+request_data+">");

    curl_base curl;
    curl.init_json_header();

    std::vector<std::string> headers;
    if(!authorization.empty())
        headers.push_back("Authorization:"+authorization);

    curl.add_header(headers);

    std::string curl_response;

    int post_status;

    if(request_data.empty())
    {
        post_status = curl.Get(str_url, curl_response);
    }
    else
    {
        post_status = curl.Post(str_url, request_data, curl_response);
    }

    if(post_status != 0)
    {
        //todo
        PUSHLOG_ERROR("curl post error <url:" + str_url + "><sn:" + sn_ + ">");
    }
    else
    {
        PUSHLOG_INFO("curl response <data: " + curl_response + "><sn:" + sn_ + ">");
    }

    //ReplyResponse* rer = packet_response.mutable_reply_response();
    rer->set_data(curl_response);
}

void service_package_responser::do_post_topic_request(ServicePacket& packet_response, ServicePacket& packet
    , std::string& str_url, std::string& authorization, std::string& request_data)
{
    packet_response.set_type(24);

    PostTopicRequest ptr = packet.post_topic_request();
    str_url = __CONFIGWRAP::get_const_instance().post_topic_domain();

    std::string forum_id = ptr.forum_id();
    std::string sign = ptr.sign();
    std::string v = ptr.v();
    std::string platform = ptr.platform();
    std::string device_id = ptr.device_id();

    authorization = ptr.authorization();

    PostTopicResponse* ptrs = packet_response.mutable_post_topic_response();
    ptrs->set_cid(ptr.cid());
    ptrs->set_type(ptr.type());

    request_data = ptr.data();

    str_url += "/" + forum_id + "/topics?" + "sign="+ sign +"&v=" + v + "&platform=" + platform + "&device_id=" + device_id;

    PUSHLOG_DEBUG("service request to http post info <url:"+str_url + ">" + "<data:"+request_data+">");

    curl_base curl;
    curl.init_json_header();

    std::vector<std::string> headers;
    if(!authorization.empty())
        headers.push_back("Authorization:"+authorization);

    curl.add_header(headers);

    std::string curl_response;

    int post_status;

    if(request_data.empty())
    {
        post_status = curl.Get(str_url, curl_response);
    }
    else
    {
        post_status = curl.Post(str_url, request_data, curl_response);
    }

    if(post_status != 0)
    {
        //todo
        PUSHLOG_ERROR("curl post error <url:" + str_url + "><sn:" + sn_ + ">");
    }
    else
    {
        PUSHLOG_INFO("curl response <data: " + curl_response + "><sn:" + sn_ + ">");
    }

    //PostTopicResponse* ptrs = packet_response.mutable_post_topic_response();
    ptrs->set_data(curl_response);

}

void service_package_responser::do_public_service_request(ServicePacket& packet_response, ServicePacket& packet
    , std::string& str_url, std::string& authorization, std::string& request_data)
{
    packet_response.set_type(26);

    PublicServiceRequest* psr = packet.mutable_public_service_request();

    PublicServiceResponse* psrr = packet_response.mutable_public_service_response();
    psrr->set_cid(psr->cid());
    psrr->set_service_id(psr->service_id());

    //time_t time_now = time(NULL);
    //int64 time64 = time_now * 1000;
    ptime pc4(microsec_clock::universal_time());
    ptime time_t_begin(date(1970,1,1));
    time_duration timestamp = pc4-time_t_begin;

    psrr->set_time(timestamp.total_milliseconds());

    service_request_json_data json_data;
    request_data = json_data.to_service_request(psr->cid(), psr->service_id(), psr->data());

    str_url = __CONFIGWRAP::get_const_instance().robot_domain();

    PUSHLOG_DEBUG("service request to http post info <url:"+str_url + ">" + "<data:"+request_data+">");

    curl_base curl;
    curl.init_json_header();

    std::vector<std::string> headers;
    if(!authorization.empty())
        headers.push_back("Authorization:"+authorization);

    curl.add_header(headers);

    std::string curl_response;

    int post_status;

    if(request_data.empty())
    {
        post_status = curl.Get(str_url, curl_response);
    }
    else
    {
        post_status = curl.Post(str_url, request_data, curl_response);
    }

    if(post_status != 0)
    {
        //todo
        PUSHLOG_ERROR("curl post error <url:" + str_url + "><sn:" + sn_ + ">");
    }
    else
    {
        PUSHLOG_INFO("curl response <data: " + curl_response + "><sn:" + sn_ + ">");
    }

    //PublicServiceResponse* psr = packet_response.mutable_public_service_response();
    psrr->set_data(curl_response);
}

bool service_package_responser::parser_group_near_request(ServicePacket& packet_response, ServicePacket & packet, std::string & error)
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

    std::cout << json << std::endl;
    PUSHLOG_DEBUG("find near <data:" + json + ">");

    groupLBSNearResp->set_status(0);
    groupLBSNearResp->set_data(json);

    return true;
}

bool service_package_responser::parser_group_city_request(ServicePacket& packet_response, ServicePacket & packet, std::string & error)
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

    std::cout << json << std::endl;
    PUSHLOG_DEBUG("find city <data:" + json + ">");

    groupCityResponse->set_status(0);
    groupCityResponse->set_data(json);

    return true;
}


service_response_result service_package_responser::parser(std::string const& data, std::string & response)
{
    service_response_result res;

    ServicePacket packet;
    ServicePacket packet_response;

    /* Test mutiple curl invoke.
    curl_base curl_test;
    std::vector<std::string> headerstt;
    std::string testauth = "xxxtttxxx";
    headerstt.push_back("Authorization: "+testauth);
    headerstt.push_back("Content-Type: text/xml");
    headerstt.push_back("Myheadertest: txml");
    curl_test.add_header(headerstt);
    std::string test_curl_response;
    curl_test.Post("http://localhost/", "", test_curl_response);

    packet_response.set_type(24);
    PostTopicResponse* testpost = packet_response.mutable_post_topic_response();
    testpost->set_data(test_curl_response);

    std::cout << test_curl_response << std::endl;

    packet_response.SerializeToString(&response);

    res.async(true);

    return res;
    //*/

    bool status = packet.ParseFromString(data);

    if(!status)
    {
        PUSHLOG_WARN("push responser parser error, pushpacket faild");
        return res;
    }

    //http url
    std::string str_url;
    //http data;
    std::string request_data;

    std::string authorization;

    std::string error;

    if(packet.type() == 21)
    {
        do_reply_request(packet_response, packet
            , str_url, authorization, request_data);
    }
    else if(packet.type() == 23)
    {
        do_post_topic_request(packet_response, packet
            , str_url, authorization, request_data);
    }
    else if(packet.type() == 25)
    {
        do_public_service_request(packet_response, packet
            , str_url, authorization, request_data);
    }
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

    packet_response.SerializeToString(&response);

    //packet_response.DebugString()

    return res;
}

}
