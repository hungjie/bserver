#include "python_call_base.h"

namespace embeded_python
{

python_call_base::python_call_base()
{
    //ctor
}

python_call_base::~python_call_base()
{
    //dtor
}

void python_call_base::add_path(const std::string& path)
{
    ffpython_t::add_path(path.c_str());
}

}
