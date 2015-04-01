#include <iostream>

#include "message_format.h"

#include "log.h"

namespace push_logic
{

message_format::message_format()
    : body_length_(0)
{
    this->head_.resize(12);
}

message_format::~message_format()
{
}

std::vector<char> const& message_format::head() const
{
    return this->head_;
}

std::vector<char>& message_format::head()
{
    return this->head_;
}

size_t message_format::length() const
{
    return message_head_length + this->body_length_;
}

std::vector<char> const& message_format::body() const
{
    return this->body_;
}

std::vector<char>& message_format::body()
{
    return this->body_;
}

size_t message_format::body_length() const
{
    return this->body_length_;
}

int message_format::decode_header(meet_you::head& h)
{
    char* pc = const_cast<char*>(this->head_.data());

    h.magic = ntohl(*(ef::int32*)pc);
    this->magic_ = h.magic;
    pc += 4;
    h.len = ntohl(*(ef::int32*)pc);
    this->len_ = h.len;
    pc += 4;
    h.cmd = ntohl(*(ef::int32*)pc);
    this->cmd_ = h.cmd;

    this->body_length_ = h.len - message_head_length;

    if(this->body_length_ < 0 || this->body_length_ > 1024*1024)
    {
        PUSHLOG_ERROR("body_length < 0 or > 1024*1024");
        return -1;
    }

    this->body_.resize(message_head_length + this->body_length_);

    return 0;
}

void message_format::encode_header(const meet_you::head& h, const std::string& body)
{
    meet_you::head rh;
    this->body_length_ = body.size();
    this->body_.resize(message_head_length + this->body_length_);

    rh.cmd = htonl(h.cmd);
    rh.magic = htonl(h.magic);
    rh.len = htonl(sizeof(rh) + this->body_length_);

    this->head_.assign((char*)&rh, (char*)&rh + message_head_length);
    this->body_.assign(body.begin(), body.end());
}

void message_format::body_combine()
{
    this->body_.insert(this->body_.begin(), this->head_.begin(), this->head_.end());
}

void message_format::flush()
{
    this->body_length_ = 0;
}

}
