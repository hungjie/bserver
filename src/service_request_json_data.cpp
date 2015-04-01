#include "service_request_json_data.h"


namespace push_logic
{

service_request_json_data::service_request_json_data(std::string const& json)
{
    this->decode(json);
}

service_request_json_data::~service_request_json_data()
{
    //dtor
}

std::string service_request_json_data::cid()
{
    if( this->value_.size() > 0 )
    {
        if(this->value_.isMember("cid"))
        {
            if(this->value_["cid"].isString())
            {
                return this->value_.get("cid", "").asString();
            }
        }
    }

    return "";
}

std::string service_request_json_data::service_id()
{
    if( this->value_.size() > 0 )
    {
        if(this->value_.isMember("service_id"))
        {
            if(this->value_["service_id"].isString())
            {
                return this->value_.get("service_id", "").asString();
            }
        }
    }

    return "";
}

std::string service_request_json_data::data()
{
    if( this->value_.size() > 0 )
    {
        if(this->value_.isMember("data"))
        {
            if(this->value_["data"].isString())
            {
                return this->value_.get("data", "").asString();
            }
        }
    }

    return "";
}

std::string service_request_json_data::to_service_request(std::string const& cid, std::string const& service_id, std::string const& data)
{
    Json::Value pt_packet;

    pt_packet["service_id"] = service_id;
    pt_packet["cid"] = cid;
    pt_packet["data"] = data;

    return this->encode(pt_packet);
}

}
