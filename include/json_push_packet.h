#ifndef JSON_PUSH_PACKET_H
#define JSON_PUSH_PACKET_H

#include <stdint.h>

#include "json_parser.h"

namespace push_logic
{

class json_push_packet : public json_parser
{
public:
    explicit json_push_packet(std::string const& json);
    virtual ~json_push_packet();

    void init_packet_type()
    {
        if(this->value_["type"].isInt())
            this->packet_type_ = this->value_.get("type", 0).asInt();

        if(this->value_["device"].isInt())
            this->device_id_ = this->value_.get("device", 1).asInt();

        if(this->value_["sn"].isString())
            this->sn_ = this->value_.get("sn", "").asString();

        if(this->packet_type_ == 10)
            this->pt_helper_ = this->value_["push_req"];
        else if(this->packet_type_ == 11)
            this->pt_helper_ = this->value_["push_resp"];
    }

    int packet_type()
    {
        return this->packet_type_;
    }

    std::string sn()
    {
        return sn_;
    }

    int device_id()
    {
        return device_id_;
    }

    std::vector<std::string> cid();

    std::string type();

    int64_t msgid();

    int64_t expiry();

    std::string data();

    std::string to_push_packet_response(std::string const& cid, std::string const& type, int64_t msgid);

private:
    int packet_type_;
    int device_id_;
    std::string sn_;

    Json::Value pt_helper_;
};

}

#endif // JSON_PUSH_PACKET_H
