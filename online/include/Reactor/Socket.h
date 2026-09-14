#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "NonCopyable.h"

namespace SearchEngine
{

class Socket
: NonCopyable
{
public:
    Socket();
    explicit Socket(int fd);
    ~Socket();
    int fd() const;
    void shutDownWrite();

private:
    int _fd;
};

}//end of namespace search_engine
#endif
