#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <exception>
#include <string>

/*
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

*/

#include "json/json.h"

namespace push_logic
{

class json_parser
{
public:
    json_parser();
    virtual ~json_parser();

protected:
    int decode(std::string const& json_str);
    std::string encode(Json::Value const& value);

    Json::Reader reader_;
    Json::StyledWriter writer_;
    Json::Value value_;
};

}
#endif // JSON_PARSER_H
