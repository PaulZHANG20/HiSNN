#ifndef ENCODE_H
#define ENCODE_H

#include <vector>
#include <opencv2/opencv.hpp>
#include <vector>
#include "Param.h"

std::vector<std::vector<int>> Encode_PulseFirst(const cv::Mat& image);

#endif 
