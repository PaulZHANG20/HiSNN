#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include "Net.h"
#include "Tag.h"
#include "Encode.h"
#include "Plot.h"
#include "Param.h"
#include "Synapse.h"
#include "Neuron.h"
#include "LIF.h" // Ensure LIF.h is included to use PreloadLIFData
#include "STDP.h" // Ensure STDP.h is included to use PreloadSTDPData
#include "LookUp.h" // Ensure LookUp.h is included to use PreloadPowerData

void Learning(
    Net& net, std::vector<std::vector<int>>& LIF_Sequence_Vector) {

    std::vector<int> LearningImageTag(Par.Num_LearningImage);
    int Fire_Tag = -1;
    int Fire_Tag_True = -1;
    std::vector<double> Power;
    std::vector<TagCounter> Counters(Par.OutputNeuron);

    LearningImageTag = ReadTagFromCSV(Par.LearningImagePath + "LearningTag.csv");
    std::cout << "Start learning, progress:" << std::endl;
    for (int epoch = 0; epoch < Par.Epochs; ++epoch) {

        for (int img = 0; img < Par.Num_LearningImage; ++img) {
            cv::Mat Image = cv::imread(
                Par.LearningImagePath + std::to_string(img) + ".png", cv::IMREAD_GRAYSCALE);
            Image = Resize_Image(Image, Par.ImageWidth, Par.ImageHeight);
            LIF_Sequence_Vector = Encode_PulseFirst(Image);
            std::vector<std::vector<double>> vmemRecords(
                Par.OutputNeuron, std::vector<double>(Par.Period + 1));
            double totalPower = 0.0; // Initialize total power
            for (int t = 1; t <= Par.Period; ++t) {
                /*std::cout << "Epoch: " << epoch + 1 << "/" << Par.Epochs << ", "
                    << "Image: " << img + 1 << "/" << Par.Num_LearningImage << ", "
                    "Step: " << t << "/" << Par.Period << std::endl;*/

                Fire_Tag = net.Learning(t, LIF_Sequence_Vector);
                if (Fire_Tag != -1) { Fire_Tag_True = Fire_Tag; }
                for (int i = 0; i < Par.OutputNeuron; ++i) {
                    vmemRecords[i][t] = net.Neurons[i].GetVmem();
                }
                // Calculate and accumulate power at each time step
                double stepPower = net.GetStepPower(t, LIF_Sequence_Vector);
                totalPower += stepPower;
                //std::cout << "Step Power: " << stepPower << "J" << std::endl;
            }
            // Store total power into Power vector
            Power.push_back(totalPower);
            std::cout << "Total Power for Image " << img + 1 << ": " << totalPower << "J" << std::endl;
            if (Fire_Tag_True != -1) {
                Counters[Fire_Tag_True].Add(LearningImageTag[img]);
                std::cout << "ImageTag: " << LearningImageTag[img] << ", "
                    << "Fire_Tag: " << Fire_Tag_True << std::endl;
            }
            net.NewStart();
            PlotWeight(net, Par.Weights_SavePath +
                "Weight_Epoch" + std::to_string(epoch + 1) + '_' +
                "Img" + std::to_string(img + 1) + ".png");
            net.SaveSynapses(
                Par.Weights_SavePath +
                "Epoch" + std::to_string(epoch + 1) + '_' +
                "Img" + std::to_string(img + 1) + ".dat");
            PlotVmem(
                net, vmemRecords, Par.Vth, Par.ProcessionImagePath +
                "Vmem_Epoch" + std::to_string(epoch + 1) + '_' +
                "Img" + std::to_string(img + 1) + ".png");
        }
        PlotEnergy(Power, Par.ProcessionImagePath + "Power.png");
    }
    // Calculate and output total power for all images
    double totalPowerAllImages = std::accumulate(Power.begin(), Power.end(), 0.0);
    std::cout << "Total Power for All Images: " << totalPowerAllImages << "J" << std::endl;
    std::vector<int> NeuronTag = Detemine_Tag(Counters);
    SaveTagsToCSV(Par.NeuronTagPath + "NeuronTag.csv", NeuronTag);

    // Write power data to CSV file
    WritePowerToCSV(Par.ProcessionImagePath + "PowerData_Learning.csv", Power);
}

