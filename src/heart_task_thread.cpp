#include "heart_task_thread.h"

#include <boost/thread/thread.hpp>

#include "peer_handle_client.h"

#include "configwrap.h"

#include "log.h"

#include "io_service_provide.h"

using namespace boost::asio::ip;

namespace push_logic
{

heart_task_thread::heart_task_thread()
{
    //ctor
}

heart_task_thread::~heart_task_thread()
{
    //dtor
}

void heart_task_thread::work()
{
    int sleeps = __CONFIGWRAP::get_const_instance().heart_second();

    while(1)
    {
        boost::this_thread::interruption_point();

        try
        {
            boost::this_thread::sleep(boost::posix_time::seconds(sleeps));

            std::map<int, std::string> m = __CONFIGWRAP::get_mutable_instance().server_map();
            std::map<int, std::string>::iterator it;

            for(it = m.begin(); it != m.end(); ++it)
            {
                std::string id = it->second;

                int connect_count = __CONFIGWRAP::get_const_instance().connect_count();
                for(int i = 0; i < connect_count; ++i )
                {
                    std::stringstream ss;
                    ss << id << ":" << i;
                    boost::shared_ptr<session_base> spt = __PEER_MAP_CLIENT::get_mutable_instance().fix_get(ss.str());

                    if(spt)
                    {
                        meet_you::head h;
                        h.cmd = 0;
                        h.magic = 0;

                        message_format mf;
                        mf.encode_header(h, "");
                        mf.body_combine();

                        spt->post_write(mf);
                    }
                    else
                    {
                        //PUSHLOG_WARN("can not get session for heart from " + id);

                        std::vector<std::string> split;
                        boost::algorithm::split (split, id, boost::algorithm::is_any_of (":"));

                        if(split.size() != 3)
                        {
                            NETLOG(RFORMATE("error connection info:" + id));
                            continue;
                        }

                        //PUSHLOG_INFO("start connect to server:" + split[1] + ":" + split[2]);

                        boost::asio::io_service& io = __IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service();
                        tcp::resolver resolver(io);
                        tcp::resolver::query query(split[1].c_str(), split[2].c_str());
                        tcp::resolver::iterator iterator = resolver.resolve(query);

                        std::string server_id = split[0] + ":" + split[1] + ":" + split[2];

                        boost::shared_ptr<peer_handle_client> c(new peer_handle_client(io, iterator, server_id, ss.str()));
                        c->registe();

                        std::map< std::string, boost::shared_ptr<session_base> >::iterator it;
                        std::map< std::string, boost::shared_ptr<session_base> > m = __PEER_MAP_CLIENT::get_const_instance().m();

                        //*
                        NETLOG("now connections to server:");
                        for(it = m.begin(); it != m.end(); ++it)
                        {
                            NETLOG(it->first);
                        }
                        //*/
                    }
                }
            }
        }
        catch(std::exception const& e)
        {
            NETLOG( RFORMATE(e.what() ));
        }
        catch(...)
        {
            NETLOG( RFORMATE("unkonwn error task" ));
        }
    }
}

}
