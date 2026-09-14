#ifndef PAGELIB_H
#define PAGELIB_H

#include "DictProducer.h"
#include "WordSegmentation.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>

using std::string;
using std::vector;
using std::unordered_map;
using std::set;
using std::pair;
using std::map;

namespace SearchEngine{
    class Configuration;

    //将内存也存起来，加一个数据成员
    struct RSSItem{
        string _title;
        string _link;
        string _description;
    };

    // class Compare{
    //     bool operator()(const pair<int, double>& lhs, const pair<int, double>& rhs){
    //         return lhs.first < rhs.first;
    //     }
    // };

    class PageLib{
    public:
        PageLib();
        ~PageLib(){}

        // unordered_map<int, map<string, int> >& getWordsMao(){
        //     return wordMap_;
        // }

        // unordered_map<int, vector<string> >& get_TopK(){
        //     return topK_;
        // }

    private:
        void create();
        void store();
        void handleInvertIndex();
        // void handleWordMap();
        // void handleTopK();
        void PageDeDuplication();

    private:
        int DOCICNUM_;  //去重后文章的总数
        vector<RSSItem> rss_;   //解析rss，将其存在vector中
        Configuration* config_; //读取配置文件，获取xml文件的路径
        DictProducer dict_;  //创建cppjeba对象
        WordSegmentation& jieba_;   //分词对象的引用
        unordered_map<string, set<pair<int, double> > > invertIndex_;   //倒排索引
        // unordered_map<int, map<string, int> > wordMap_; //未用到
        // unordered_map<int, vector<string>>   topK_; //未用到
    };
}

#endif