#ifndef SYSTEM_TOOL_H_INCLUDED
#define SYSTEM_TOOL_H_INCLUDED

#include <string>
#include <iostream>

#include <boost/asio/signal_set.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <boost/asio.hpp>

//#include "peer_handle_client.h"
//#include "peer_handle_server.h"

//#include "io_service_provide.h"

//#include "session_task.h"

//#include "peer_handle_map.h"

//#include "../proto/connect_server.pb.h"
//#include "../proto/push_server.pb.h"

#include "message_format.h"

#include "log.h"
//#include "configwrap.h"
//#include "json_push_packet.h"

//#include "heart_task_thread.h"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include "proto/connect_server.pb.h"
#include "proto/push_server.pb.h"

boost::thread_group pool_;

using namespace boost::asio;
using namespace boost::asio::ip;

using namespace push_logic;
using namespace meet_you;

//typedef boost::shared_ptr<peer_handle_server> peer_server_ptr;

boost::mutex mutex;
int64_t global_test_count = 0;

void test_add_count()
{
    boost::mutex::scoped_lock lock(mutex);
    global_test_count++;
}

void test_print_global_count()
{
    int64_t old_global_test_count = 0;
    while(1)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
        std::stringstream ss;
        ss << "now receive count:" << global_test_count << " tps:" << (global_test_count - old_global_test_count);

        old_global_test_count = global_test_count;

        PUSHLOG_DEBUG(ss.str());
    }
}

bool Base64Encode(const std::string& input, std::string& output)
{
    typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8> > Base64EncodeIterator;
    std::stringstream result;
    copy(Base64EncodeIterator(input.begin()) , Base64EncodeIterator(input.end()), std::ostream_iterator<char>(result));
    size_t equal_count = (3 - input.length() % 3) % 3;
    for (size_t i = 0; i < equal_count; i++)
    {
        result.put('=');
    }
    output = result.str();
    return output.empty() == false;
}

int random_num(int count)
{
    int rand_count = rand() % count;

    //std::cout << rand_count << std::endl;

    return rand_count;
}

boost::asio::io_service io;

void test_send_loop(std::string const& ip, std::string const& port, std::string const& content, std::string const& cid_prex
                    , int count, int i, int start, int num, int cmd)
{
    try
    {
        //*
        //__IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service();

        tcp::resolver resolver(io);
        tcp::resolver::query query(tcp::v4(), ip, port);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io);
        //boost::asio::ip::tcp::no_delay option(true);
        //s.set_option(option);

        boost::asio::connect(s, iterator);
        //*/

        /*
        boost::asio::io_service& io = __IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service();
        tcp::resolver resolver(io);
        tcp::resolver::query query(ip, port);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        boost::shared_ptr<peer_handle_client> c(new peer_handle_client(io, iterator));
        */
        std::string content;
        Base64Encode("{\"type\":\"test\"}", content);

        for(int i = 0; i < count; ++i)
        {
            int start_tmp = start;

            if(num > 0)
                start_tmp += random_num(num);


            std::stringstream cid;
            cid << cid_prex << start_tmp;

            std::stringstream service_content;//f8:a4:5f:53:29:6c
            service_content << "{\"type\":10, \"push_req\":{\"cid\":\"" << cid.str() << "\",\"type\":200,\"msgid\":123456789,\"expiry\":1234,\"data\":\""<< content << "\"}}";

            message_format smf;
            meet_you::head hh;
            hh.cmd = cmd;
            //hh.len = sizeof(hh) + service_content.size();
            hh.magic = 0;

            smf.encode_header(hh, service_content.str());
            smf.body_combine();

            //c->write(smf);
            boost::asio::write(s, boost::asio::buffer(smf.body().data(), smf.length()));

            /*
            boost::asio::deadline_timer tt(io, boost::posix_time::seconds(2));
            tt.wait();

            return;
            */

            //*
            message_format mf;
            boost::asio::read(s, boost::asio::buffer(mf.head().data(), message_format::message_head_length));

            meet_you::head h;
            mf.decode_header(h);

            boost::asio::read(s, boost::asio::buffer(mf.body().data(), mf.body_length()));

            test_add_count();

            //std::string reply(mf.body().begin(),mf.body().end());

            //*/
        }

        s.close();

        //std::cout << "close" << std::endl;
    }
    catch(boost::system::error_code const& e)
    {
        PUSHLOG_ERROR(e.message());
    }
    catch(std::exception const& e)
    {
        PUSHLOG_ERROR( e.what() );
    }
    catch(...)
    {
        PUSHLOG_ERROR("unkonwn thread error");
    }
}

