#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "DisallowCopyAssign.h"

#include <string>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>

using std::string;
using std::map;
using std::set;
using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;
using std::pair;
using std::cerr;

namespace SearchEngine{
    //单例模式
    class Configuration{
    public:
        static Configuration* getInstance(const char* filePath);
        //获取存放配置文件的位置
        map<string, string>& getConfigs();

    private:
        //私有化构造函数
        explicit Configuration(const string& filePath);
        //私有化析构函数
        ~Configuration();

    private:
        //禁止赋值和复制
        DISALLOWCOPYANDASSIGN(Configuration);

        static Configuration* pInstance_; 
        string configFilePath_; //配置文件路径
        map<string, string> configs_;   //配置文件内容
    };

    //饱汉模式，离线部分不用考虑多线程安全
    Configuration* Configuration::pInstance_ = nullptr;

    Configuration* Configuration::getInstance(const char* filePath){
        //静态成员函数
        if(!pInstance_){
            pInstance_ = new Configuration(string(filePath));
        }
        return pInstance_;
    }

    Configuration::Configuration(const string& filePath)
    : configFilePath_(filePath){
        //初始化配置文件部分
        ifstream ifs;
        ifs.open(configFilePath_);
        if(!ifs.good()){
            std::cerr << "open en_file_dir faile" << endl;
            return;
        }

        string line, key, val;
        while(getline(ifs, line), !ifs.eof()){
            istringstream iss(line);
            iss >> key;
            iss >> val;
            pair<string, string> record(key, val);
            configs_.insert(record);
        }
        ifs.close();
    }

    map<string, string>& Configuration::getConfigs(){
        return configs_;
    }
}

#endif