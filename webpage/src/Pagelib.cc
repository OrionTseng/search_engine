#include "../include/Pagelib.h"
#include "../include/tinyxml2.h"
#include "../include/DirScanner.h"
#include "../include/Configuration.h"
#include "../include/simhash/Simhasher.hpp"

#include <math.h>
#include <fstream>
#include <regex>
#include <iostream>
#include <fstream>
#include <map>

using std::cout;
using std::endl;
using std::ofstream;
using std::map;
using namespace tinyxml2;

namespace SearchEngine{
    PageLib::PageLib()
    :config_(Configuration::getInstance("conf/page.conf"))
    ,jieba_(dict_.getJieba()){
        //进行rss解析，将处理好的结果存入vector中
        create();
        cout << "网页信息采集成功，服务器处理中..." << endl;
        //网页去重
        PageDeDuplication();
        cout << "网页去重已完成" << endl;
        //建立倒排索引
        handleInvertIndex();
        cout << "倒排索引建立成功" << endl;
        //持久化网页库、偏移库、倒排索引库
        store();
        cout << "任务结束，此次创建网页信息完成" << endl;
    }

    //解析rss
    void PageLib::create(){
        //创建目录扫描对象，该类重载了函数调用运算符
        DirScanner dir;

        //通过键值对获取存储的xml文件路径
        dir(config_->getConfigs().find("XMLDIR")->second); //getConfigs函数返回类型是一个map

        //解析xml文件
        for(string& filename : dir.files()){
            XMLDocument doc;
            doc.LoadFile(filename.c_str());
            if(doc.ErrorID()){
                perror("loadFile fail");
                return ;
            }
            XMLElement* itemNode = doc.FirstChildElement("rss")->FirstChildElement("channel")->FirstChildElement("item");
            while(itemNode){
                RSSItem rssItem;
                std::regex reg("<[^>]+");   //通用正则表达式

                //每次都在判断，防止标题，连接，描述信息为空
                if(itemNode->FirstChildElement("title")){
                    string title = itemNode->FirstChildElement("title")->GetText();
                    title = regex_replace(title, reg, "");
                    rssItem._title = title;
                }else{
                    rssItem._title = "这篇文档没有标题";
                }
                string link = itemNode->FirstChildElement("link")->GetText();
                rssItem._link = link;

                if(itemNode->FirstChildElement("description")){
                    string description = itemNode->FirstChildElement("description")->GetText();
                    description = regex_replace(description, reg, "");
                    rssItem._description = description;
                }else{
                    rssItem._description = "这篇文档没有内容";
                }
                //将网页信息一篇一篇的存储在vector容器中
                rss_.push_back(rssItem);
                itemNode = itemNode->NextSiblingElement("item");
            }
        }
    }
     
    //网页去重
    void PageLib::PageDeDuplication(){
        size_t topN = 20;

        //初始化simhash对象，使用simhash进行去重
        simhash::Simhasher simhasher(DICT_PATH, HMM_PATH, IDF_PATH, STOP_WORD_PATH);
        vector<pair<int, uint64_t> > Sim;

        //调试信息，可以查看去重前的文章数目
        //cout << "去重前的文章数目 = " << rss_.size() << endl;

        for(size_t i = 0; i < rss_.size(); ++i){
            uint64_t u = 0;
            //make函数的作用：根据前topN个权重计算降维后的Simhash值(唯一的64位的指纹)
            simhasher.make(rss_[i]._description, topN, u);
            //将当前文章的指纹和id号存入vector中
            Sim.push_back(std::make_pair(i, u));
        }

        //将文章的Simhash值进行两两比较（使用海明距离比较，指纹的位数比较，64位中有几位不同）
        //进行异或运算，值越小越相似，设置一个set<int>容器，存放要删除的文章，删除短的保留长的
        //遍历Sim，找到set中的文章id，达到去重目的
        set<int> tmp;
        for(size_t it = 0; it < Sim.size(); ++it){
            int id1 = Sim[it].first;
            int sz1 = rss_[id1]._description.size();
            for(size_t it2 = (it + 1); it2 < Sim.size(); ++it2){
                //isEqual函数的作用：改写第三方库的海明距离函数，返回距离
                //判断两篇文章的海明距离
                if(simhash::Simhasher::isEqual(Sim[it].second, Sim[it2].second)){
                    //判断文章长短
                    int sz2 = rss_[Sim[it2].first]._description.size();
                    if(sz1 >= sz2 ) 
                    {
                        tmp.insert(Sim[it2].first);
                    }
                    else
                    {
                        tmp.insert(id1);
                        break;
                    }
                }
            }
        }
        for(auto num : tmp){
            num -= 0;
            int i = 0;
            for(auto it = rss_.begin(); it != rss_.end(); ++it, ++i)
            {
                if(num == i)
                {
                    rss_.erase(it);//从rss_中删除重复的文章
                    break;
                }
            }
        }
        DOCICNUM_ = rss_.size();

        //调试信息，可以查看去重后的文章数目
        //cout << "去重后的文章数目 = " << rss_.size() << endl;
    }