std::string system_params;

void system_ls()
{
    std::vector< boost::shared_ptr<boost::thread> > ts;

    if(system_params.empty())
        return;

    std::vector<std::string> split;

    boost::split(split, system_params, boost::is_any_of(" "), boost::algorithm::token_compress_on);

    int size = split.size();

    if(size == 10)
    {
        int count = atoi(split[5].c_str());
        int loop_num = atoi(split[6].c_str());
        int start = atoi(split[7].c_str());
        int num = atoi(split[8].c_str());
        int cmd = atoi(split[9].c_str());
        for(int i = 0; i < count; i++)
        {
            //boost::shared_ptr<boost::thread> t( new boost::thread(boost::bind(&test_send, split[1], split[2], split[3])) );
            boost::shared_ptr<boost::thread> t( new boost::thread(boost::bind(&test_send_loop, split[1], split[2], split[3], split[4], loop_num, i, start, num, cmd)) );
            ts.push_back(t);
        }

        for(int i = 0; i < count; i++)
        {
            ts[i]->join();
        }
    }
}

void test_send_specify(std::string const& ip, std::string const& port, std::string const& content, std::string const& cid_prex
                       , int count, int i, int cmd)
{
    try
    {
        //*
        //__IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service();

        tcp::resolver resolver(io);
        tcp::resolver::query query(tcp::v4(), ip, port);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io);
        //boost::asio::ip::tcp::no_delay option(true);
        //s.set_option(option);

        boost::asio::connect(s, iterator);
        //*/

        /*
        boost::asio::io_service& io = __IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service();
        tcp::resolver resolver(io);
        tcp::resolver::query query(ip, port);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        boost::shared_ptr<peer_handle_client> c(new peer_handle_client(io, iterator));
        */
        std::stringstream strcontent;
        strcontent << "{\"title\":\"test" << i << "\",\"type\":\"6\", \"url\":\"http://www.baidu.com\", \"content\":\"" << content << i << "\"}" ;

        std::string content2;
        Base64Encode(strcontent.str(), content2);

        for(int i = 0; i < count; ++i)
        {
            std::stringstream cid;
            cid << cid_prex;

            std::stringstream service_content;//f8:a4:5f:53:29:6c
            service_content << "{\"type\":10, \"push_req\":{\"cid\":\"" << cid.str() << "\",\"type\":100,\"msgid\":123456789,\"expiry\":1234,\"data\":\""<< content2 << "\"}}";

            message_format smf;
            meet_you::head hh;
            hh.cmd = cmd;
            //hh.len = sizeof(hh) + service_content.size();
            hh.magic = 0;

            smf.encode_header(hh, service_content.str());
            smf.body_combine();

            //c->write(smf);
            boost::asio::write(s, boost::asio::buffer(smf.body().data(), smf.length()));

            /*
            boost::asio::deadline_timer tt(io, boost::posix_time::seconds(2));
            tt.wait();

            return;
            */

            //*
            message_format mf;
            boost::asio::read(s, boost::asio::buffer(mf.head().data(), message_format::message_head_length));

            meet_you::head h;
            mf.decode_header(h);

            boost::asio::read(s, boost::asio::buffer(mf.body().data(), mf.body_length()));

            test_add_count();

            //std::string reply(mf.body().begin(),mf.body().end());

            //*/
        }

        s.close();

        //std::cout << "close" << std::endl;
    }
    catch(boost::system::error_code const& e)
    {
        PUSHLOG_ERROR(e.message());
    }
    catch(std::exception const& e)
    {
        PUSHLOG_ERROR( e.what() );
    }
    catch(...)
    {
        PUSHLOG_ERROR("unkonwn thread error");
    }
}

