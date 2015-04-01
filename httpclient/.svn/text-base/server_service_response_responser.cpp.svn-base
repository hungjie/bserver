#include "server_service_response_responser.h"

#include "service_request_json_data.h"

#include "../proto/ServicePacket.pb.h"

#include "uuid/uuid.h"

using namespace meet_you;
using namespace net_service;

namespace push_logic
{

server_service_response_responser::server_service_response_responser()
{
    //ctor
}

server_service_response_responser::~server_service_response_responser()
{
    //dtor
}

int server_service_response_responser::response(std::string const& id, meet_you::head const& h, std::string const& data)
{
    return 0;
}

service_response_result server_service_response_responser::parser(std::string const& data, std::string & response)
{
    service_response_result res;
    res.async(true);

    service_request_json_data json_data(data);

    std::string cid = json_data.cid();
    std::string service_id = json_data.service_id();
    std::string resposne_data = json_data.data();

    if(cid.empty() || service_id.empty() || resposne_data.empty())
    {
        PUSHLOG_DEBUG("cid or service_id or resposne_data empty!");
        return res;
    }

    ServiceResponse srp;
    std::string sn;

    sn.resize(36);
    uuid_t uuid;

    uuid_generate(uuid);
    uuid_unparse(uuid, const_cast<char*>( sn.data() ));

    srp.set_sn(sn);
    srp.set_service_type(600);

    ServicePacket packet_response;
    packet_response.set_type(26);
    PublicServiceResponse* psr = packet_response.mutable_public_service_response();

    psr->set_cid(cid);
    psr->set_service_id(service_id);

    psr->set_data(resposne_data);

    std::string packet_res;
    packet_response.SerializeToString(&packet_res);

    srp.set_payload(packet_res);

    std::map<std::string, meet_you::Sess> sesss;
    std::map<std::string, meet_you::Sess>::iterator it;

    __SESSION_TASK::get_mutable_instance().get_session(cid, sesss);

    if(sesss.size() == 0)
    {
        PUSHLOG_WARN("json push msgmissing get session empty" + RFORMATE( "cid:" + cid) + RFORMATE("sn:" + sn));
    }

    srp.set_sn(sn);

    for(it = sesss.begin(); it != sesss.end(); ++it)
    {
        srp.set_sessid(it->first);

        std::string out;
        srp.SerializeToString(&out);

        message_format mf;

        meet_you::head hh;
        hh.cmd = 6667;
        hh.len = sizeof(hh) + out.size();
        hh.magic = 0;

        mf.encode_header(hh, out);
        mf.body_combine();

        std::string server_id = "server:" + it->second.server_addr;

        std::stringstream s_ss;
        std::stringstream msg_ss;
        s_ss << RFORMATE("start send to server:" + server_id) << RFORMATE("sn:" + sn) << RFORMATE(msg_ss.str()) << RFORMATE("cid:" + cid);
        PUSHLOG_INFO(s_ss.str());

        this->dispatch_to_server(server_id, mf);
    }

    return res;
}

}
