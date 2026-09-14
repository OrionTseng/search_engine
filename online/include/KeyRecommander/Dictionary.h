#ifndef __SE_DICTIONARY_H__
#define __SE_DICTIONARY_H__

#include "../DisallowCopyAssign.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <set>

using std::string;
using std::vector;
using std::unordered_map;
using std::set;

namespace SearchEngine 
{

class Dictionary 
{
public:
    static Dictionary *getInstance();
    void destroy();
    
    vector<std::pair<string, int>> doQuery(const string& key);

    //1. 求取一个字符占据的字节数
    size_t nBytesCode(const char ch);
    //2. 求取一个字符串的字符长度
    std::size_t length(const std::string &str);
    //3. 中英文通用的最小编辑距离算法
    //上述的 1，2 都是辅助 3 的算法函数
    int editDistance(const std::string & lhs, const std::string &rhs);
    //4.辅助函数，求三个数的最小值
    //辅助完成最小距离编辑算法，得到最小距离
    int triple_min(const int& a, const int& b, const int& c);
    //实现c++中英文字符串按块切分
    //将输入字符串按字符（或单词）切分，并将结果存储在 words 容器中
    void handleWord(const string& input, vector<string>& words);

	//加载三个库对应的函数
    void loadDict();
    void loadIndex();
    void loadIdMap();

private:
    DISALLOWCOPYANDASSIGN(Dictionary);
    Dictionary() = default; 

private:
    static Dictionary*   _pdict;
	//词典:单词 + 所有文章中频率
    unordered_map<string , int> _dict; 
	//索引:字 + 单词id集合
    unordered_map<string, set<int>> _index;
	//id映射:单词id + 单词
    unordered_map<int, string> _idMap;
};

} // namespace search_engine

#endif // __SE_DICTIONARY_H__
