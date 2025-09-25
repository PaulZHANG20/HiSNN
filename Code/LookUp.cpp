#include "LookUp.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <cmath>

std::vector<double> tun;
std::vector<double> power;

std::vector<double> x_values_stdp;                  // STDP x value list
std::vector<double> y_values_stdp;                  // STDP y value list
std::vector<std::vector<double>> z_values_stdp;     // STDP z value matrix

std::vector<double> x_values_power;                 // Power x value list
std::vector<double> y_values_power;                 // Power y value list
std::vector<std::vector<double>> z_values_power;    // Power z value matrix

// ------------ Find y by x, implemented by interpolation ------------
void Read_csv_xy(
    std::vector<double>& x_values,
    std::vector<double>& y_values,
    const std::string& filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return;
    }
    std::string line;
    std::getline(file, line);   // Skip header
    x_values.reserve(1000); // Pre-allocate memory, assuming 1000 rows of data
    y_values.reserve(1000);
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string x_str, y_str;
        std::getline(ss, x_str, ',');
        std::getline(ss, y_str, ',');
        try {
            double x_value = std::stod(x_str);
            double y_value = std::stod(y_str);
            x_values.push_back(x_value);
            y_values.push_back(y_value);
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Invalid data: " << line << std::endl;
        }
    }
    file.close();
}

double Interpolate_xy(
    const std::vector<double>& x_values,
    const std::vector<double>& y_values,
    double x_value) {
    // Check if input is within range
    if (x_value <= x_values.front()) { return y_values.front(); }
    if (x_value >= x_values.back()) { return y_values.back(); }
    // Find interpolation position
    auto it = std::lower_bound(x_values.begin(), x_values.end(), x_value);
    auto idx = std::distance(x_values.begin(), it);
    // Ensure idx is not out of bounds
    if (idx == 0 || idx == x_values.size()) {
        std::cerr << "Interpolation position out of bounds" << std::endl;
        return y_values.front(); // Return a default value
    }
    // Interpolation calculation
    double x_value1 = x_values[idx - 1];
    double x_value2 = x_values[idx];
    double y_value1 = y_values[idx - 1];
    double y_value2 = y_values[idx];
    // Ensure no division by zero
    if (x_value2 == x_value1) {
        std::cerr << "Division by zero: x_value2 equals x_value1" << std::endl;
        return y_values.front(); // Return a default value
    }
    double y_value = y_value1 + (x_value - x_value1) * (y_value2 - y_value1) / (x_value2 - x_value1);
    return y_value;
}

// ------------ STDP and Power lookup implementation ------------
void Read_csv_xyz_stdp(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    std::getline(file, line); // Skip header

    // Temporarily store z values, grouped by x
    std::map<double, std::map<double, double>> temp_map;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string x_str, y_str, z_str;
        std::getline(ss, x_str, ',');
        std::getline(ss, y_str, ',');
        std::getline(ss, z_str, ',');
        double x = std::stod(x_str);
        double y = std::stod(y_str);
        double z = std::stod(z_str);

        // Debug print
        //std::cout << "Read line: x = " << x << ", y = " << y << ", z = " << z << std::endl;
        temp_map[x][y] = z;
    }
    file.close();

    // Extract and sort all unique x and y values
    x_values_stdp.clear();
    y_values_stdp.clear();
    for (const auto& x_entry : temp_map) {
        x_values_stdp.push_back(x_entry.first);
        if (y_values_stdp.empty()) {
            for (const auto& y_entry : x_entry.second) {
                y_values_stdp.push_back(y_entry.first);
            }
        }
    }
    std::sort(x_values_stdp.begin(), x_values_stdp.end());
    std::sort(y_values_stdp.begin(), y_values_stdp.end());

    // Fill z_values_stdp
    z_values_stdp.clear();
    z_values_stdp.resize(x_values_stdp.size(), std::vector<double>(y_values_stdp.size(), 0.0));
    for (size_t i = 0; i < x_values_stdp.size(); ++i) {
        double x = x_values_stdp[i];
        for (size_t j = 0; j < y_values_stdp.size(); ++j) {
            double y = y_values_stdp[j];
            if (temp_map[x].find(y) != temp_map[x].end()) {
                z_values_stdp[i][j] = temp_map[x][y];
            }
            else {
                // Debug print
                //std::cout << "Missing data for x = " << x << ", y = " << y << std::endl;
            }
        }
    }
}

