#ifndef GROUP_RESPONSER_H
#define GROUP_RESPONSER_H

#include "task_responser.h"

#include "proto/ServicePacket.pb.h"

namespace push_logic
{

class group_responser : public task_responser
{
    public:
        group_responser(std::string const& sn);
        virtual ~group_responser();

        virtual int response(std::string const& id, meet_you::head const& h, std::string const& data);
        virtual net_service::service_response_result parser(std::string const& data, std::string & response);

    protected:
        bool parser_group_near_request(ServicePacket& packet_response, ServicePacket & packet, std::string & error);
        bool parser_group_city_request(ServicePacket& packet_response, ServicePacket & packet, std::string & error);

    private:
        std::string sn_;
};

}

#endif // GROUP_RESPONSER_H
