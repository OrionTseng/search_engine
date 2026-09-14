#ifndef DICTPRODUCER_H
#define DICTPRODUCER_H 

#include "WordSegmentation.h"

namespace SearchEngine{
    class DictProducer {    //词典生成类
    public:
        DictProducer():jieba_(){}

        ~DictProducer(){}

        WordSegmentation& getJieba(){
            return jieba_;
        }

    private:
        WordSegmentation jieba_;
    };
}
#endif