#ifndef PARAM_H    
#define PARAM_H

#include <string>

struct Param {
    // Applied voltage
    double V_PL = 0.7;
    double V_Rst = 0.5;
    double V_Read = V_PL - V_Rst;
    // STDP voltage
    double pmin = 0.3;
    double pmax = 0.7;
    double nmin = -0.7;
    double nmax = -1.4;

    // Waveform parameters, unit: ns
    int Step = 1;                   // 1ns
    int Period_Charge = 3000;       // 3us
    int Width_LIF = 1;              // 1ns
    int Period_LIF = 500;           // 500ns
    int Width_STDP = 10;            // 10ns
    int Period_STDP = Period_LIF * 2;

    // Device parameters
    double C_Fe = 3.471e-14;   // Considering silicon layer capacitance and MOS capacitance, can be corrected
    double Width_Fe = 0.7e-6;     // 0.7um
    double Length_Fe = 0.7e-6;     // 0.7um

    // Lookup table paths
    std::string LookUp_LIF = "./Data/Lookup/LIF.csv";  // Limited: membrane capacitance, LIF width
    std::string LookUp_STDP = "./Data/Lookup/STDP.csv";
    std::string LookUp_Power = "./Data/Lookup/Power_test.csv";
    // Limited: STDP BL width, STDP period, STDP voltage, FTJ response time

    // Neuron parameters
    double Capacitance = 1.3;  // pF
    double Leakage = 0.0;
    double Vth = 0.6;   // Different neurons can have different thresholds + variable threshold
    double Vrest = 0.0;
    int Period_Ref = 500;

    // Synapse parameters
    double DefaultRes = 100e6;        // Default resistance 100MΩ
    double ResMin = 45e6;             // Minimum resistance 45MΩ
    double ResMax = 1150e6;           // Maximum resistance 1150MΩ

    // Network hyperparameters
    int InputNeuron = 196;
    int OutputNeuron = 5;
    int NumClass = 5;

    // Training parameters
    bool Learning_or_Classify = false;
    int Epochs = 1;
    int Period = Period_LIF;

    // Image parameters
    std::string LearningImagePath = "./Data/Learning/";
    std::string ClassifyImagePath = "./Data/Classify/";
    std::string NeuronTagPath = "./Data/Learning/";
    int Num_LearningImage = 50;
    int Num_ClassifyImage = 100;
    int ImageWidth = 14;
    int ImageHeight = 14;
    //EncodingType encodingType = EncodingType::PulseFirst;

    // Weight saving and loading
    bool Generate_NewWeights = false;
    //std::string Weights_LoadFile = "./Weight_85_1.3/Random.dat";
    std::string Weights_LoadFile = "./Weight/Epoch1_Img50.dat";
    std::string Weights_SaveRandomFile = "./Weight/Random.dat";
    std::string Weights_SavePath = "./Weight/";

    // Image process save path
    std::string ProcessionImagePath = "./Data/Learning/Procession/";
    std::string ProcessionImagePath_c = "./Data/Classify/Procession/";
};
const Param Par;

#endif 
