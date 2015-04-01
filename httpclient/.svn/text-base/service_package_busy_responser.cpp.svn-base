#include "service_package_busy_responser.h"

#include "../proto/ServicePacket.pb.h"

#include "configwrap.h"

using namespace meet_you;
//using namespace Httpclient;
using namespace net_service;

namespace push_logic
{

service_package_busy_responser::service_package_busy_responser()
{
    //ctor
}

service_package_busy_responser::~service_package_busy_responser()
{
    //dtor
}

int service_package_busy_responser::response(std::string const& id, meet_you::head const& h, std::string const& data)
{
    return 0;
}

service_response_result service_package_busy_responser::parser(std::string const& data, std::string & response)
{
    service_response_result res;

    ServicePacket packet;
    ServicePacket packet_response;

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

    if(packet.type() == 21)
    {
        packet_response.set_type(22);

        ReplyRequest rr = packet.reply_request();
        str_url = __CONFIGWRAP::get_const_instance().reply_domain();

        std::string topic_id = rr.topic_id();
        std::string sign = rr.sign();
        std::string v = rr.v();
        std::string platform = rr.platform();
        std::string device_id = rr.device_id();

        ReplyResponse* rer = packet_response.mutable_reply_response();
        rer->set_cid(rr.cid());
        rer->set_type(rr.type());

        request_data = rr.data();

        str_url += "/" + topic_id + "/reviews?" + "sign="+ sign +"&v=" + v + "&platform=" + platform + "&device_id=" + device_id;
    }
    else if(packet.type() == 23)
    {
        packet_response.set_type(24);

        PostTopicRequest ptr = packet.post_topic_request();
        str_url = __CONFIGWRAP::get_const_instance().post_topic_domain();

        std::string forum_id = ptr.forum_id();
        std::string sign = ptr.sign();
        std::string v = ptr.v();
        std::string platform = ptr.platform();
        std::string device_id = ptr.device_id();

        PostTopicResponse* ptrs = packet_response.mutable_post_topic_response();
        ptrs->set_cid(ptr.cid());
        ptrs->set_type(ptr.type());

        request_data = ptr.data();

        str_url += "/" + forum_id + "/topics?" + "sign="+ sign +"&v=" + v + "&platform=" + platform + "&device_id=" + device_id;
    }

    PUSHLOG_DEBUG("busy for service request to http post info <url:"+str_url + ">" + "<data:"+request_data+">");

    std::string curl_response;
    curl_response = "{\"error\":\"system service in busy\", \"code\":1010}";

    if(packet_response.type() == 22)
    {
        ReplyResponse* rer = packet_response.mutable_reply_response();
        rer->set_data(curl_response);
    }
    else if(packet_response.type() == 24)
    {
        PostTopicResponse* ptrs = packet_response.mutable_post_topic_response();
        ptrs->set_data(curl_response);
    }

    packet_response.SerializeToString(&response);

    return res;
}

}
