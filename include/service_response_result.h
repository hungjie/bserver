#ifndef SERVICE_RESPONSE_RESULT_H
#define SERVICE_RESPONSE_RESULT_H

namespace net_service
{

class service_response_result
{
public:
    service_response_result();
    virtual ~service_response_result();

    int status() const
    {
        return status_;
    }
    void status(int status)
    {
        status_ = status;
    }

    void async(bool async)
    {
        async_ = async;
    }
    bool async() const
    {
        return async_;
    }

    void sync(bool sync)
    {
        async_ = !sync;
    }
    bool sync() const
    {
        return !async_;
    }

protected:
    int status_;
    bool async_;
};

}
#endif // SERVICE_RESPONSE_RESULT_H