void test_send_service_request(std::string const& ip, std::string const& port, std::string const& content, std::string const& cid_prex
                               , int count, int i, int cmd)
{
    try
    {
        tcp::resolver resolver(io);
        tcp::resolver::query query(tcp::v4(), ip, port);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io);
        //boost::asio::ip::tcp::no_delay option(true);
        //s.set_option(option);

        boost::asio::connect(s, iterator);

        ServiceRequest sr;
        sr.set_sessid("test");
        sr.set_service_type(100);
        sr.set_sn("1234567890");

        std::string payload;

        PushPacket pp;
        pp.set_type(12);
        PushCacheRequest* pcr = pp.mutable_push_cache_request();
        pcr->set_cid(cid_prex);
        pcr->set_type("200");
        pcr->set_start_msgid(0);
        pcr->set_size(5);

        pp.SerializeToString(&payload);

        sr.set_payload(payload);

        std::string service_content;

        sr.SerializeToString(&service_content);

        for(size_t pos = 0; pos < count; ++pos)
        {
            message_format smf;
            meet_you::head hh;
            hh.cmd = cmd;
            //hh.len = sizeof(hh) + service_content.size();
            hh.magic = 0;

            smf.encode_header(hh, service_content.c_str());
            smf.body_combine();

            boost::asio::write(s, boost::asio::buffer(smf.body().data(), smf.length()));

            message_format mf;
            boost::asio::read(s, boost::asio::buffer(mf.head().data(), message_format::message_head_length));

            meet_you::head h;
            mf.decode_header(h);

            boost::asio::read(s, boost::asio::buffer(mf.body().data(), mf.body_length()));

            std::string c = std::string(mf.body().begin(), mf.body().end());

            ServiceResponse srs;
            srs.ParseFromString(c);

            std::cout << "sn:" << srs.sn() << std::endl;

            PushPacket pp2;
            pp2.ParseFromString( srs.payload() );

            std::cout << "response type:" << pp2.type() << std::endl;

            PushCacheResponse pcrs = pp2.push_cache_response();
            std::cout << "cid:" << pcrs.cid() << " left size:" << pcrs.left_size() << " last msgid:"<< pcrs.last_msgid() << std::endl;

            test_add_count();
        }
    }
    catch(boost::system::error_code const& e)
    {
        PUSHLOG_ERROR(e.message());
    }
    catch(std::exception const& e)
    {
        PUSHLOG_ERROR( e.what() );
    }
    catch(...)
    {
        PUSHLOG_ERROR("unkonwn thread error");
    }
}

void system_ss()
{
    std::vector< boost::shared_ptr<boost::thread> > ts;

    if(system_params.empty())
        return;

    std::vector<std::string> split;

    boost::split(split, system_params, boost::is_any_of(" "), boost::algorithm::token_compress_on);

    int size = split.size();

    if(size == 8)
    {
        int count = atoi(split[5].c_str());
        int loop_num = atoi(split[6].c_str());
        int cmd = atoi(split[7].c_str());

        for(int i = 0; i < count; i++)
        {
            //boost::shared_ptr<boost::thread> t( new boost::thread(boost::bind(&test_send, split[1], split[2], split[3])) );
            boost::shared_ptr<boost::thread> t( new boost::thread(boost::bind(&test_send_specify, split[1], split[2], split[3], split[4], loop_num, i, cmd)) );
            ts.push_back(t);
        }

        for(int i = 0; i < count; i++)
        {
            ts[i]->join();
        }
    }
}

void system_sr()
{
    std::vector< boost::shared_ptr<boost::thread> > ts;

    if(system_params.empty())
        return;

    std::vector<std::string> split;

    boost::split(split, system_params, boost::is_any_of(" "), boost::algorithm::token_compress_on);

    int size = split.size();

    if(size == 8)
    {
        int count = atoi(split[5].c_str());
        int loop_num = atoi(split[6].c_str());
        int cmd = atoi(split[7].c_str());

        for(int i = 0; i < count; i++)
        {
            //boost::shared_ptr<boost::thread> t( new boost::thread(boost::bind(&test_send, split[1], split[2], split[3])) );
            boost::shared_ptr<boost::thread> t( new boost::thread(boost::bind(&test_send_service_request, split[1], split[2], split[3], split[4], loop_num, i, cmd)) );
            ts.push_back(t);
        }

        for(int i = 0; i < count; i++)
        {
            ts[i]->join();
        }
    }

}

