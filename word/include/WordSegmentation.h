#ifndef WORDSEGMENTATION_H
#define WORDSEGMENTATION_H

#include "cppjieba/Jieba.hpp"

#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
using std::string;

namespace SearchEngine
{
    //最大概率法(MPSegment: Max Probability)分词所使用的词典路径)
    const char* const DICT_PATH = "/Volumes/MYwork/code/my_github/project-learning/search_engine/offline/word/include/cppjieba/dict/jieba.dict.utf8";
    //隐式马尔科夫模型(HMMSegment: Hidden Markov Model)分词所使用的词典路径
    const char* const HMM_PATH = "/Volumes/MYwork/code/my_github/project-learning/search_engine/offline/word/include/cppjieba/dict/hmm_model.utf8";
    //用户自定义词典路径
    const char* const USER_DICT_PATH = "/Volumes/MYwork/code/my_github/project-learning/search_engine/offline/word/include/cppjieba/dict/user.dict.utf8";
    //IDF路径
    const char* const IDF_PATH = "/Volumes/MYwork/code/my_github/project-learning/search_engine/offline/word/include/cppjieba/dict/idf.utf8";
    //停用词路径
    const char * const STOP_WORD_PATH = "/Volumes/MYwork/code/my_github/project-learning/search_engine/offline/word/include/cppjieba/dict/stop_words.utf8";

    class WordSegmentation {    //使用Jieba分词库进行分词
    public:
        WordSegmentation()
            :jieba_(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)
        {
            // // 尝试分词一个简单字符串来检查初始化是否成功
            // vector<string> words;
            // jieba_.Cut("测试", words);
            // if (words.empty()) {
            //     std::cerr << "Jieba init failed!" << std::endl;
            //     exit(1);
            // }
        }
        vector<string> operator()(const string str);
        void CutSmall(const string &str, vector<string>& vec, size_t sz);
    private:
        cppjieba::Jieba jieba_;
    };
}

#endif //WORDSEGMENTATION_H
