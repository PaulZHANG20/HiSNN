#ifndef PLOT_H
#define PLOT_H

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
//#include <matplotlibcpp>
#include "Net.h"
#include "Param.h"

// namespace plt = matplotlibcpp;

// Change image size
cv::Mat Resize_Image(const cv::Mat& image, int width, int height);
// Plot energy, bar chart
void PlotEnergy(const std::vector<double>& Powers, const std::string& outputFile);
// Plot accuracy
void PlotAccuracy(const std::vector<double>& accuracies, const std::string& filename);
// Plot weights
void PlotWeight(const Net& net, const std::string& SaveFile);
// Plot membrane voltage changes
void PlotVmem(const Net& net, const std::vector<std::vector<double>>& Vmems, double Vth, const std::string& outputFile);
#endif
