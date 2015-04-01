#ifndef SERVICE_REQUEST_JSON_DATA_H
#define SERVICE_REQUEST_JSON_DATA_H

#include "json_parser.h"

namespace push_logic
{

class service_request_json_data : public json_parser
{
    public:
        service_request_json_data(){}
        service_request_json_data(std::string const& json);
        virtual ~service_request_json_data();

        std::string to_service_request(std::string const& cid, std::string const& service_id, std::string const& data);

        std::string cid();
        std::string service_id();
        std::string data();

    protected:
    private:

};

}
#endif // SERVICE_REQUEST_JSON_DATA_H
