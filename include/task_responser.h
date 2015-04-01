#ifndef TASK_RESPONSE_H
#define TASK_RESPONSE_H

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/serialization/singleton.hpp>

#include "message_format.h"

#include "../proto/connect_server.pb.h"
#include "session_task.h"

#include "service_response_result.h"

namespace push_logic
{

class task_responser
{
public:
    task_responser();
    virtual ~task_responser();

    virtual int response(std::string const& id, meet_you::head const& h, std::string const& data);
    virtual net_service::service_response_result parser(std::string const& data, std::string & response);

    virtual void after_parser(meet_you::ServiceRequest& srp) {}

protected:
    bool Base64Encode(const std::string& input, std::string& output);
    bool Base64Decode(const std::string& input, std::string& output);

    void dispatch_to_server(std::string const& server_id, message_format const& mf);
    void fix_dispatch_to_server(std::string const& server_id, message_format const& mf);
    void dispatch_to_client(std::string const& server_id, message_format const& mf);

private:
};

}

#endif // TASK_RESPONSE_H
