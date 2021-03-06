#include "peer_handle_map.h"

#include "log.h"

#include "configwrap.h"

namespace push_logic
{

boost::shared_ptr<session_base> peer_handle_map::null_session_ptr_;

peer_handle_map::peer_handle_map()
{
    //ctor
}

peer_handle_map::~peer_handle_map()
{
    //dtor
}

int peer_handle_map::find_matched(std::string const& id)
{
    std::map< std::string, boost::shared_ptr<session_base> >::iterator it = this->map_.begin();

    int count = 0;
    for(; it != this->map_.end(); ++it)
    {
        if( it->first.find(id) != std::string::npos )
        {
            count++;
        }
    }

    return count;
}

std::string peer_handle_map::set(std::string const& id, boost::shared_ptr<session_base> session_ptr)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    std::map< std::string, boost::shared_ptr<session_base> >::iterator it = this->map_.find(id);

    if(it != this->map_.end())
    {
        this->map_.erase(it);
        //it->second->close();
    }

    this->map_[id] = session_ptr;

    return id;
}

boost::shared_ptr<session_base> peer_handle_map::get(std::string const& id)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    std::map< std::string, boost::shared_ptr<session_base> >::iterator it;
    it = this->map_.find(id);

    if(it != this->map_.end())
    {
        return this->map_[id];
    }

    return this->null_session_ptr_;
}

void peer_handle_map::erase(std::string const& id)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    this->map_.erase(id);
}


std::string peer_handle_map_for_client::set(std::string const& id, boost::shared_ptr<session_base> session_ptr)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    int count = this->find_matched(id);

    std::stringstream ss;
    ss << id << ":" << count;

    if(count >= __CONFIGWRAP::get_const_instance().connect_count())
    {
        PUSHLOG_INFO("peer map fill full " + RFORMATE(ss.str()));
        return "";
    }

    if( this->map_[ss.str()] )
    {
        for(int i = 0; i < count; ++i)
        {
            std::stringstream ss_tmp;
            ss_tmp << id << ":" << i;

            std::string str_tmp = ss_tmp.str();

            if(!this->map_[str_tmp])
            {
                PUSHLOG_INFO("start save session for : " + RFORMATE(ss_tmp.str()));

                this->map_[str_tmp] = session_ptr;
                client_list_[id].push_back(ss.str());

                return str_tmp;
            }
        }

        return "";
    }
    else
    {
        PUSHLOG_INFO("start save session for : " + RFORMATE(ss.str()));
        this->map_[ss.str()] = session_ptr;
    }

    client_list_[id].push_back(ss.str());

    return ss.str();
}

int peer_handle_map_for_client::fix_set(std::string const& server_id, std::string const& id, boost::shared_ptr<session_base> session_ptr)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    if(server_id.empty() || id.empty())
    {
        PUSHLOG_ERROR("fix_set, empty server_id or id " + server_id + " " + id);
        return -1;
    }

    this->map_[id] = session_ptr;

    std::vector<std::string>::iterator it = client_list_[server_id].begin();
    for(; it != client_list_[server_id].end(); ++it)
    {
        if(*it == id)
        {
            client_list_[server_id].erase(it);
            break;
        }
    }

    client_list_[server_id].push_back(id);

    return 0;
}

boost::shared_ptr<session_base> peer_handle_map_for_client::get(std::string const& id)
{
    boost::mutex::scoped_lock lock(this->mutex_);

    //int count = this->find_matched(id);
    int count = client_list_[id].size();
    if(count == 0)
    {
        return this->null_session_ptr_;
    }

    int random_num = this->random_num(count);

    std::string ss = client_list_[id][random_num];

    PUSHLOG_TRACE("get client map for session " + RFORMATE("id:" + ss));

    std::map< std::string, boost::shared_ptr<session_base> >::iterator it;
    it = this->map_.find(ss);
    if(it != this->map_.end())
    {
        return this->map_[ss];
    }

    return this->null_session_ptr_;
}

boost::shared_ptr<session_base> peer_handle_map_for_client::fix_get(std::string const& id)
{
    return peer_handle_map::get(id);
}

boost::shared_ptr<session_base> peer_handle_map_for_client::simple_get(std::string const& id)
{
    int count = __CONFIGWRAP::get_const_instance().connect_count();

    std::stringstream ss;
    ss << id << ":" << this->random_num(count);

    std::map< std::string, boost::shared_ptr<session_base> >::iterator it;
    boost::mutex::scoped_lock lock(this->mutex_);
    {
        it = this->map_.find(ss.str());
        if(it != this->map_.end())
        {
            return this->map_[ss.str()];
        }
    }

    return this->null_session_ptr_;
}

void peer_handle_map_for_client::erase(std::string const& id)
{
    std::vector<std::string> split;
    boost::algorithm::split (split, id, boost::algorithm::is_any_of (":"));

    if(split.size() != 4)
    {
        PUSHLOG_WARN("error erase in peer_handle_map_for_client, invalid id.");
        return;
    }

    std::string address = split[0] + ":" + split[1] + ":" + split[2];

    boost::mutex::scoped_lock lock(this->mutex_);
    this->map_.erase(id);

    std::vector<std::string>::iterator it = client_list_[address].begin();
    for(; it != client_list_[address].end(); ++it)
    {
        if(*it == id)
        {
            client_list_[address].erase(it);
            break;
        }
    }
}

void peer_handle_map_for_client::erase(std::string const& server_id, std::string const& id)
{
    boost::mutex::scoped_lock lock(this->mutex_);
    this->map_.erase(id);

    std::vector<std::string>::iterator it = client_list_[server_id].begin();
    for(; it != client_list_[server_id].end(); ++it)
    {
        if(*it == id)
        {
            client_list_[server_id].erase(it);
            break;
        }
    }
}

void peer_handle_map_for_client::close_all()
{
    boost::mutex::scoped_lock lock(this->mutex_);

    std::map< std::string, boost::shared_ptr<session_base> >::iterator it = map_.begin();

    for(; it != map_.end(); )
    {
        map_.erase(it++);
    }
}

}
