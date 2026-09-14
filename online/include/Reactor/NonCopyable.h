#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

namespace SearchEngine
{

//不允许复制或者赋值
class NonCopyable
{
protected:
    NonCopyable()
    {
    }

    ~NonCopyable()
    {

    }

    NonCopyable(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;
};

}//end of namespace search_engine

#endif