void test_send(std::string const& ip, std::string const& port, std::string const& content, int i)
{
    //boost::mutex::scoped_lock lock(mutex);

    try
    {
        /*
        boost::asio::io_service& io = __IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service();

        tcp::resolver resolver(io);
        tcp::resolver::query query(tcp::v4(), ip, port);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io);
        boost::asio::connect(s, iterator);
        */
        /*
                boost::asio::io_service& io = __IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service();
                tcp::resolver resolver(io);
                tcp::resolver::query query(ip, port);
                tcp::resolver::iterator iterator = resolver.resolve(query);

                boost::shared_ptr<peer_handle_client> c(new peer_handle_client(io, iterator));
        */

        /*
        ServiceRequest sr;
        PushPacket pp;
        PushRequest pr;

        sr.set_sessid("1101:19:19:1399444694435");
        sr.set_service_type(100);
        sr.set_sn("1234567890");

        pp.set_type(10);

        pr.set_cid("hongzhao");
        pr.set_type("test");
        pr.set_msgid(888);
        pr.set_expiry(10);
        pr.set_data(content);

        pp.set_allocated_push_req(&pr);

        std::string service_req;
        pp.SerializeToString(&service_req);
        sr.set_payload(service_req);

        std::string service_content;
        sr.SerializeToString(&service_content);
        */

        /*
                std::stringstream service_content;
                service_content << "{\"type\":10, \"push_req\":{\"cid\":\"f8:a4:5f:53:29:6c\",\"type\":200,\"msgid\":123456789,\"expiry\":1234,\"data\":\"" << i << "\"}}";

                message_format smf;
                meet_you::head hh;
                hh.cmd = 8889;
                //hh.len = sizeof(hh) + service_content.size();
                hh.magic = 0;

                smf.encode_header(hh, service_content.str());
                smf.body_combine();
        */
        //boost::asio::write(s, boost::asio::buffer(smf.body().data(), smf.length()));
//       c->write(smf);

        /*
        boost::asio::deadline_timer tt(io, boost::posix_time::seconds(2));
        tt.wait();

        return;
        */

        /*
        message_format mf;
        boost::asio::read(s, boost::asio::buffer(mf.head().data(), message_format::message_head_length));

        meet_you::head h;
        mf.decode_header(h);

        boost::asio::read(s, boost::asio::buffer(mf.body().data(), mf.body_length()));

        std::string reply(mf.body().begin(),mf.body().end());
        std::cout << "reply:" << reply << std::endl;
        */

        /*
        ServiceResponse srp;
        srp.ParseFromArray(mf.body().data(), mf.body_length());

        PushPacket ppr;
        ppr.ParseFromString(srp.payload());

        PushResponse prr = ppr.push_resp();

        std::cout << "reply content:" << prr.cid() << prr.type() << prr.msgid() << std::endl;
        */

        //s.close();

        //std::cout << "close" << std::endl;
    }
    catch(boost::system::error_code const& e)
    {
        std::cerr << e.message() << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "unkonwn thread error" << std::endl;
    }
}

