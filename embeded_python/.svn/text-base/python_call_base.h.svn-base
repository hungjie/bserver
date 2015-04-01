#ifndef PYTHON_CALL_BASE_H
#define PYTHON_CALL_BASE_H

#include <typeinfo>

#include "python_status_ensure.h"

namespace embeded_python
{

class python_call_base
{
public:
    python_call_base();
    virtual ~python_call_base();

    template<typename return_type>
    return_type call(const string& mod_name, const string& func)
    {
        PYTHON_API_ENTER

        return ffpython_.call<return_type>(mod_name.c_str(), func.c_str());
    }

    template<typename return_type>
    return_type call(std::string const& path, const string& mod_name, const string& func)
    {
        PYTHON_API_ENTER

        add_path(path);

        return ffpython_.call<return_type>(mod_name.c_str(), func.c_str());
    }
/*
    template <>
    void call<void>(std::string const& path, const string& mod_name, const string& func)
    {
        PYTHON_API_ENTER

        add_path(path);

        ffpython_.call<void>(mod_name.c_str(), func.c_str());
    }
*/
    template <typename return_type, typename arg1>
    return_type call(std::string const& path, const string& mod_name, const string& func, arg1 const& a1)
    {
        PYTHON_API_ENTER

        add_path(path);

        return ffpython_.call<return_type>(mod_name.c_str(), func.c_str(), a1);
    }

    template <typename return_type, typename arg1, typename arg2>
    return_type call(std::string const& path, const string& mod_name, const string& func, arg1 const& a1, arg2 const& a2)
    {
        PYTHON_API_ENTER

        add_path(path);

        return ffpython_.call<return_type>(mod_name.c_str(), func.c_str(), a1, a2);
    }

    template <typename return_type, typename arg1, typename arg2, typename arg3>
    return_type call(std::string const& path, const string& mod_name, const string& func, arg1 const& a1, arg2 const& a2, arg3 const& a3)
    {
        PYTHON_API_ENTER

        add_path(path);

        return ffpython_.call<return_type>(mod_name.c_str(), func.c_str(), a1, a2, a3);
    }

    template <typename return_type, typename arg1, typename arg2, typename arg3, typename arg4>
    return_type call(std::string const& path, const string& mod_name, const string& func, arg1 const& a1, arg2 const& a2, arg3 const& a3, arg4 const& a4)
    {
        PYTHON_API_ENTER

        add_path(path);

        return ffpython_.call<return_type>(mod_name.c_str(), func.c_str(), a1, a2, a3, a4);
    }

protected:
    void add_path(std::string const& path);

    ffpython_t ffpython_;
};

}
#endif // PYTHON_CALL_BASE_H
