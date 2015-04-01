#include <boost/asio/signal_set.hpp>

#include <curl/curl.h>

#include "ffpython.h"

#include "peer_handle_client.h"
#include "peer_handle_server.h"

#include "io_service_provide.h"

#include "session_task.h"

#include "peer_handle_map.h"

#include "log.h"
#include "configwrap.h"
#include "json_push_packet.h"

#include "heart_task_thread.h"

using namespace boost::asio;
using namespace boost::asio::ip;

using namespace push_logic;
using namespace meet_you;

typedef boost::shared_ptr<peer_handle_server> peer_server_ptr;
peer_server_ptr server;

bool stop_enter = false;
//boost::mutex mutex_stop;


class system_exst_thread
{
public:
    system_exst_thread()
        : stop_(false)
    {
    }

    void run()
    {
        while(!stop_)
        {
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        }

        __PEER_MAP_CLIENT::get_mutable_instance().close_all();

        if(server)
            server->stop();

        curl_global_cleanup();
        PyGILState_Ensure();
        Py_Finalize();

        exit(1);
    }

    void stop()
    {
        stop_ = true;
    }

private:
    bool stop_;
};

system_exst_thread system_thread;

void stop()
{
    std::cout << "signal stop handle" << std::endl;

    system_thread.stop();
}

int main(int argc, char** argv)
{
    try
    {
        int port = 0;
        std::string log_path = "/tmp/log";
        std::string config_path = "/usr/etc/server_service_config.ini";

        if(argc == 4)
        {
            port = std::atoi(argv[1]);
            log_path = argv[2];
            config_path = argv[3];
        }

        __PUSH_LOG::get_mutable_instance().log_prefix("bserver");
        __PUSH_LOG::get_mutable_instance().set_path(log_path.c_str());
        PUSHLOG_INFO("set log path : " + log_path);

        /* Must initialize libcurl before any threads are started */
        curl_global_init(CURL_GLOBAL_ALL);

        Py_Initialize();
        PyEval_InitThreads();
        // 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁。
        PyEval_ReleaseThread(PyThreadState_Get());

        boost::filesystem::path path(config_path.c_str());

        //boost::filesystem::current_path() /= "logic_config.ini"
        if( __CONFIGWRAP::get_mutable_instance().init( path ) )
        {
            return 1;
        }

        srand((unsigned)time(0));

        if(port == 0)
            port = __CONFIGWRAP::get_const_instance().port();

        std::cout << "listen:" << port << std::endl;

        __IO_SERVICE_PROVIDE::get_mutable_instance();

        int thread_count = __CONFIGWRAP::get_const_instance().thread_cout();

        //boost::thread t(boost::bind(&command));

        for(int i = 0; i < thread_count; i++)
        {
            __IO_SERVICE_PROVIDE::get_mutable_instance().work().create_thread(boost::bind(&session_task::work, &__SESSION_TASK::get_mutable_instance()));
            //__IO_SERVICE_PROVIDE::get_mutable_instance().task_thread().insert( std::make_pair(thread->get_id(), thread) );
        }

        boost::thread heart_thread(boost::bind(&heart_task_thread::work, &__HEART_TASK::get_mutable_instance()));

        boost::asio::signal_set signals(__IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service());

        signals.add(SIGPIPE);
        //signals.add(SIGCHLD);
        signals.add(SIGTSTP);
        signals.add(SIGTTOU);
        signals.add(SIGTTIN);
        signals.add(SIGHUP);
        signals.add(SIGQUIT);
        signals.add(SIGURG);
        signals.add(SIGTERM);
        signals.add(SIGINT);

        signals.async_wait(boost::bind(stop));

        tcp::endpoint endpoint(tcp::v4(), port);

        server.reset( new peer_handle_server(endpoint) );

        __CONFIGWRAP::get_mutable_instance().init_server_connection();

        boost::thread system_thread_exit(boost::bind(&system_exst_thread::run, &system_thread));

        server->run();

        __IO_SERVICE_PROVIDE::get_mutable_instance().run();

        server->join();
        heart_thread.join();

        system_thread_exit.join();

        __IO_SERVICE_PROVIDE::get_mutable_instance().stop();

        curl_global_cleanup();
        PyGILState_Ensure();
        Py_Finalize();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    catch(...)
    {
        std::cerr << "unkonwn exception" << std::endl;
    }

    return 0;
}
