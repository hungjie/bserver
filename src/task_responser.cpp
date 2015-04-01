#include <string>
#include <iostream>
#include <sstream>

#include "task_responser.h"

#include "peer_handle_map.h"

#include "log.h"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include <boost/lexical_cast.hpp>

#include "configwrap.h"

#include "uuid/uuid.h"

using namespace meet_you;
using namespace net_service;

namespace push_logic
{

task_responser::task_responser()
{

}

task_responser::~task_responser()
{

}

service_response_result task_responser::parser(std::string const& data, std::string & res)
{
    service_response_result status;

    ServiceRequest sr;
    sr.ParseFromString(data);

    ServiceResponse srp;

    srp.set_sessid(sr.sessid());
    srp.set_service_type(sr.service_type());
    srp.set_sn(sr.sn());
    srp.set_payload(sr.payload());

    srp.SerializeToString(&res);

    return status;
}

int task_responser::response(std::string const& id, meet_you::head const& h, std::string const& data)
{
    message_format mf;
    meet_you::head hh = h;
    hh.cmd++;
    mf.encode_header(hh, data);
    mf.body_combine();

    dispatch_to_client(id, mf);

    return 0;
}

bool task_responser::Base64Encode(const std::string& input, std::string& output)
{
    typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8> > Base64EncodeIterator;
    std::stringstream result;

    copy(Base64EncodeIterator(input.begin()) , Base64EncodeIterator(input.end()), std::ostream_iterator<char>(result));

    size_t equal_count = (3 - input.length() % 3) % 3;
    for (size_t i = 0; i < equal_count; i++)
    {
        result.put('=');
    }

    output = result.str();

    return output.empty() == false;
}

bool task_responser::Base64Decode(const std::string& input,std:: string& output)
{
    typedef boost::archive::iterators::transform_width<boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6> Base64DecodeIterator;
    std::stringstream result;

    try
    {
        copy(Base64DecodeIterator(input.begin()) , Base64DecodeIterator(input.end()), std::ostream_iterator<char>(result));
    }
    catch(...)
    {
        return false;
    }

    output = result.str();

    return output.empty() == false;
}

void task_responser::dispatch_to_server(std::string const& server_id, message_format const& mf)
{
    if(!server_id.empty())
    {
        boost::shared_ptr<session_base> sb = __PEER_MAP_CLIENT::get_mutable_instance().get(server_id);

        if(sb)
        {
            sb->write(mf);

            std::stringstream s_ss;
            s_ss << "send to connected server:" << server_id << " send count:" << __CONFIGWRAP::get_mutable_instance().sendcount();
            PUSHLOG_INFO(s_ss.str());
        }
        else
        {
            PUSHLOG_WARN( RFORMATE( "can not get link info : " + server_id) );
        }
    }
    else
    {
        PUSHLOG_WARN("can not get the server info to dispatch!");
    }
}

void task_responser::fix_dispatch_to_server(std::string const& server_id, message_format const& mf)
{
    if(!server_id.empty())
    {
        boost::shared_ptr<session_base> sb = __PEER_MAP_CLIENT::get_mutable_instance().fix_get(server_id);

        if(sb)
        {
            sb->write(mf);

            std::stringstream s_ss;
            s_ss << RFORMATE("send to connected server:" + server_id) << " send count:" << __CONFIGWRAP::get_mutable_instance().sendcount();
            PUSHLOG_INFO(s_ss.str());
        }
        else
        {
            PUSHLOG_WARN("can not get link info : " + server_id);
        }
    }
    else
    {
        PUSHLOG_WARN("can not get the server info to dispatch!");
    }
}

void task_responser::dispatch_to_client(std::string const& server_id, message_format const& mf)
{
    if(!server_id.empty())
    {
        boost::shared_ptr<session_base> sb = __PEER_MAP::get_mutable_instance().get(server_id);

        if(sb)
        {
            sb->write(mf);

            std::stringstream s_ss;
            s_ss << RFORMATE("send to connected client:" + server_id) << " send count:" << __CONFIGWRAP::get_mutable_instance().sendcount();
            PUSHLOG_INFO(s_ss.str());
        }
        else
        {
            PUSHLOG_WARN( RFORMATE("can not get link info : " + server_id));
        }
    }
    else
    {
        PUSHLOG_WARN("can not get the server info to dispatch!");
    }
}

}
