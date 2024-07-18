#include "../include/inverted_index.h"
#include <iostream>

void TestInvertedIndexFunctionality(const std::vector<std::string>& docs,
                                    const std::vector<std::string>& requests)
{
    std::vector<std::vector<engine::Entry>> result;
//    std::vector<std::map<int, int>> result;
    engine::InvertedIndex idx;

    idx.UpdateDocumentBase(docs);

    for(auto& request : requests)
    {
        std::vector<engine::Entry> word_count = idx.GetWordCount(request);
        result.push_back(word_count);
//        std::map<int, int> word_count = idx.GetWordCount(request);
//        result.emplace_back(word_count);
    }

    std::cout << "{\n";
    for(auto& vec : result)
    {
        std::cout << "{\n";
        for(auto& mean : vec)
        {
            std::cout << "{" << mean.doc_id << ", " << mean.count << "}, ";
//            std::cout << "{" << mean.first << ", " << mean.second << "}, ";
        }
        std::cout << "}\n";
    }
    std::cout << "}" << std::endl;
}

void testSecond()
{
    const std::vector<std::string> docs =
    {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    const std::vector<std::string> requests = {"milk", "water", "cappuccino"};
    TestInvertedIndexFunctionality(docs, requests);
}

int main()
{
    testSecond();
    return 0;
}
