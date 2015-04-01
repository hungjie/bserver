#ifndef MESSAGE_FORMAT_H
#define MESSAGE_FORMAT_H

#ifdef __linux
#include <arpa/inet.h>
#else
#include<winsock2.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "msg_head.h"

namespace push_logic
{

class message_format
{
public:
    enum
    {
        message_head_length=sizeof(meet_you::head)
    };
    enum
    {
        max_message_length=65535
    };

    message_format();
    virtual ~message_format();

    std::vector<char> const& head() const;

    std::vector<char>& head();

    size_t length() const;

    std::vector<char> const& body() const;

    std::vector<char>& body();

    size_t body_length() const;

    int decode_header(meet_you::head& h);

    void encode_header(const meet_you::head& h, const std::string& body);

    void body_combine();

    void flush();

    std::string const& remote_id() const
    {
        return this->remote_id_;
    }

    void remote_id(std::string const& id)
    {
        this->remote_id_ = id;
    }

    std::string const& server_id() const
    {
        return this->server_id_;
    }

    void server_id(std::string const& id)
    {
        this->server_id_ = id;
    }

    meet_you::head meet_head() const
    {
        meet_you::head h;
        h.cmd = this->cmd_;
        h.len = this->len_;
        h.magic = this->magic_;

        return h;
    }

protected:
private:
    std::vector<char> head_;
    std::vector<char> body_;
    size_t body_length_;
    ef::int32 magic_;
    ef::int32 len_;
    ef::int32 cmd_;

    std::string remote_id_;
    std::string server_id_;
};
}

#endif // MESSAGE_FORMAT_H
