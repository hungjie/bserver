#ifndef PYTHON_FUNC_SERVICE_RESPONSER_H
#define PYTHON_FUNC_SERVICE_RESPONSER_H

#include "task_responser.h"

namespace push_logic
{

class python_func_service_responser : public task_responser
{
    public:
        python_func_service_responser(std::string const& sn);
        virtual ~python_func_service_responser();

        virtual int response(std::string const& id, meet_you::head const& h, std::string const& data);
        virtual net_service::service_response_result parser(std::string const& data, std::string & response);

    protected:
        void replace_all(std::string & str, std::string const &src, std::string const &dst)
        {
            std::string::size_type pos = 0;

            std::string::size_type srclen = src.size ();

            std::string::size_type dstlen = dst.size ();

            while ((pos = str.find (src, pos)) != std::string::npos)
            {
                str.replace (pos, srclen, dst);

                pos += dstlen;
            }
        }

    private:
        std::string sn_;
};

}
#endif // PYTHON_FUNC_SERVICE_RESPONSER_H