    //建立倒排索引库
    void PageLib::handleInvertIndex(){
        //统计每篇文章的词频
        unordered_map<string, unordered_map<int, double>> tf;
        int i = 0;

        //遍历去重后的文章，然后将描述信息分词，将单词，文章id与词频存在tf中
        for(auto it = rss_.begin(); it != rss_.end(); ++it){
            //让docid从1开始
            int docid = ++i;
            string doc = it->_description;
            vector<string> results = jieba_(doc);
            for(string& word : results){
                auto& isExit = tf[word];
                //判断在当前文章里，该单词有没有出现过，出现过词频+1，未出现过，作为新值插入，词频赋值为1
                if(isExit[docid])
                {
                    ++isExit[docid];
                }
                else 
                {
                    isExit[docid] = 1;
                }
            }
        }

        //计算单词在每篇文章中的权重，将其放在倒排索引表中
        //计算权重使用TF-IDF算法
        //原始数据：hellp （id:1, 频率：3） （id:2, 频率：10）
        unordered_map<int, double> docW;    //存放文章id和权重平方和
        for(auto it = tf.begin(); it != tf.end(); ++it){    //遍历tf中的每个单词
            string word = it->first;    //单词
            size_t DF = it->second.size();  //该词对应的文章数量
            for(auto num = it->second.begin(); num != it->second.end(); ++num){
                //文章id
                int docid = num->first; 
                //该词在该文章中的词频
                int TF = num->second;
                //逆文档频率的计算公式：log(文章总数/包含该单词的文档数+1)，+1避免分母为0,包含该单词的文档数可用频率代替
                //衡量词的普遍重要性
                double IDF = log(1.0 * DOCICNUM_ /(DF + 1))/log(2); 
                //TF_IDF权重计算公式：w = TF * IDF
                double w = TF * IDF;
                //将tf中对应的词频替换为权重
                num->second = w;
                //收集当前docid每个词语的权重的平方和，为后续归一化处理做准备（归一化处理解决文档长度差异导致的权重偏差问题）
                docW[docid] += w * w;
            }
        }

        //已得到 TF-IDF处理后：hellp （id:1, 权重：w） （id:2, 权重：w）
        //进行归一化处理
        for(auto it = tf.begin(); it != tf.end(); ++ it) {
            string word = it->first;
            //遍历该词对应的文章
            for(auto elem = it->second.begin(); elem != it->second.end(); ++elem){
                //文章id
                int docid = elem->first;
                //该文章id对应的权重平方
                double sumW2 = docW[docid];
                //该文章id对应的权重
                double w = elem->second;
                //归一化公式：w’ = w/sqrt(w1^2 +w2^2+w3^2….+wn^2)
                //括号里是该文章中每个词语的权重的平方，w是目标词的权重
                double FinW = w / sqrt(sumW2);
                //将单词，文章id，归一化权重存放在倒排索引库中
                invertIndex_[word].insert(std::make_pair(docid, FinW));
            }
        }
    }

    //持久化网页库、偏移库、倒排索引库
    void PageLib::store(){
            /* _dict.store(); */
        ofstream ofs("data/ripepage.dat"); //网页库
        if(!ofs)
        { 
            perror("ofs fail");
            return;
        }
        ofstream ofs1("data/offset.dat"); //网页偏移库
        if(!ofs1) 
        { 
            perror("ofs fail");
            return;
        }
        for(size_t i = 0; i != rss_.size(); ++i)
        {
            size_t idx = i + 1;
            ofs1 << idx << " ";//文章id
            size_t  beginpos = ofs.tellp();
            ofs1 << beginpos << " ";//文章的起始位置
            ofs << "<doc>"<<endl;
            ofs << "\t<docid>" << i + 1 << "</docid>" <<endl;
            ofs << "\t<title>" << rss_[i]._title << "</title>" <<endl;
            ofs << "\t<url>" << rss_[i]._link << "</url>" <<endl;
            ofs << "\t<description>" << rss_[i]._description<< "</description>" <<endl;
            ofs << "</doc>" <<endl;
            size_t endpos = ofs.tellp();//文章的终止位置
            size_t len = endpos - beginpos;//文章的长度（包含有<doc><docid><title>这些）
            ofs1 << len << "\n";
        }
        cout << "网页库和网页偏移库持久化完成" << endl;

        ofstream ofs3;
        ofs3.open("data/invertIndex.dat");
        //遍历倒排索引表，将其存放在文件中
        for(auto & it : invertIndex_)
        {
            ofs3 << it.first << " ";
            for(auto it2 = it.second.begin(); it2 != it.second.end(); ++it2) 
            {
                ofs3 << it2->first << " " << it2->second << " " ;
            }
            ofs3 << endl;
        }
        cout << "倒排索引库持久化完成" << endl;
        ofs1.close();
        ofs.close();
        ofs3.close();
    }

    // void PageLib::handleWordMap(){

    // }

    // void PageLib::handleTopK(){

    // }
   
}