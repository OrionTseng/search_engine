#include "../include/WordSegmentation.h"

vector<string> SearchEngine::WordSegmentation::operator()(const string str)
{
    vector<string> words;
    //完整段落
    jieba_.CutAll(str, words); 
    return words;
}

void SearchEngine::WordSegmentation::CutSmall(const string& str, vector<string>& vec, size_t sz)
{
    jieba_.CutSmall(str, vec, sz);
}

//测试
// int main(){
//     const std::string text = "今天天气不错，我们一起出去玩吧！";
//     std::vector<std::string> vec;
//     SearchEngine::WordSegmentation obj;
//     obj.CutSmall(text, vec, 2);

//     // 检查分词结果
//     if (vec.empty()) {
//         std::cerr << "Segmentation failed!" << std::endl;
//         return 1;
//     }

//     for (auto word : vec) {
//         std::cout << word << "/";
//     }

//     std::cout << std::endl;

//     return 0;
// }