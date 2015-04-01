#ifndef GROUP_SERVICE_REQUEST_RESPONSER_H
#define GROUP_SERVICE_REQUEST_RESPONSER_H

#include "task_responser.h"

#include "group_responser.h"

#include "../proto/connect_server.pb.h"
#include "../proto/ServicePacket.pb.h"

namespace push_logic
{

template <class task_type = group_responser>
class group_service_request_responser : public task_responser
{
    public:
        group_service_request_responser();
        virtual ~group_service_request_responser();

        virtual int response(std::string const& id, meet_you::head const& h, std::string const& data);
        virtual net_service::service_response_result parser(std::string const& data, std::string & response);

    protected:

    private:
};

template <class task_type>
group_service_request_responser<task_type>::group_service_request_responser()
{
    //ctor
}

template <class task_type>
group_service_request_responser<task_type>::~group_service_request_responser()
{
    //dtor
}

template <class task_type>
int group_service_request_responser<task_type>::response(std::string const& id, meet_you::head const& h, std::string const& data)
{
    message_format mf;
    meet_you::head hh = h;
    hh.cmd++;
    mf.encode_header(hh, data);
    mf.body_combine();

    fix_dispatch_to_server(id, mf);

    return 0;
}

template<class task_type>
net_service::service_response_result group_service_request_responser<task_type>::parser(std::string const& data, std::string & response)
{
    net_service::service_response_result res;

    meet_you::ServiceRequest sr;
    meet_you::ServiceResponse serr;

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
                task_type task(sr.sn());
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

#endif // GROUP_SERVICE_REQUEST_RESPONSER_H
