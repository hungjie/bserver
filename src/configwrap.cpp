#include "configwrap.h"

#include <boost/lexical_cast.hpp>

#include "log.h"

#include "peer_handle_client.h"

#include "io_service_provide.h"

#include "peer_handle_map.h"

using namespace boost::asio::ip;

namespace push_logic
{

configwrap::configwrap()
    : port_(0)
    , id_(1)
    , send_count_id_(1)
    , connect_count_(1)
    , heart_second_(60)
    , thread_cout_(boost::thread::hardware_concurrency())
    , io_pool_size_(0)
{
    //ctor
}

configwrap::~configwrap()
{
    //dtor
}

void zookeeper_log(std::string const& log)
{
    ZKLOG(log);
}

int servlist_callback_func(int type, void* obj)
{
    return  __CONFIGWRAP::get_mutable_instance().servlist_callback(type);
}

int configwrap::init_zookeeper()
{
    std::string zookeeper_path = this->get("zookeeper.url");
    int zk_timeout = this->pt_.get<int>("zookeeper.timeout");

    if(zookeeper_path.empty())
    {
        PUSHLOG_ERROR("empty zookeeper url!");
        return -1;
    }

    int zk_status;
    if(zk_timeout > 0)
    {
        zk_status = zk_.init(zookeeper_path, zk_timeout);
    }
    else
    {
        zk_status = zk_.init(zookeeper_path);
    }

    if(zk_status != 0)
    {
        PUSHLOG_ERROR("zk init error!");
        return -1;
    }

    zk_.setLogCB(zookeeper_log);

    std::string serverlist_path = this->get("zookeeper.serverlist_path");
    if(serverlist_path.empty())
    {
        PUSHLOG_ERROR("empty serverlist path!");
        return -1;
    }

    std::string disable_serverlist_path = this->get("zookeeper.disable_serverlist_path");
    if(disable_serverlist_path.empty())
    {
        PUSHLOG_ERROR("empty disable_serverlist_path path!");
        return -1;
    }

    if(this->svlstCache_.init(&zk_, serverlist_path, disable_serverlist_path) != 0)
    {
        PUSHLOG_ERROR("servlist cache init faild");
        return -1;
    }

    this->svlstCache_.set_callback(servlist_callback_func, 0);

    session_path_ = this->get("zookeeper.session_path");
    if(session_path_.empty())
    {
        PUSHLOG_ERROR("empty zookeeper session path!");
        return -1;
    }

    session_path_bak_ = this->get("zookeeper.session_path_bak");
    if(session_path_bak_.empty())
    {
        PUSHLOG_ERROR("empty zookeeper session_path_bak");
        return -1;
    }
    /*
        if(sessCache_.init(&zk_, session_path, session_path_bak) != 0)
        {
            PUSHLOG_ERROR("zookeeper session init error");
            return -1;
        }
    */

    db_group_path_ = this->get("zookeeper.db_group_path");
    if(db_group_path_.empty())
    {
        PUSHLOG_ERROR("empty zookeeper db_group_path");
        return -1;
    }

    //std::vector<meet_you::serv_cfg> serv_cfgs;
    meet_you::SvList serv_cfgs;

    this->svlstCache_.get_list_all(0, serv_cfgs);

    if(serv_cfgs.empty())
    {
        PUSHLOG_INFO( "redis server list empty") ;
        //return -1;
    }
    else
    {
        for(size_t i = 0; i < serv_cfgs.size(); ++i)
        {
            std::stringstream ss;
            ss << "server:" << serv_cfgs[i].ipaddr << ":" << serv_cfgs[i].port;

            PUSHLOG_INFO(RFORMATE(ss.str()));

            this->server_map_.insert( std::make_pair(serv_cfgs[i].id, ss.str()) );
        }
    }

    return 0;
}

int configwrap::init_session_n_srvlist()
{
    std::string dburls_str = this->get("session.dburls");
    boost::algorithm::split (dburls_, dburls_str, boost::algorithm::is_any_of (","));

    std::string bakdburls_str = this->get("session.bakdburls");
    boost::algorithm::split (bakdburls_, bakdburls_str, boost::algorithm::is_any_of (","));

    std::vector<std::string> dbservs;
    std::string dbserv_str = this->get("session.dbservlist");
    boost::algorithm::split (dbservs, dbserv_str, boost::algorithm::is_any_of (","));

    std::vector<std::string> dbbakservs;
    std::string bakserv_str = this->get("session.bakservlist");
    boost::algorithm::split (dbbakservs, bakserv_str, boost::algorithm::is_any_of (","));

    std::string msg_cache_url = this->get("messagebox.msg_cache_url");
    std::vector<std::string> msg_cache_urls;
    boost::algorithm::split (msg_cache_urls, msg_cache_url, boost::algorithm::is_any_of (","));

    std::string bakmsg_cache_url = this->get("messagebox.bakmsg_chache_url");
    std::vector<std::string> bakmsg_cache_urls;
    boost::algorithm::split (bakmsg_cache_urls, bakmsg_cache_url, boost::algorithm::is_any_of (","));

    size_t bakmsg_cache_urls_size = bakmsg_cache_urls.size();
    for(size_t i = 0; i < msg_cache_urls.size(); ++i)
    {
        if(i <= bakmsg_cache_urls_size -1)
        {
            message_box_urls_[msg_cache_urls[i]] = bakmsg_cache_urls[i];
        }
        else
        {
            message_box_urls_[msg_cache_urls[i]] = "";
        }
    }

    size_t bakdb_size = bakdburls_.size();
    for(size_t i = 0; i < dburls_.size(); ++i)
    {
        if(i <= bakdb_size - 1)
        {
            db_bak_urls_[dburls_[i]] = bakdburls_[i];
        }
        else
        {
            db_bak_urls_[dburls_[i]] = "";
        }
    }

    if (this->sess_cache_.init(dburls_, bakdburls_) != 0)
    {
        PUSHLOG_ERROR("session cache init error");
        return -1;
    }

    if(this->servlist_cache_.init(dbservs, dbbakservs) != 0)
    {
        PUSHLOG_INFO("servlist cache init faild");
    }

    std::vector<meet_you::serv_cfg> serv_cfgs;

    this->servlist_cache_.get_list(0, serv_cfgs);

    if(serv_cfgs.empty())
    {
        PUSHLOG_INFO( "redis server list empty" );

        std::string sids = this->get("session.sids");

        if(sids.empty())
        {
            PUSHLOG_ERROR( "error config" );
            return -1;
        }

        std::vector<std::string> split;
        std::vector<std::string>::const_iterator it;

        boost::algorithm::split (split, sids, boost::algorithm::is_any_of (","));

        for(it = split.begin(); it != split.end(); it++)
        {
            std::cout << *it << std::endl;
            int key = boost::lexical_cast<int>(*it);
            this->server_map_.insert( std::make_pair(key, "server:" + this->get("session."+*it)) );

            PUSHLOG_INFO(RFORMATE("server:" + this->get("session."+*it)));
        }
    }
    else
    {
        for(size_t i = 0; i < serv_cfgs.size(); ++i)
        {
            std::stringstream ss;
            ss << "server:" << serv_cfgs[i].ipaddr << ":" << serv_cfgs[i].port;

            PUSHLOG_INFO(RFORMATE(ss.str()));

            this->server_map_.insert( std::make_pair(serv_cfgs[i].id, ss.str()) );
        }
    }

    return 0;
}

int configwrap::servlist_callback(int type)
{
    PUSHLOG_INFO( "start servlist flush callback" );

    meet_you::SvList serv_cfgs;

    this->svlstCache_.get_list_all_from_cache(0, serv_cfgs);

    if(serv_cfgs.empty())
    {
        PUSHLOG_INFO( "redis server list empty" );
        return -1;
    }
    else
    {
        //clear old server list for heart task
        PUSHLOG_INFO( "clear old server list for heart task");
        this->server_map_.clear();

        for(size_t i = 0; i < serv_cfgs.size(); ++i)
        {
            std::stringstream ss;
            ss << "server:" << serv_cfgs[i].ipaddr << ":" << serv_cfgs[i].port;

            PUSHLOG_INFO(RFORMATE(ss.str()));

            this->server_map_.insert( std::make_pair(serv_cfgs[i].id, ss.str()) );
            //this->server_map_[serv_cfgs[i].id] = ss.str();
        }
    }

    std::map<int, std::string>::iterator it = this->server_map_.begin();
    for(; it != this->server_map_.end(); ++it)
    {
        std::string id = it->second;

        int connect_count = this->connect_count_;
        for(int i = 0; i < connect_count; ++i )
        {
            std::stringstream ss;
            ss << id << ":" << i;
            boost::shared_ptr<session_base> spt = __PEER_MAP_CLIENT::get_mutable_instance().fix_get(ss.str());

            if(!spt)
            {
                std::vector<std::string> split;
                boost::algorithm::split (split, id, boost::algorithm::is_any_of (":"));

                if(split.size() != 3)
                {
                    PUSHLOG_INFO(RFORMATE("error connection info:" + split[1] + ":" + split[2]));
                    continue;
                }

                PUSHLOG_INFO(RFORMATE("start connect to server:" + ss.str()));

                boost::asio::io_service& io = __IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service();
                tcp::resolver resolver(io);
                tcp::resolver::query query(split[1].c_str(), split[2].c_str());
                tcp::resolver::iterator iterator = resolver.resolve(query);

                std::string server_id = split[0] + ":" + split[1] + ":" + split[2];

                boost::shared_ptr<peer_handle_client> c(new peer_handle_client(io, iterator, server_id, ss.str()));
                c->registe();
            }
        }
    }

    PUSHLOG_INFO( RFORMATE("end servlist flush callback" ));

    return 0;
}

int configwrap::init_mongo_hosts()
{
    std::string hosts = this->pt_.get<std::string>("mongo.hosts");

    std::vector<std::string> hosts_v;
    boost::algorithm::split (hosts_v, hosts, boost::algorithm::is_any_of (","));

    for(size_t i = 0; i < hosts_v.size(); ++i)
    {
        std::string ipport = hosts_v[i];
        std::vector<std::string> ipport_v;

        boost::algorithm::split (ipport_v, ipport, boost::algorithm::is_any_of (":"));

        if(ipport_v.size() == 2)
        {
            hosts_.push_back(std::make_pair(ipport_v[0], boost::lexical_cast<int>(ipport_v[1])));
        }
    }

    return 0;
}

int configwrap::init(boost::filesystem::path const& path)
{
    if(!boost::filesystem::exists(path))
    {
        PUSHLOG_ERROR("config not exsit" );
        return -1;
    }

    this->path_ = path.string();

    boost::property_tree::ini_parser::read_ini(path.string(), this->pt_);

    this->port_ = this->pt_.get<int>("session.port");
    this->thread_cout_ = this->pt_.get<int>("session.task_thread_count");
    this->io_pool_size_ = this->pt_.get<int>("session.io_pool_size");

    if(this->port_ == 0)
    {
        PUSHLOG_ERROR( "config error port" );
        return -1;
    }

    this->connect_count_ = this->pt_.get<int>("session.connect_count");
    this->heart_second_ = this->pt_.get<int>("session.heart_second");

    this->reply_domain_ = this->pt_.get<std::string>("session.reply_domain");
    this->post_topic_domain_ = this->pt_.get<std::string>("session.post_topic_domain");

    this->robot_domain_ = this->pt_.get<std::string>("session.robot_domain");

    this->task_queue_max_ = this->pt_.get<unsigned int>("session.task_queue_max");

    int zookeeper_support = this->pt_.get<unsigned int>("session.zookeeper_support", 0);

    python_root_ = this->pt_.get<std::string>("session.python_root");

    init_mongo_hosts();

#ifdef KAFKAVERSION
    return 0;
#else
    if(zookeeper_support == 1)
        return init_zookeeper();
#endif // KAFKAVERSION

    return 0;
}

std::string configwrap::get(std::string const& path)
{
    return this->pt_.get<std::string>(path);
}

void configwrap::init_server_connection()
{
    try
    {
        std::map<int, std::string>::iterator it;

        for(it=this->server_map_.begin(); it != this->server_map_.end(); it++)
        {
            for(int i = 0; i < this->connect_count_; ++i)
            {
                std::string addrs = it->second;

                std::vector<std::string> split;
                boost::algorithm::split (split, addrs, boost::algorithm::is_any_of (":"));

                if(split.size() != 3)
                {
                    PUSHLOG_INFO(RFORMATE("error connection info:" + split[1] + ":" + split[2]));
                    continue;
                }

                PUSHLOG_INFO(RFORMATE("start connect to server:" + split[1] + ":" + split[2]));

                boost::asio::io_service& io = __IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service();
                tcp::resolver resolver(io);
                tcp::resolver::query query(split[1].c_str(), split[2].c_str());
                tcp::resolver::iterator iterator = resolver.resolve(query);

                boost::shared_ptr<peer_handle_client> c(new peer_handle_client(io, iterator));
                c->registe();
            }
        }
    }
    catch (std::exception& e)
    {
        std::stringstream ss;
        ss << "Exception: " << e.what();
        PUSHLOG_ERROR( ss.str() );
    }
}

void configwrap::get_session(std::string const& cid, std::map<std::string, meet_you::sess_t> & sesss)
{
    this->sess_cache_.get_session(cid, sesss);
}

}
