#include "server_service_responser.h"

#include "../proto/connect_server.pb.h"
#include "../proto/ServicePacket.pb.h"

#include "service_package_responser.h"

using namespace meet_you;
using namespace net_service;

namespace push_logic
{

server_service_responser::server_service_responser()
{
    //ctor
}

server_service_responser::~server_service_responser()
{
    //dtor
}

int server_service_responser::response(std::string const& id, meet_you::head const& h, std::string const& data)
{
    message_format mf;
    meet_you::head hh = h;
    hh.cmd++;
    mf.encode_header(hh, data);
    mf.body_combine();

    fix_dispatch_to_server(id, mf);

    return 0;
}

service_response_result server_service_responser::parser(std::string const& data, std::string & response)
{
    service_response_result res;

    ServiceRequest sr;
    ServiceResponse serr;

    //std::cout << data << ", size:" << data.size() << std::endl;

    if( sr.ParseFromString( data ) )
    {
        int service_type = sr.service_type();

        std::stringstream ss;
        std::stringstream ss_service_type;
        ss_service_type << "service_type:" << sr.service_type() ;
        std::stringstream ss_sn;
        ss_sn << "sn:" << sr.sn();
        ss << RFORMATE("service receve session:" + sr.sessid()) << RFORMATE(ss_service_type.str()) << RFORMATE(ss_sn.str());
        PUSHLOG_DEBUG(ss.str());

        if(service_type == 600)//server
        {
            if(sr.has_payload())
            {
                service_package_responser task(sr.sn());
                std::string package_response;

                res = task.parser(sr.payload(), package_response);

                if(res.status() ==0 && res.sync())
                {
                    serr.set_sn(sr.sn());
                    serr.set_sessid(sr.sessid());
                    serr.set_service_type(sr.service_type());

                    if(res.status() == 0)
                    {
                        serr.set_status(0);
                    }
                    else
                    {
                        serr.set_status(-1);
                    }

                    serr.set_payload(package_response);

                    serr.SerializeToString(&response);
                }
            }
            else
            {
                PUSHLOG_WARN("ServiceRequest has not payload!");
            }
        }
    }
    else
    {
        PUSHLOG_WARN("ServiceRequest parse error data");
    }

    return res;
}

}
