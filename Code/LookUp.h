#ifndef LOOKUP_H
#define LOOKUP_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <limits>
#include "Param.h"

extern std::vector<double> voltages;
extern std::vector<double> deltaV;
extern std::vector<std::vector<double>> z_values_stdp;
extern std::vector<std::vector<double>> z_values_power;
extern std::vector<double> x_values_stdp;
extern std::vector<double> x_values_power;
extern std::vector<double> y_values_stdp;
extern std::vector<double> y_values_power;

void PreloadPowerData();
void PreloadLIFData();
void PreloadSTDPData();
// DataPoint struct
struct DataPoint {
    double x;    // current
    double y;    // voltage
    double z;    // ratio
};

void Read_csv_xy(std::vector<double>& x_values, std::vector<double>& y_values, const std::string& filename);
double Interpolate_xy(const std::vector<double>& x_values, const std::vector<double>& y_values, double x_value);
void Read_csv_xyz_stdp(const std::string& filename);
void Read_csv_xyz_power(const std::string& filename);
double FindClosest_xyz_stdp(double x_now, double y_now);
double FindClosest_xyz_power(double x_now, double y_now);

#endif
