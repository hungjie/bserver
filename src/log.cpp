#include "log.h"

#include<unistd.h>

using namespace log4cplus;
using namespace log4cplus::helpers;

namespace push_logic
{

void push_log::log_prefix(std::string const& log_prefix)
{
    log_prefix_ = log_prefix;
}

void push_log::set_path(std::string const& path)
{
    boost::filesystem::path p(path);

    if(!boost::filesystem::exists(p))
    {
        boost::filesystem::create_directories(p);
    }

#ifdef KAFKAVERSION
    _append = SharedAppenderPtr(new DailyRollingFileAppender(path + "/" + log_prefix_ + ".log", HOURLY, true, 100));
    _append2 = SharedAppenderPtr(new DailyRollingFileAppender(path + "/" + log_prefix_ + "redis.log", DAILY, true, 10));
    _append3 = SharedAppenderPtr(new DailyRollingFileAppender(path + "/" + log_prefix_ + "zookeeper.log", DAILY, true, 10));
    _append4 = SharedAppenderPtr(new DailyRollingFileAppender(path + "/" + log_prefix_ + "net.log", DAILY, true, 10));
    _append5 = SharedAppenderPtr(new DailyRollingFileAppender(path + "/" + log_prefix_ + "fail_task.log", DAILY, true, 10));
#else
    _append = SharedAppenderPtr(new DailyRollingFileAppender(path + "/" + log_prefix_ + "_service.log", HOURLY, true, 100));
    _append2 = SharedAppenderPtr(new DailyRollingFileAppender(path + "/" + log_prefix_ + "_redis.log", DAILY, true, 10));
    _append3 = SharedAppenderPtr(new DailyRollingFileAppender(path + "/" + log_prefix_ + "_zookeeper.log", DAILY, true, 10));
    _append4 = SharedAppenderPtr(new DailyRollingFileAppender(path + "/" + log_prefix_ + "_net_log.log", DAILY, true, 10));
    _append5 = SharedAppenderPtr(new DailyRollingFileAppender(path + "/" + log_prefix_ + "_fail_task.log", DAILY, true, 10));
#endif

    std::stringstream pattern;
    pattern << "[%D{%m/%d/%y %H:%M:%S:%q}] [" << getpid() << "] [THREAD=%t] ["<< log_prefix_.c_str() << "] [%p] [%l] [%m]%n";
    _layout = std::auto_ptr<Layout>(new PatternLayout(pattern.str()));
    _layout2 = std::auto_ptr<Layout>(new PatternLayout(pattern.str()));
    _layout3 = std::auto_ptr<Layout>(new PatternLayout(pattern.str()));
    _layout4 = std::auto_ptr<Layout>(new PatternLayout(pattern.str()));
    _layout5 = std::auto_ptr<Layout>(new PatternLayout(pattern.str()));

    _append->setLayout( _layout );
    _append2->setLayout( _layout2 );
    _append3->setLayout( _layout3 );
    _append4->setLayout( _layout4 );
    _append5->setLayout( _layout5 );

    system_logger = Logger::getInstance("system");
    system_logger.addAppender(_append);
    system_logger.setLogLevel(ALL_LOG_LEVEL);

    //redis_logger = Logger::getInstance("redis");

    //Logger::getRoot().addAppender(_append);
    //Logger root = Logger::getRoot();
    redis_logger = Logger::getInstance("redis");
    redis_logger.addAppender(_append2);
    redis_logger.setLogLevel(ALL_LOG_LEVEL);
    //Logger subTest = Logger::getInstance("test.subtest");
    //Logger::getRoot().setLogLevel(ALL_LOG_LEVEL);

    zookeeper_logger = Logger::getInstance("zookeeper");
    zookeeper_logger.addAppender(_append3);
    zookeeper_logger.setLogLevel(ALL_LOG_LEVEL);

    net_logger = Logger::getInstance("net");
    net_logger.addAppender(_append4);
    net_logger.setLogLevel(ALL_LOG_LEVEL);

    fail_logger = Logger::getInstance("fail");
    fail_logger.addAppender(_append5);
    fail_logger.setLogLevel(ALL_LOG_LEVEL);
}

}
