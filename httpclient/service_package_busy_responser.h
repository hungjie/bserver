#ifndef SERVICE_PACKAGE_BUSY_RESPONSER_H
#define SERVICE_PACKAGE_BUSY_RESPONSER_H

#include "task_responser.h"

namespace push_logic
{

class service_package_busy_responser : public task_responser
{
public:
    service_package_busy_responser();
    virtual ~service_package_busy_responser();

    virtual int response(std::string const& id, meet_you::head const& h, std::string const& data);
    virtual net_service::service_response_result parser(std::string const& data, std::string & response);
protected:
private:
};

}
#endif // SERVICE_PACKAGE_BUSY_RESPONSER_H
