#include "curl_base.h"

namespace Httpclient
{

curl_base::curl_base()
    : headers_(NULL)
{
}

curl_base::~curl_base()
{
    //dtor

}

void curl_base::init_header()
{
}

void curl_base::init_json_header()
{
    headers_ = curl_slist_append(headers_, "Content-type: application/json");
    headers_ = curl_slist_append(headers_, "Accept: application/json");
    headers_ = curl_slist_append(headers_, "Cache-Control: no-cache");
    headers_ = curl_slist_append(headers_, "Pragma: no-cache");
}

void curl_base::add_header(std::vector<std::string> const& headers)
{
    std::vector<std::string>::const_iterator it = headers.begin();
    for(; it != headers.end(); ++it)
    {
        headers_ = curl_slist_append(headers_, (*it).c_str());
    }
}

static size_t CurlOnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
    std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
    if( NULL == str || NULL == buffer )
    {
        return -1;
    }

    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);

    //std::cout << *str << std::endl;

    return nmemb;
}

int curl_base::Post(std::string const& addr, std::string const& request_data, std::string & response_data)
{
    CURLcode res;

    CURL* curl = curl_easy_init();

    if(NULL == curl)
    {
        return CURLE_FAILED_INIT;
    }

    //curl_easy_setopt(curl, CURLOPT_URL, "http://s.notify.live.net/u/1/sin/HmQAAABqSJ27xjnEVDM1Df8WxEHQN5ylphWq6y8EJUr4gq9VzTJ-BWyrlGSJP5MHbrPGG7vHnR_zGHxkotpTo8bx68Tn/d2luZG93c3Bob25lZGVmYXVsdA/aBBgEl-oiUehuL_toiJejA/GXMIMJfqLKk55JY9PIcF50yOZuw");
    curl_easy_setopt(curl, CURLOPT_URL, addr.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlOnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response_data);
    curl_easy_setopt(curl, CURLOPT_POST, 1);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);

    /**
    * 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
    * 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
    */
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

    res =  curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    return res;
}

int curl_base::Get(std::string const& strUrl, std::string & response_data)
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if(NULL == curl)
    {
        return CURLE_FAILED_INIT;
    }

    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlOnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response_data);

    /**
    * 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
    * 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
    */
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return res;
}

}
