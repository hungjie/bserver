#include "json_push_packet.h"

#include <vector>

#include <boost/lexical_cast.hpp>

#include "log.h"

namespace push_logic
{

json_push_packet::json_push_packet(std::string const& json)
    : packet_type_(0)
    , device_id_(1)
    , sn_("")
{
    if(this->decode(json) == 0)
        this->init_packet_type();
}

json_push_packet::~json_push_packet()
{
}

std::vector<std::string> json_push_packet::cid()
{
    std::vector<std::string> cids;
    if( this->pt_helper_.size() > 0 )
    {
        if(this->pt_helper_["cid"].isArray())
        {
            for(size_t i = 0; i < this->pt_helper_["cid"].size(); ++i)
            {
                cids.push_back(this->pt_helper_["cid"][i].asString());
            }
        }
        else
        {
            cids.push_back(this->pt_helper_.get("cid", "").asString());
        }

        return cids;
    }

    PUSHLOG_WARN("empty cid");

    return cids;
}

std::string json_push_packet::type()
{
    if( this->pt_helper_.size() > 0 )
    {
        if(this->pt_helper_["type"].isString())
            return this->pt_helper_.get("type", "").asString();
        else if(this->pt_helper_["type"].isInt())
        {
            int type = this->pt_helper_.get("type", 0).asInt();
            return boost::lexical_cast<std::string>(type);
        }
    }

    PUSHLOG_WARN("empty type");

    return "";
}

int64_t json_push_packet::msgid()
{
    if( this->pt_helper_.size() > 0 )
    {
        return this->pt_helper_.get("msgid", 0).asUInt();
    }

    PUSHLOG_WARN("empty msgid");

    return -1;
}

int64_t json_push_packet::expiry()
{
    if( this->pt_helper_.size() > 0 )
    {
        return this->pt_helper_.get("expiry", 0).asUInt();
    }

    PUSHLOG_WARN("empty expiry");

    return -1;
}

std::string json_push_packet::data()
{
    if( this->pt_helper_.size() > 0 )
    {
        return this->pt_helper_.get("data", "").asString();
    }

    PUSHLOG_WARN("empty data");

    return "";
}

std::string json_push_packet::to_push_packet_response(std::string const& cid, std::string const& type, int64_t msgid)
{
    Json::Value pt_packet;
    Json::Value pt_response;

    pt_packet["type"] = 11;

    pt_response["cid"] = cid;
    pt_response["type"] = type;
    pt_response["msgid"] = (u_int32_t)msgid;

    pt_packet["push_resp"] = pt_response;

    return this->encode(pt_packet);
}

}
