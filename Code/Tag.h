#ifndef TAG_H
#define TAG_H

#include <opencv2/opencv.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include "Param.h"

// Read the second column of a CSV file into a vector<int>, skip the header
std::vector<int> ReadTagFromCSV(const std::string& filename);

// Save tags to a CSV file
void SaveTagsToCSV(const std::string& filename, const std::vector<int>& tags);

// Add WritePowerToCSV function declaration
void WritePowerToCSV(const std::string& filename, const std::vector<double>& power);

// TagCounter class, used for counting tags
class TagCounter {
public:
    TagCounter();

    void Add(int tag);
    void clear();
    int Get_MaxTag();

public:
    std::vector<int> Tag;
};

// Determine tags
std::vector<int> Detemine_Tag(const std::vector<TagCounter>& Counters);

std::vector<int> ReadTagFromCSV(const std::string& filename);

void SaveTagsToCSV(const std::string& filename, const std::vector<int>& tags);

#endif // TAG_H
