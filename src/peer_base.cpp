#include "peer_base.h"

#include "io_service_provide.h"

namespace push_logic
{

peer_base::peer_base()
    : io_service_(__IO_SERVICE_PROVIDE::get_mutable_instance().get_io_service())
    //, signals_(io_service)
{
}

peer_base::~peer_base()
{
}

void peer_base::stop()
{
    std::cout << "signals get" << std::endl;
}

void peer_base::dump_stack(std::stringstream & log)
{
    void *bufs[100];
    int n = backtrace(bufs, 100);
    char **infos = backtrace_symbols(bufs, n);
    if(!infos) exit(1);
    log << "Frame info:" << std::endl;
    char cmd[512];
    int len = snprintf(cmd, sizeof(cmd),
                       "addr2line -ifC -e /home/push_logic_server/push_logic_server");
    char *p = cmd + len;
    size_t s = sizeof(cmd) - len;
    for(int i = 0; i < n; ++i)
    {
        log << infos[i] << std::endl;
        if(s > 0)
        {
            len = snprintf(p, s, " %p", bufs[i]);
            p += len;
            s -= len;
        }
    }
    log << "src info:" << std::endl;
    FILE *fp;
    char buf[128];
    if((fp = popen(cmd, "r")))
    {
        while(fgets(buf, sizeof(buf), fp))
            log << buf;
        pclose(fp);
    }
    free(infos);
    // same as:
    //backtrace_symbols_fd(bufs, n, STDOUT_FILENO);
}

}
