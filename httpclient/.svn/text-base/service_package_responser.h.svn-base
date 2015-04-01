#ifndef SERVICE_PACKAGE_RESPONSER_H
#define SERVICE_PACKAGE_RESPONSER_H

#include "task_responser.h"

#include "../proto/ServicePacket.pb.h"

namespace push_logic
{

class service_package_responser : public task_responser
{
    public:
        service_package_responser(std::string const& sn);
        virtual ~service_package_responser();

        virtual int response(std::string const& id, meet_you::head const& h, std::string const& data);
        virtual net_service::service_response_result parser(std::string const& data, std::string & response);

    protected:
        void do_reply_request(ServicePacket& packet_response, ServicePacket& packet
            , std::string& str_url, std::string& authorization, std::string& request_data);

        void do_post_topic_request(ServicePacket& packet_response, ServicePacket& packet
            , std::string& str_url, std::string& authorization, std::string& request_data);

        void do_public_service_request(ServicePacket& packet_response, ServicePacket& packet
            , std::string& str_url, std::string& authorization, std::string& request_data);

        bool parser_group_near_request(ServicePacket& packet_response, ServicePacket & packet, std::string & error);
        bool parser_group_city_request(ServicePacket& packet_response, ServicePacket & packet, std::string & error);

    private:
        std::string sn_;
};

}
#endif // SERVICE_PACKAGE_RESPONSER_H
