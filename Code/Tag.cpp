#include "Tag.h"

TagCounter::TagCounter() : Tag(Par.OutputNeuron, -1) {}

void TagCounter::Add(int tag) { Tag[tag]++; }

void TagCounter::clear() {
    for (int i = 0; i < Par.OutputNeuron; ++i) { Tag[i] = -1; }
}

int TagCounter::Get_MaxTag() {
    int max = 0;
    int max_tag = 0;
    for (int i = 0; i < Par.OutputNeuron; ++i) {
        if (Tag[i] > max) {
            max = Tag[i];
            max_tag = i;
        }
    }
    return max_tag;
}

std::vector<int> Detemine_Tag(const std::vector<TagCounter>& Counters) {
    std::vector<int> Leaning_Tag;
    for (auto counter : Counters) {
        Leaning_Tag.push_back(counter.Get_MaxTag());
    }
    return Leaning_Tag;
}

// --------------------Tag and CSV--------------------
std::vector<int> ReadTagFromCSV(const std::string& filename) {
    std::vector<int> result;
    std::ifstream file(filename);
    std::string line;

    // Skip header
    if (std::getline(file, line)) {
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string cell;
            int columnIndex = 0;
            while (std::getline(ss, cell, ',')) {
                if (columnIndex == 1) { // Second column
                    result.push_back(std::stoi(cell));
                    break;
                }
                ++columnIndex;
            }
        }
    }

    return result;
}

void SaveTagsToCSV(const std::string& filename, const std::vector<int>& tags) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return;
    }
    file << "OutputNeuron,Tag\n";
    for (size_t i = 0; i < tags.size(); ++i) {
        file << i << "," << tags[i] << "\n";
    }
    file.close();
}
void WritePowerToCSV(const std::string& filename, const std::vector<double>& power) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& p : power) {
            file << p << "\n";
        }
        file.close();
    }
    else {
        std::cerr << "Cannot open file: " << filename << std::endl;
    }
}
