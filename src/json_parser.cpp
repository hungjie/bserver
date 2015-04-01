#include "json_parser.h"

#include "log.h"

namespace push_logic
{

json_parser::json_parser()
{
    //ctor
}

json_parser::~json_parser()
{
    //dtor
}

int json_parser::decode(std::string const& json_str)
{
    try
    {
        if( !reader_.parse( json_str, value_) )
        {
            PUSHLOG_ERROR(reader_.getFormatedErrorMessages());
            return -1;
        }
    }
    catch(std::exception const& error)
    {
        PUSHLOG_ERROR(error.what());

        return -1;
    }
    catch(...)
    {
        PUSHLOG_ERROR("json parser error");
        return -1;
    }

    if(value_.isNull() || !value_.isObject())
    {
        PUSHLOG_ERROR("json parser value is null");
        return -1;
    }

    return 0;
}

std::string json_parser::encode(Json::Value const& value)
{
    try
    {
        return writer_.write(value);
    }
    catch(std::exception const& error)
    {
        PUSHLOG_ERROR(error.what());
    }
    catch(...)
    {
        PUSHLOG_ERROR("json encode error");
    }

    return "";
}

}
