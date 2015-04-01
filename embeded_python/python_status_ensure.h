#ifndef PYTHON_STATUS_ENSURE_H
#define PYTHON_STATUS_ENSURE_H

#include "ffpython.h"

namespace embeded_python
{

class python_status_ensure
{
public:
    python_status_ensure();
    virtual ~python_status_ensure();
protected:
private:
    PyGILState_STATE state_;
};

#define PYTHON_API_ENTER python_status_ensure python_status_ensure_;

class AllowThreadThrough
{
public:

    AllowThreadThrough()
    {
        state_ = PyEval_SaveThread();
    }

    ~AllowThreadThrough()
    {
        PyEval_RestoreThread ( state_ );
    }

private:

    PyThreadState* state_;
};

#define PYTHON_THREAD_ENTER AllowThreadThrough AllowThreadThrough_;

}
#endif // PYTHON_STATUS_ENSURE_H
