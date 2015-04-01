#include "python_status_ensure.h"

//#include "boost/python.hpp"

namespace embeded_python
{


python_status_ensure::python_status_ensure()
{
    state_ = PyGILState_Ensure( );
}

python_status_ensure::~python_status_ensure()
{
    PyGILState_Release ( state_ );
}

}