void Classify(
    Net& net, std::vector<std::vector<int>>& LIF_Sequence_Vector) {

    std::vector<int> ClassifyImageTag(Par.Num_ClassifyImage);
    int Fire_Tag = -1;
    int Fire_Tag_True = -1;
    double Accuracy;
    double AccuracySum = 0.0;
    std::vector<double> ClassifyAccuracy;
    std::vector<int> NeuronTag;
    std::vector<double> Power;

    // Create a CSV file to save the accuracy of each image
    std::ofstream accuracyFile(Par.ClassifyImagePath + "Accuracy.csv");
    if (!accuracyFile.is_open()) {
        std::cerr << "Cannot open file: " << Par.ClassifyImagePath + "Accuracy.csv" << std::endl;
        return;
    }
    accuracyFile << "ImageIndex,Accuracy\n"; // Write header

    ClassifyImageTag = ReadTagFromCSV(Par.ClassifyImagePath + "ClassifyTag.csv");
    NeuronTag = ReadTagFromCSV(Par.NeuronTagPath + "NeuronTag.csv");
    std::cout << "Start classification, progress:" << std::endl;
    for (int img = 0; img < Par.Num_ClassifyImage; ++img) {
        cv::Mat Image = cv::imread(
            Par.ClassifyImagePath + std::to_string(img) + ".png", cv::IMREAD_GRAYSCALE);
        Image = Resize_Image(Image, Par.ImageWidth, Par.ImageHeight);
        LIF_Sequence_Vector = Encode_PulseFirst(Image);
        std::vector<std::vector<double>> vmemRecords(
            Par.OutputNeuron, std::vector<double>(Par.Period + 1));
        double totalPower = 0.0; // Initialize total power
        for (int t = 1; t <= Par.Period; ++t) {
            Fire_Tag = net.Classify(t, LIF_Sequence_Vector);
            if (Fire_Tag != -1) {
                Fire_Tag_True = Fire_Tag;
                //break;
            }
            for (int i = 0; i < Par.OutputNeuron; ++i) {
                vmemRecords[i][t] = net.Neurons[i].GetVmem();
            }
            // Calculate and accumulate power at each time step
            double stepPower = net.GetStepPower(t, LIF_Sequence_Vector);
            totalPower += stepPower;
            //std::cout << "Step Power: " << stepPower << "J" << std::endl;
        }
        // Store total power into Power vector
        Power.push_back(totalPower);
        std::cout << "Total Power for Image " << img + 1 << ": " << totalPower << "J" << std::endl;
        net.NewStart();
        if (Fire_Tag_True != -1) {
            std::cout << "NeuronTag[Fire_Tag_True]: " << NeuronTag[Fire_Tag_True] << ", "
                << "ClassifyImageTag[img]: " << ClassifyImageTag[img] << std::endl;
            if (NeuronTag[Fire_Tag_True] == ClassifyImageTag[img]) { Accuracy = 1; }
            else { Accuracy = 0; }
        }
        std::cout << "Accuracy: " << Accuracy << std::endl;
        AccuracySum += Accuracy;
        std::cout << "AccuracySum: " << AccuracySum << std::endl;
        ClassifyAccuracy.push_back(AccuracySum / (img + 1));
        std::cout << "Image: " << img + 1 << "/" << Par.Num_ClassifyImage << ", "
            << "Accuracy: " << AccuracySum / (img + 1) << std::endl;

        // Write the accuracy of each image to the CSV file
        accuracyFile << img + 1 << "," << AccuracySum / (img + 1) << "\n";

        PlotVmem(
            net, vmemRecords, Par.Vth, Par.ProcessionImagePath_c +
            "Img" + std::to_string(img + 1) + ".png");
    }

    PlotAccuracy(ClassifyAccuracy, Par.ProcessionImagePath_c + "Accuracy.png");

    // Write power data to CSV file
    WritePowerToCSV(Par.ProcessionImagePath_c + "PowerData_Classify.csv", Power);

    // Close the CSV file
    accuracyFile.close();
}

///*
int main() {
    // Record start time
    auto start = std::chrono::high_resolution_clock::now();
    Net net(Par.InputNeuron, Par.OutputNeuron);
    bool Learning_or_Classify = Par.Learning_or_Classify;

    // Preload LIF data
    PreloadLIFData();

    // Preload STDP data
    //PreloadSTDPData();

    // Preload Power data
    PreloadPowerData();

    if (Par.Generate_NewWeights) {
        net.InitSynapses();
        net.SaveSynapses(Par.Weights_SaveRandomFile);

        PlotWeight(net, Par.ProcessionImagePath + "Initial.png");
    }
    else {
        net.LoadSynapses(Par.Weights_LoadFile);

        PlotWeight(net, Par.ProcessionImagePath_c + "Initial.png");
    }
    std::vector<std::vector<int>> LIF_Sequence_Vector(Par.InputNeuron);
    std::vector<int> NeuronTag(Par.OutputNeuron);
    if (Learning_or_Classify) {
        Learning(net, LIF_Sequence_Vector);
    }
    else {
        Classify(net, LIF_Sequence_Vector);
    }
    // Record end time
    auto end = std::chrono::high_resolution_clock::now();
    // Calculate running time
    std::chrono::duration<double> duration = end - start;
    std::cout << "Program running time: " << duration.count() << " seconds" << std::endl;

    return 0;
}
//*/
