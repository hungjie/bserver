#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include <string>
#include <memory>

#include <boost/serialization/singleton.hpp>
#include <boost/filesystem.hpp>

#include <log4cplus/logger.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/appender.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/loggingmacros.h>

namespace push_logic
{

class push_log
{
public:
    push_log() : log_prefix_("default") {}
    ~push_log() {}
    void set_path(std::string const& path);

    void log_prefix(std::string const& log_prefix);

    log4cplus::Logger redis_logger;
    log4cplus::Logger zookeeper_logger;
    log4cplus::Logger system_logger;
    log4cplus::Logger net_logger;
    log4cplus::Logger fail_logger;

    std::string RFORMATE(std::string const& str)
    {
        std::stringstream stemp;
        stemp << "<" << str << ">";
        return stemp.str();
    }

private:
    log4cplus::SharedAppenderPtr _append;
    log4cplus::SharedAppenderPtr _append2;
    log4cplus::SharedAppenderPtr _append3;
    log4cplus::SharedAppenderPtr _append4;
    log4cplus::SharedAppenderPtr _append5;

    std::auto_ptr<log4cplus::Layout> _layout;
    std::auto_ptr<log4cplus::Layout> _layout2;
    std::auto_ptr<log4cplus::Layout> _layout3;
    std::auto_ptr<log4cplus::Layout> _layout4;
    std::auto_ptr<log4cplus::Layout> _layout5;

    std::string log_prefix_;
};

typedef boost::serialization::singleton<push_log> __PUSH_LOG;

#define PUSHLOG_TRACE(logEvent)                               \
    LOG4CPLUS_TRACE(__PUSH_LOG::get_mutable_instance().system_logger, logEvent)

#define PUSHLOG_DEBUG(logEvent)                               \
    LOG4CPLUS_DEBUG(__PUSH_LOG::get_mutable_instance().system_logger, logEvent)

#define PUSHLOG_INFO(logEvent)                               \
    LOG4CPLUS_INFO(__PUSH_LOG::get_mutable_instance().system_logger, logEvent)

#define PUSHLOG_WARN(logEvent)                               \
    LOG4CPLUS_WARN(__PUSH_LOG::get_mutable_instance().system_logger, logEvent)

#define PUSHLOG_ERROR(logEvent)                               \
    LOG4CPLUS_ERROR(__PUSH_LOG::get_mutable_instance().system_logger, logEvent)

#define PUSHLOG_FATAL(logEvent)                               \
    LOG4CPLUS_FATAL(__PUSH_LOG::get_mutable_instance().system_logger, logEvent)

#define REDISLOG(logEvent)                                    \
    LOG4CPLUS_INFO(__PUSH_LOG::get_mutable_instance().redis_logger, logEvent)

#define ZKLOG(logEvent)                                    \
    LOG4CPLUS_INFO(__PUSH_LOG::get_mutable_instance().zookeeper_logger, logEvent)

#define NETLOG(logEvent)                                    \
    LOG4CPLUS_DEBUG(__PUSH_LOG::get_mutable_instance().net_logger, logEvent)

#define FAILLOG(logEvent)                                    \
    LOG4CPLUS_DEBUG(__PUSH_LOG::get_mutable_instance().fail_logger, logEvent)

#define RFORMATE(logEvent)                                     \
    __PUSH_LOG::get_mutable_instance().RFORMATE(logEvent)
}

#endif // LOG_H_INCLUDED
