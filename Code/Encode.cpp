#include "Encode.h"

// -------------------- First Spike Encoding --------------------
// For possible future use of other encoding methods, use a two-level container.
// For first spike encoding, the second-level container has only one element.
std::vector<std::vector<int>> Encode_PulseFirst(const cv::Mat& image) {
    int rows = image.rows;
    int cols = image.cols;
    int totalPixels = rows * cols;
    std::vector<std::vector<int>> PulseFirstVectors(totalPixels); // Initialize
    // Traverse each pixel of the image
    int index = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // Get grayscale value
            int grayValue = image.at<uchar>(i, j);
            // Encode according to grayscale value
            int PulseFirstValue = static_cast<int>((1.0 - grayValue / 255.0) * Par.Period_LIF);
            // Store the encoded value in the corresponding vector
            PulseFirstVectors[index].push_back(PulseFirstValue);
            ++index;
        }
    }
    return PulseFirstVectors;
}