void command()
{
    std::string input_string;
    while (std::getline(std::cin, input_string))
    {
        if(input_string == "quit")
        {

            break;
        }
        else
        {
            std::vector<std::string> split;
            std::vector<std::string>::const_iterator i;

            boost::split(split, input_string, boost::is_any_of(" "), boost::algorithm::token_compress_on);

            int size = split.size();
            if( size > 0)
            {
                /*if(split[0] == "c")
                {
                    if(size == 4)
                    {
                        int count = atoi(split[3].c_str());

                        for(int i = 0; i < count; ++i)
                        {
                            boost::asio::io_service& io = __IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service();
                            tcp::resolver resolver(io);
                            tcp::resolver::query query(split[1].c_str(), split[2].c_str());
                            tcp::resolver::iterator iterator = resolver.resolve(query);

                //                            boost::shared_ptr<peer_handle_client> c(new peer_handle_client(io, iterator));
                //                            c->registe();
                        }
                    }
                }
                else if(split[0] == "s")
                {
                    if(size == 4)
                    {
                        std::string dev_id = "f8:a4:5f:53:29:6c";

                        //std::map<std::string, meet_you::Sess> m;
                        //std::map<std::string, meet_you::Sess>::iterator it;
                        //__CONFIGWRAP::get_mutable_instance().get_session(dev_id, m);
                      if(m.size() <= 0)
                        {
                            std::cout << "error get session" << std::endl;
                            return;
                        }

                        it = m.begin();

                        for(; it != m.end(); ++it)
                        {
                            ServiceRequest sr;
                            PushPacket pp;

                            PushRequest* pr = pp.mutable_push_req();

                            std::string session_id = it->first;
                            std::cout << "session id:" << session_id << std::endl;

                            sr.set_sessid(session_id);
                            sr.set_service_type(100);
                            sr.set_sn("1234567890");

                            pp.set_type(10);

                            pr->set_cid(dev_id);
                            pr->set_type("test");
                            pr->set_msgid(888);
                            pr->set_expiry(10);
                            pr->set_data(split[3]);

                            std::string service_req;
                            pp.SerializeToString(&service_req);
                            sr.set_payload(service_req);

                            std::string service_content;
                            sr.SerializeToString(&service_content);

                            message_format smf;
                            meet_you::head hh;
                            hh.cmd = 200;
                            hh.len = sizeof(hh) + service_content.size();
                            hh.magic = 0;

                            smf.encode_header(hh, service_content);
                            smf.body_combine();

                            std::stringstream id;
                            id << "server:" << split[1] << ":" << split[2] <<":0" ;

                            boost::shared_ptr<session_base> s = __PEER_MAP_CLIENT::get_mutable_instance().fix_get(id.str());
                            if(s)
                            {
                                s->write(smf);
                            }
                            else
                            {
                                std::cout << "no found " << id.str() << std::endl;
                            }

                        }
                    }
                }
                else */if(split[0] == "cs")
                {
                    std::vector< boost::shared_ptr<boost::thread> > ts;

                    if(size == 5)
                    {
                        int count = atoi(split[4].c_str());
                        for(int i = 0; i < count; i++)
                        {
                            boost::shared_ptr<boost::thread> t( new boost::thread(boost::bind(&test_send, split[1], split[2], split[3], i)) );
                            //boost::shared_ptr<boost::thread> t( new boost::thread(boost::bind(&test_send_loop, split[1], split[2], split[3], split[5])));
                            boost::this_thread::sleep(boost::posix_time::milliseconds(1));
                            ts.push_back(t);
                        }

                        for(int i = 0; i < count; i++)
                        {
                            ts[i]->join();
                        }
                    }
                }
                else if(split[0] == "ls")
                {
                    std::vector< boost::shared_ptr<boost::thread> > ts;

                    if(size == 10)
                    {
                        int count = atoi(split[5].c_str());
                        int loop_num = atoi(split[6].c_str());
                        int start = atoi(split[7].c_str());
                        int num = atoi(split[8].c_str());
                        int cmd = atoi(split[9].c_str());
                        for(int i = 0; i < count; i++)
                        {
                            //boost::shared_ptr<boost::thread> t( new boost::thread(boost::bind(&test_send, split[1], split[2], split[3])) );
                            boost::shared_ptr<boost::thread> t( new boost::thread(boost::bind(&test_send_loop, split[1], split[2], split[3], split[4], loop_num, i, start, num, cmd)) );
                            ts.push_back(t);
                        }

                        for(int i = 0; i < count; i++)
                        {
                            ts[i]->join();
                        }
                    }
                }
                else if(split[0] == "help")
                {
                    std::cout << "ls [ip] [port] [content] [cidprex] [threadcount] [perloopnum] [startcid] [cidcount] [cmd]" << std::endl;
                }
            }
        }

        boost::this_thread::interruption_point();
    }

}

#endif // SYSTEM_TOOL_H_INCLUDED
