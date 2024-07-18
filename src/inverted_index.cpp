#include "../include/inverted_index.h"
#include "../include/threadPool.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <algorithm>

std::mutex thread_lock;

bool engine::Entry::operator==(const Entry& other) const
{
    return ((other.count == this->count) && (other.doc_id == this->doc_id));
}

bool check_dictionary(const int& number_doc, std::vector<engine::Entry>& list_dictionary)
{
    for(auto& it: list_dictionary)
    {
        if(it.doc_id == number_doc)
        {
            ++it.count;
            return true;
        }
    }
    return false;
}

void comparison(std::vector<engine::Entry>& list_dictionary)
{
    if(list_dictionary[list_dictionary.size() - 1].doc_id < list_dictionary[list_dictionary.size() - 2].doc_id)
    {
        std::swap(list_dictionary[list_dictionary.size()-1],
                             list_dictionary[list_dictionary.size()-2]);
    }
}

void emplaceEntry(const int number_doc, const std::string& text_document ,std::map<std::string, std::vector<engine::Entry>>& freq_dictionary)
{
    std::stringstream docStream{std::move(text_document)};
    std::string word;
    while(docStream >> word)
    {
        if(freq_dictionary.find(word) == freq_dictionary.end())
            freq_dictionary[word].emplace_back(engine::Entry(number_doc, 1));
        else if(!check_dictionary(number_doc, freq_dictionary[word]))
        {
            freq_dictionary[word].emplace_back(engine::Entry(number_doc, 1));
            comparison(freq_dictionary[word]);
//            std::sort(freq_dictionary[word].begin(), freq_dictionary[word].end(),
//                      [](engine::Entry& first, engine::Entry& second)
//                      {
//                        return first.doc_id < second.doc_id;
//                      });
        }
    }
}

void init_thread(const std::vector<std::string>& docs, std::map<std::string, std::vector<engine::Entry>>& freq_dictionary)
{
    ThreadPool myPool;
    std::vector<std::future<void>> tasks;
    for(int i{}; i < docs.size(); ++i)
        tasks.emplace_back(myPool.commit(emplaceEntry, i, std::ref(docs[i]), std::ref(freq_dictionary)));

    for(auto& task : tasks)
        task.get();
}

engine::InvertedIndex::InvertedIndex(const std::vector<std::string>& input_docs) /*: docs(input_docs)*/
{
    init_thread(input_docs, freq_dictionary);
}


engine::InvertedIndex& engine::InvertedIndex::operator=(const engine::InvertedIndex& other)
{
    if(&other == this) return *this;
    for(auto& it: other.freq_dictionary)
    {
        for(auto& it2: it.second)
            this->freq_dictionary[it.first].emplace_back(it2);
    }
    return *this;
}

void engine::InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs)
{
    freq_dictionary.clear();
    init_thread(input_docs, freq_dictionary);
}

std::vector<engine::Entry>& engine::InvertedIndex::GetWordCount(const std::string& word)
{
    if(freq_dictionary.empty()) throw std::runtime_error("Initialization error"); //Initialization error

    return freq_dictionary[word];
}

//std::mutex thread_lock;

//bool engine::Entry::operator==(const Entry& other) const
//{
//    return ((other.count == this->count) && (other.doc_id == this->doc_id));
//}

//bool check_dictionary(const int& number_doc, std::vector<engine::Entry>& list_dictionary)
//{
//    for(auto& it: list_dictionary)
//    {
//        if(it.doc_id == number_doc)
//        {
//            ++it.count;
//            return true;
//        }
//    }
//    return false;
//}

//void comparison(std::vector<engine::Entry>& list_dictionary)
//{
//    if(list_dictionary[list_dictionary.size() - 1].doc_id < list_dictionary[list_dictionary.size() - 2].doc_id)
//    {
//        std::swap(list_dictionary[list_dictionary.size()-1],
//                  list_dictionary[list_dictionary.size()-2]);
//    }
//}

//void emplaceEntry(const int number_doc, const std::string& text_document ,std::map<std::string, std::map<int,int>>& freq_dictionary)
//{
//    std::stringstream docStream{std::move(text_document)};
//    std::string word;
//    while(docStream >> word)
//    {
//        if(freq_dictionary.find(word) == freq_dictionary.end())
//            freq_dictionary[word][number_doc] = 1;
//        else if(freq_dictionary[word].find(number_doc) == freq_dictionary[word].end())
//            freq_dictionary[word][number_doc] = 1;
//        else
//            ++freq_dictionary[word][number_doc];
//    }
//}

//void init_thread(const std::vector<std::string>& docs, std::map<std::string, std::map<int,int>>& freq_dictionary)
//{
//    ThreadPool myPool;
//    std::vector<std::future<void>> tasks;
//    for(int i{}; i < docs.size(); ++i)
//        tasks.emplace_back(myPool.commit(emplaceEntry, i, std::ref(docs[i]), std::ref(freq_dictionary)));

//    for(auto& task : tasks)
//        task.get();
//}

//engine::InvertedIndex::InvertedIndex(const std::vector<std::string>& input_docs) /*: docs(input_docs)*/
//{
//    init_thread(input_docs, freq_dictionary);
//}


//engine::InvertedIndex& engine::InvertedIndex::operator=(const engine::InvertedIndex& other)
//{
//    if(&other == this) return *this;
//    for(auto& it: other.freq_dictionary)
//    {
//        this->freq_dictionary[it.first] = it.second;
////        for(auto& it2: it.second)
////            this->freq_dictionary[it.first][];
//    }
//    return *this;
//}

//void engine::InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs)
//{
//    freq_dictionary.clear();
//    init_thread(input_docs, freq_dictionary);
//}

//std::map<int, int>& engine::InvertedIndex::GetWordCount(const std::string& word)
//{
//    if(freq_dictionary.empty()) throw std::runtime_error("Initialization error"); //Initialization error

//    return freq_dictionary[word];
//}
