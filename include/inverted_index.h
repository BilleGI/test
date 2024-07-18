#pragma once

#include <vector>
#include <map>
#include <string>


namespace engine
{
    struct Entry
    {
        size_t doc_id;
        size_t count;
        bool operator==(const Entry& other) const;
    };


    class InvertedIndex
    {
    public:
        InvertedIndex() = default;
        InvertedIndex(const std::vector<std::string>& docs);
        InvertedIndex& operator=(const InvertedIndex&);
        void UpdateDocumentBase(const std::vector<std::string>& input_docs);
        std::vector<Entry>& GetWordCount(const std::string& word);
//        std::map<int, int>& GetWordCount(const std::string& word);
    private:
        std::map<std::string, std::vector<Entry>> freq_dictionary;
//        std::map<std::string, std::map<int,int>> freq_dictionary;
    };
}
