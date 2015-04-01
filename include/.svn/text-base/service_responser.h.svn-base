#ifndef SERVICE_RESPONSER_H
#define SERVICE_RESPONSER_H

#include "task_responser.h"

namespace push_logic
{

class service_responser : public task_responser
{
    public:
        service_responser();
        virtual ~service_responser();

        virtual int response(std::string const& id, meet_you::head const& h, std::string const& data);
        virtual std::string parser(std::string const& data);
    protected:
    private:
};

}

#endif // SERVICE_RESPONSER_H
