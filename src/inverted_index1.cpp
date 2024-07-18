#include "../include/inverted_index.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <functional>
#include <exception>
#include <stdexcept>
#include <sstream>
#include "../include/threadPool.h"

std::mutex threadLock;

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
    if( list_dictionary.size() < 2) return;
    if(list_dictionary[list_dictionary.size() - 1].doc_id < list_dictionary[list_dictionary.size() - 2].doc_id)
    {
        std::swap(list_dictionary[list_dictionary.size()-1],
                             list_dictionary[list_dictionary.size()-2]);
    }
}

void sort(const int number_doc, const std::string& text_document ,std::map<std::string, std::vector<engine::Entry>>& freq_dictionary)
{
    std::stringstream stream(text_document);
    std::string word;
    while(stream >> word)
    {
        threadLock.lock();
        if(freq_dictionary.find(word) == freq_dictionary.end())
            freq_dictionary[word].emplace_back(engine::Entry(number_doc, 1));
        else
        {
            if(!check_dictionary(number_doc, freq_dictionary[word]))
            {
                freq_dictionary[word].emplace_back(engine::Entry(number_doc, 1));
                comparison(freq_dictionary[word]);
            }
        }
        threadLock.unlock();
    }
}

void init_thread(const std::vector<std::string>& docs, std::map<std::string, std::vector<engine::Entry>>& freq_dictionary)
{
//    try{
//        ThreadPool indexFiles;
//        std::vector<std::future<void>> futures;
//        for(size_t i{}; i < docs.size(); ++i)
//            futures.emplace_back(indexFiles.commit(sort, i, std::ref(docs[i]), std::ref(freq_dictionary)));

//        for(auto& future : futures)
//            future.get();
//    }
//    catch(std::exception& e)
//    {
//        std::cout << "Error: " << e.what() << std::endl;
//    }
    std::vector<std::thread> thread_sort;
    for(int i = 0; i < docs.size(); ++i)
        thread_sort.emplace_back(sort, i, std::ref(docs[i]), std::ref(freq_dictionary));

    for(auto& my_join: thread_sort)
        my_join.join();

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
            this->freq_dictionary[it.first].push_back(it2);
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
    if(freq_dictionary.empty()) throw std::exception(); //Initialization error
    else if(freq_dictionary.find(word) == freq_dictionary.end()) throw std::runtime_error("World not found");

    return freq_dictionary[word];
}