void Read_csv_xyz_power(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::string line;
    std::getline(file, line); // Skip header

    // Temporarily store z values, grouped by x
    std::map<double, std::map<double, double>> temp_map;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string x_str, y_str, z_str;
        std::getline(ss, x_str, ',');
        std::getline(ss, y_str, ',');
        std::getline(ss, z_str, ',');
        double x = std::stod(x_str);
        double y = std::stod(y_str);
        double z = std::stod(z_str);

        // Round y to 6 decimal places
        double rounded_y = std::round(y * 1e6) / 1e6;

        // Debug print (optional)
        // std::cout << "Read line: x = " << x << ", y = " << y << " (rounded to " << rounded_y << "), z = " << z << std::endl;

        temp_map[x][rounded_y] = z;
    }
    file.close();

    // Extract and sort all unique x and y values
    x_values_power.clear();
    y_values_power.clear();
    for (const auto& x_entry : temp_map) {
        x_values_power.push_back(x_entry.first);
        if (y_values_power.empty()) {
            for (const auto& y_entry : x_entry.second) {
                y_values_power.push_back(y_entry.first);
            }
        }
    }
    std::sort(x_values_power.begin(), x_values_power.end());
    std::sort(y_values_power.begin(), y_values_power.end());

    // Fill z_values_power
    z_values_power.clear();
    z_values_power.resize(x_values_power.size(), std::vector<double>(y_values_power.size(), 0.0));

    for (size_t i = 0; i < x_values_power.size(); ++i) {
        double x = x_values_power[i];
        for (size_t j = 0; j < y_values_power.size(); ++j) {
            double y = y_values_power[j];
            // Round y to 6 decimal places
            double rounded_y = std::round(y * 1e6) / 1e6;
            if (temp_map[x].find(rounded_y) != temp_map[x].end()) {
                z_values_power[i][j] = temp_map[x][rounded_y];

                // Debug print (optional)
                // std::cout << "Filling z_values_power[" << i << "][" << j << "] = " << temp_map[x][rounded_y] << " for x = " << x << ", y = " << rounded_y << std::endl;
            }
            else {
                // Debug print (optional)
                // std::cout << "Missing data for x = " << x << ", y = " << rounded_y << std::endl;
            }
        }
    }
}

// Find the closest z value
double FindClosest_xyz_stdp(double x_now, double y_now) {
    if (x_values_stdp.empty() || y_values_stdp.empty() || z_values_stdp.empty()) {
        throw std::runtime_error("Data not initialized");
    }

    // Find the closest x index (using binary search)
    auto x_upper = std::lower_bound(x_values_stdp.begin(), x_values_stdp.end(), x_now);
    int x_index = x_upper - x_values_stdp.begin();
    if (x_index > 0 && x_index < (int)x_values_stdp.size()) {
        if (std::abs(x_values_stdp[x_index - 1] - x_now) < std::abs(x_values_stdp[x_index] - x_now)) {
            x_index--;
        }
    }
    else if (x_index == (int)x_values_stdp.size()) {
        x_index--;
    }

    // Find the closest y index (using binary search)
    auto y_upper = std::lower_bound(y_values_stdp.begin(), y_values_stdp.end(), y_now);
    int y_index = y_upper - y_values_stdp.begin();
    if (y_index > 0 && y_index < (int)y_values_stdp.size()) {
        if (std::abs(y_values_stdp[y_index - 1] - y_now) < std::abs(y_values_stdp[y_index] - y_now)) {
            y_index--;
        }
    }
    else if (y_index == (int)y_values_stdp.size()) {
        y_index--;
    }

    return z_values_stdp[x_index][y_index];
}

double FindClosest_xyz_power(double x_now, double y_now) {
    if (x_values_power.empty() || y_values_power.empty() || z_values_power.empty()) {
        throw std::runtime_error("Data not initialized");
    }

    // Find the closest x index (using binary search)
    auto x_upper = std::lower_bound(x_values_power.begin(), x_values_power.end(), x_now);
    int x_index = x_upper - x_values_power.begin();
    if (x_index > 0 && x_index < (int)x_values_power.size()) {
        if (std::abs(x_values_power[x_index - 1] - x_now) < std::abs(x_values_power[x_index] - x_now)) {
            x_index--;
        }
    }
    else if (x_index == (int)x_values_power.size()) {
        x_index--;
    }

    // Find the closest y index (using binary search)
    auto y_upper = std::lower_bound(y_values_power.begin(), y_values_power.end(), y_now);
    int y_index = y_upper - y_values_power.begin();
    if (y_index > 0 && y_index < (int)y_values_power.size()) {
        if (std::abs(y_values_power[y_index - 1] - y_now) < std::abs(y_values_power[y_index] - y_now)) {
            y_index--;
        }
    }
    else if (y_index == (int)y_values_power.size()) {
        y_index--;
    }

    // Get the corresponding z value
    return z_values_power[x_index][y_index];
}
