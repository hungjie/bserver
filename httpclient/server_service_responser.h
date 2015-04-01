#ifndef SERVER_SERVICE_RESPONSER_H
#define SERVER_SERVICE_RESPONSER_H

#include "task_responser.h"

namespace push_logic
{

class server_service_responser : public task_responser
{
public:
    server_service_responser();
    virtual ~server_service_responser();

    virtual int response(std::string const& id, meet_you::head const& h, std::string const& data);
    virtual net_service::service_response_result parser(std::string const& data, std::string & response);
protected:
private:
};

}
#endif // SERVER_SERVICE_RESPONSER_H
