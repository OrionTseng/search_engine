//
// Created by Tseng on 25-8-4.
//

#include "../include/DictProducer.h"

#include <fstream>

using std::ifstream;
using std::ofstream;

namespace SearchEngine
{
    DictProducer::DictProducer()
        :jieba_(), dir_()
    {
        //打开目录
        dir_("data/CN/art");
        for (auto& fileName : dir_.files())
        {
            //将所有文件路径存储在DictProduce的vector容器中
            file_.push_back(fileName);
        }
        //创建中文词典库
        createCnDict();
        //创建英文词典库
        createEnDict();
        //将词典库、索引库，id库存起来
        store();
    }

    DictProducer::~DictProducer()
    {

    }

    //创建中文词典库
    void DictProducer::createCnDict()
    {
        for (string& fileName : file_)
        {
            ifstream ifs;
            openFile(ifs, fileName);
            string line;
            while (ifs >> line)
            {
                //使用cppjieba对象进行分词
                vector<string> results = jieba_(line);
                for (string& elem : results)
                {
                    //汉字组，判断是不是中文，中文一个字符是3个字节（UTF-8）
                    if (elem.size() % CHSIZE == 0)
                    {
                        int& isExit = dict_[elem];  //查找是否有词频数值
                        //查找dict_[elem]，如果在就返回引用，不在就插入键值elem，并将dict_[elem]初始化为0
                        if (isExit) //有词频++
                        {
                            ++isExit;   //词频
                        }
                        else  //没有则代表是新词
                        {
                            isExit = 1; //词频设为1
                            //单词（或者短语）第一次出现的时候，会将对应的序号与单词（或者短语）存起来
                            //单词的序号最大值其实就是单词的总数
                            size_t id = dict_.size();   //词项id为当前词典的大小，每插入一个词典大小就会+1，可作为词项id
                            idMap_[id] = elem;  //id对应词项
                            insertIndex(elem, id);  //插入倒排索引
                        }
                    }
                }
            }
            ifs.close();
        }
    }

    void DictProducer::insertIndex(const string& elem, size_t id)
    {
        //简历倒排索引,有词找到id，再由id找到词
        vector<string> results;
        //细粒度分词，将一个复合词拆分成一个单位的词，存入results
        jieba_.CutSmall(elem, results, 1);
        for (string& it : results)
        {
            //若该词在index_中存在，插入id，set自动去重，自动升序排序
            //若该词没有在index_中，则插入键值对it，将其值初始化为空的set<int>
            index_[it].insert(id);
        }
    }

    void DictProducer::store()
    {
        ofstream ofs_dict;  //将词典+词频，存入dict.dat文件中
        inputFile(ofs_dict, "data/dict.dat");
        for (auto it = dict_.begin(); it != dict_.end(); ++it)
        {
            ofs_dict << it->first << " " << it->second << endl;
        }

        ofstream ofs_index; //存储倒排索引数据
        inputFile(ofs_index, "data/index.dat");
        for (auto it = index_.begin(); it != index_.end(); ++it)
        {
            ofs_index << it->first << " ";
            for (auto& st : it->second)
            {
                ofs_index << st << " ";
            }
            ofs_index << endl;
        }

        ofstream ofs_idMap;
        inputFile(ofs_idMap, "data/idMap.dat");
        for (auto it = idMap_.begin(); it != idMap_.end(); ++it)
        {
            ofs_idMap << it->first << " " << it->second << endl;
        }

        ofs_idMap.close();
        ofs_index.close();
        ofs_dict.close();
    }

    //创建英文词典库
    void DictProducer::createEnDict()
    {
        //英文就一个文件，不需要遍历
        ifstream ifs;
        openFile(ifs, "data/EN/english.txt");
        string line;
        while (getline(ifs, line))
        {
            processLine(line);  //大小写转换
            istringstream iss(line);
            string word;
            while (iss >> word)
            {
                //英文，一个字符就是一个字节
                int& isExit = dict_[word];
                if (isExit)
                {
                    //有该词则词频++
                    ++isExit;
                }
                else
                {
                    //没有该词则词频赋值为1
                    isExit = 1;
                    //将词典大小作为词项的id号
                    size_t id = dict_.size();
                    idMap_[id] = word;  //id对应词项
                    insertEnIndex(word, id);  //插入倒排索引
                }
            }
        }
        ifs.close();
    }

    void DictProducer::insertEnIndex(const string& word, size_t id)
    {
        //建立倒排索引
        for (size_t i = 0; i < word.size(); ++i)
        {
            //把单词拆分成字母
            string ch(1, word[i]);
            //将字母放入倒排索引
            index_[ch].insert(id);  //set默认自动去重，所以不会用重复的单词绑定到倒排索引中
        }
    }

    void DictProducer::processLine(string& line)
    {
        for (auto& elem : line)
        {
            if (!isalpha(elem)) //检查是否为字母
            {
                elem = ' '; //把非字母替换成空格
            }
            else if (isupper(elem)) //判断字符是否为大写字母
            {
                elem = tolower(elem); //将大写字母转换成小写字母
            }
        }
    }

    void DictProducer::openFile(ifstream& ifs, const string& fileName)
    {
        //输入文件，用于读取文件内容
        ifs.open(fileName);
        cout << "open >>>" << fileName << endl;
        if (!ifs)
        {
            perror("open file failed in Dict ifs");
            return;
        }
    }

    void DictProducer::inputFile(ofstream& ofs, const string& fileName)
    {
        //输出文件，写数据到问价中
        ofs.open(fileName);
        if (!ofs)
        {
            perror("open file failed in Dict ofs");
            return;
        }
    }



}