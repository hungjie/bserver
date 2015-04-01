#ifndef CURL_BASE_H
#define CURL_BASE_H

#include <string>
#include <vector>

#include "curl/curl.h"

namespace Httpclient
{

class curl_base
{
    public:
        curl_base();
        virtual ~curl_base();

        int Post(std::string const& addr, std::string const& request_data, std::string & response_data);
        int Get(std::string const& addr, std::string & response_data);

        void add_header(std::vector<std::string> const& headers);
        void init_json_header();

    protected:
        void init_header();

    private:
        struct curl_slist *headers_;
};

}
#endif // CURL_BASE_H
