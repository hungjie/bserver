#ifndef MSG_HEAD_H
#define MSG_HEAD_H

#include "base/ef_btype.h"

namespace meet_you
{

using namespace ef;

enum
{
    LOGIN_REQ = 100,
    LOGIN_RESP = LOGIN_REQ + 1,
    SERVICE_REQ = 200,
    SERVICE_RESP = SERVICE_REQ + 1,
    SERVICE_REG_REQ = 300,
    SERVICE_REG_RESP = SERVICE_REG_REQ + 1,

    JSON_PUSH_REQ = 9999,
    JSON_PUSH_RESP = JSON_PUSH_REQ + 1,
};

struct head
{
    int32 magic;
    int32 len;//include head
    //0: keepalive, 1: keepalive resp
    //100: login, 101: login resp
    //200: service, 201: service resp
    //300: regist service 301: regist service resp
    //400: kick client
    //9999:json push request 10000:json push response
    int32 cmd;
};


};

#endif
