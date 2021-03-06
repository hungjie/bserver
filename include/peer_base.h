#ifndef PEER_BASE_H
#define PEER_BASE_H

#include <csignal>
#include <cstring>
#include <unistd.h>
#include <execinfo.h>

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "message_format.h"

namespace push_logic
{

class peer_base
{
public:
    peer_base();
    virtual ~peer_base();

protected:
    void stop();
    void dump_stack(std::stringstream & log);

    boost::asio::io_service& io_service_;

private:
    //boost::asio::signal_set signals_;

};

}
#endif // PEER_BASE_H
