#ifndef SERVER_SESSION_H
#define SERVER_SESSION_H

#include <deque>

#include "session_base.h"

namespace push_logic
{

class server_session : public session_base
{
public:
    server_session(boost::asio::io_service& io_service);
    virtual ~server_session();

protected:
    virtual int do_head();
    virtual int do_body();

    int open();
private:
};

typedef boost::shared_ptr<server_session> server_session_ptr;

}
#endif // SERVER_SESSION_H
