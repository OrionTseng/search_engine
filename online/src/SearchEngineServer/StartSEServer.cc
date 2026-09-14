#include "../../include/Configuration.h"
#include "../../include/KeyRecommander/Dictionary.h"
#include "../../include/WebPageSearcher/WebPageQuery.h"
#include "../../include/SearchEngineServer/SearchEngineServer.h"
#include "../../include/LRUCache/CacheManager.h"
#include "../../include/TimerManager/TimerFd.h"

#include <thread>

using std::string;
using std::thread;

using namespace SearchEngine;

int main(int argc, char *argv[]) 
{
    map<string, string> configs = Configuration::getInstance()->getConfigs();
    // 获取配置模块从conf/server.conf配置文件中读取数据，包括ip、port、线程数等

    int threadNum = stoi(configs["threadNum"]);
    // 线程池线程数
    int queSize   = stoi(configs["queSize"]);
    // 队列大小
    string ip     = configs["ip"];
    // 服务器IP
    size_t port   = stoi(configs["port"]);
    // 服务器端口

    Dictionary::getInstance();
    // 加载关键字推荐字典文件中的数据加载到内存中
    // 数据结构包括：词典、索引、数据库（idMap）
    
    WebPageQuery::getWebQueryPtr();
    // 加载网页搜索对应数据文件加载到内存中
    // 数据结构包括：网页库、网页偏移库、倒排索引

    cout << "loading server ..." << endl;

    CacheManager *p = CacheManager::getInstance();
    // 预加载缓存的目的为了将CacheManager中的数据成员进行初始化

    // 定时器定时执行更新函数updateCache、updateCache2，更新缓存
    // 数据同步逻辑，一般为了查询前将缓存数据同步
    TimerFd tf1(2, 3, std::bind(&CacheManager::updateCache, p));
    thread th1(&TimerFd::start, &tf1);

    TimerFd tf2(3, 5, std::bind(&CacheManager::updateCache2, p));
    thread th2(&TimerFd::start, &tf2);

    // 启动搜索引擎服务器的代码，处理用户的查询请求
    SearchEngineServer seServer(threadNum, queSize, ip, port);
    seServer.start();
    return 0;
}

