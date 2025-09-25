#include "Plot.h"

// Change image size
cv::Mat Resize_Image(const cv::Mat& image, int width, int height) {
    cv::Mat resizedImage;
    cv::resize(image, resizedImage, cv::Size(width, height));
    return resizedImage;
}

// Plot energy, bar chart
void PlotEnergy(
    const std::vector<double>& Powers,
    const std::string& outputFile) {
    int width = 800;
    int height = 600;
    int padding = 100;
    cv::Mat image(height + 2 * padding, width + 2 * padding, CV_8UC3, cv::Scalar(255, 255, 255));
    double maxPower = *std::max_element(Powers.begin(), Powers.end());
    double yMax = maxPower * 1.5;
    int barWidth = width / Powers.size();

    // Draw bar chart
    for (size_t i = 0; i < Powers.size(); ++i) {
        int barHeight = static_cast<int>((Powers[i] / yMax) * height);
        cv::rectangle(image, cv::Point(i * barWidth + padding, height + padding - barHeight), cv::Point((i + 1) * barWidth + padding, height + padding), cv::Scalar(255, 0, 0), cv::FILLED);
    }

    // Draw grid
    for (int i = 0; i <= 10; ++i) {
        int y = padding + i * height / 10;
        cv::line(image, cv::Point(padding, y), cv::Point(width + padding, y), cv::Scalar(200, 200, 200), 1);
    }
    for (int i = 0; i <= 10; ++i) {
        int x = padding + i * width / 10;
        cv::line(image, cv::Point(x, padding), cv::Point(x, height + padding), cv::Scalar(200, 200, 200), 1);
    }

    // Add tick labels
    for (int i = 0; i <= 10; ++i) {
        int y = padding + i * height / 10;
        double yLabel = yMax * (10 - i) / 10;
        std::ostringstream yLabelStream;
        yLabelStream << std::fixed << std::setprecision(2) << yLabel;
        cv::putText(image, yLabelStream.str(), cv::Point(padding - 70, y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }
    for (int i = 0; i <= 10; ++i) {
        int x = padding + i * width / 10;
        int xLabel = i * Powers.size() / 10;
        cv::putText(image, std::to_string(xLabel), cv::Point(x - 10, height + padding + 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }

    // Add text
    cv::putText(image, "Cycle", cv::Point(width / 2 + padding - 50, height + padding + 40), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);
    cv::putText(image, "Network Energy Consumption", cv::Point(width / 2 + padding - 200, padding - 10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);
    cv::putText(image, "E (J)", cv::Point(padding - 40, height / 2 + padding), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);

    cv::imwrite(outputFile, image);
}

// Plot accuracy
void PlotAccuracy(
    const std::vector<double>& accuracies,
    const std::string& filename) {
    int width = 800;
    int height = 600;
    int padding = 100;
    cv::Mat image(height + 2 * padding, width + 2 * padding, CV_8UC3, cv::Scalar(255, 255, 255));
    std::vector<double> xValues(accuracies.size());
    std::iota(xValues.begin(), xValues.end(), 1);
    double maxAccuracy = *std::max_element(accuracies.begin(), accuracies.end());
    for (size_t i = 1; i < accuracies.size(); ++i) {
        cv::line(image, cv::Point((i - 1) * width / accuracies.size() + padding, height + padding - static_cast<int>((accuracies[i - 1] / maxAccuracy) * height)),
            cv::Point(i * width / accuracies.size() + padding, height + padding - static_cast<int>((accuracies[i] / maxAccuracy) * height)), cv::Scalar(255, 0, 0), 2);
    }
    // Add tick labels
    for (int i = 0; i <= 10; ++i) {
        int y = padding + i * height / 10;
        double yLabel = maxAccuracy * (10 - i) / 10;
        cv::putText(image, std::to_string(yLabel), cv::Point(padding - 50, y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }
    for (int i = 0; i <= 10; ++i) {
        int x = padding + i * width / 10;
        int xLabel = i * accuracies.size() / 10;
        cv::putText(image, std::to_string(xLabel), cv::Point(x - 10, height + padding + 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }
    // Add text
    cv::putText(image, "Epoch", cv::Point(width / 2 + padding - 50, height + padding + 40), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);
    cv::putText(image, "Accuracy", cv::Point(width / 2 + padding - 200, padding - 10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);
    cv::putText(image, "Accuracy (%)", cv::Point(padding - 40, height / 2 + padding), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);

    cv::imwrite(filename, image);
}

// Plot weights
void PlotWeight(const Net& net, const std::string& SaveFile) {
    const auto& synapses = net.GetSynapses();
    int inputNeuronCount = net.InputNeuron;
    int outputNeuronCount = net.OutputNeuron;
    int gridSize = static_cast<int>(std::sqrt(inputNeuronCount));
    int imageSize = gridSize * 100;
    cv::Mat bigImage(outputNeuronCount * imageSize, imageSize, CV_8UC1, cv::Scalar(255));
    for (int n = 0; n < outputNeuronCount; ++n) {
        cv::Mat smallImage(gridSize, gridSize, CV_8UC1);
        double minResistance = Par.ResMin;
        double maxResistance = Par.ResMax;
        for (int i = 0; i < inputNeuronCount; ++i) {
            double resistance = synapses[n][i].GetRes();
            int row = i / gridSize;
            int col = i % gridSize;
            smallImage.at<uchar>(row, col) = static_cast<uchar>(255.0 * (maxResistance - resistance) / (maxResistance - minResistance));
        }
        cv::resize(smallImage, smallImage, cv::Size(imageSize, imageSize), 0, 0, cv::INTER_NEAREST);
        smallImage.copyTo(bigImage(cv::Rect(0, n * imageSize, imageSize, imageSize)));
        // Add subtitle
        cv::putText(bigImage, "Neuron " + std::to_string(n + 1), cv::Point(10, n * imageSize + 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0), 2);
    }
    // Add main title
    cv::putText(bigImage, "Synapse Weights", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0), 3);
    cv::imwrite(SaveFile, bigImage);
}

// Plot membrane voltage changes
void PlotVmem(
    const Net& net,
    const std::vector<std::vector<double>>& Vmems,
    double Vth,
    const std::string& outputFile) {
    int m = Vmems.size();
    if (m == 0) return;
    int n = 500; // Set the maximum value of the x-axis to 500
    int width = 1200;
    int height = 800;
    int padding = 100;
    cv::Mat image(height + 2 * padding, width + 2 * padding, CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < m; ++i) {
        std::vector<double> xValues(n + 1);
        std::iota(xValues.begin() + 1, xValues.end(), 1); // Generate x-axis values
        xValues[0] = 0; // Add point (0, 0)
        std::vector<double> yValues = Vmems[i];
        yValues.resize(n + 1, 0); // Ensure yValues size matches xValues
        for (size_t j = 1; j < xValues.size(); ++j) {
            cv::line(image, cv::Point((j - 1) * width / xValues.size() + padding, height + padding - static_cast<int>((yValues[j - 1] / 0.7) * height)),
                cv::Point(j * width / xValues.size() + padding, height + padding - static_cast<int>((yValues[j] / 0.7) * height)), cv::Scalar(255, 0, 0), 2);
        }
        // Add label at the end of each line
        cv::putText(image, "Neuron " + std::to_string(i + 1), cv::Point(width + padding + 10, height + padding - static_cast<int>((yValues.back() / 0.7) * height)), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }
    // Draw red line
    cv::line(image, cv::Point(padding, height + padding - static_cast<int>((Vth / 0.7) * height)), cv::Point(width + padding, height + padding - static_cast<int>((Vth / 0.7) * height)), cv::Scalar(0, 0, 255), 2, cv::LINE_8, 0);
    // Draw grid
    for (int i = 0; i <= 14; ++i) { // 14 ticks, each tick 0.05
        int y = padding + i * height / 14;
        cv::line(image, cv::Point(padding, y), cv::Point(width + padding, y), cv::Scalar(200, 200, 200), 1);
    }
    for (int i = 0; i <= 10; ++i) {
        int x = padding + i * width / 10;
        cv::line(image, cv::Point(x, padding), cv::Point(x, height + padding), cv::Scalar(200, 200, 200), 1);
    }
    // Add tick labels
    for (int i = 0; i <= 14; ++i) { // 14 ticks, each tick 0.05
        int y = padding + i * height / 14;
        double yLabel = 0.7 * (14 - i) / 14;
        std::ostringstream yLabelStream;
        yLabelStream << std::fixed << std::setprecision(3) << yLabel;
        cv::putText(image, yLabelStream.str(), cv::Point(padding - 70, y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }
    for (int i = 0; i <= 10; ++i) {
        int x = padding + i * width / 10;
        int xLabel = i * n / 10;
        cv::putText(image, std::to_string(xLabel), cv::Point(x - 15, height + padding + 50), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }
    // Set main title
    cv::putText(image, "Vmem", cv::Point(width / 2 + padding - 50, padding - 20), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 0), 3);
    // Add axis labels
    cv::putText(image, "Time (ns)", cv::Point(width / 2 + padding - 50, height + padding + 70), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);
    cv::putText(image, "Voltage (V)", cv::Point(padding - 100, height / 2 + padding), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);

    cv::imwrite(outputFile, image);
}
