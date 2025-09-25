#ifndef NET_H
#define NET_H

#include <vector>
#include <random>
#include <fstream>
#include <limits>
#include "Synapse.h"
#include "Neuron.h"
#include <string>
#include <vector>
#include "Param.h"
#include "Synapse.h"
#include "Neuron.h"

class Net {
public:
    // Constructor
    Net(int InputNeuron, int OutputNeuron);
    // Get the synapse array of the network
    std::vector<std::vector<Synapse>> GetSynapses() const;
    // Get the neuron sequence of the network
    std::vector<Neuron> GetNeuron() const;

    // Reset the state of the neurons
    void NewStart();
    // Get deltaV of the neuron
    double GetNeuronDeltaV(int i, int t, const std::vector<std::vector<int>>& LIF_Sequence_Vector) const;
    // Get the power consumption of the network
    double GetPower(std::vector<std::vector<int>> LIF_Sequence_Vector) const;
    // Get the power consumption of the network at each step
    double GetStepPower(int t, const std::vector<std::vector<int>>& LIF_Sequence_Vector) const;
    // Learning process
    int Learning(int t, const std::vector<std::vector<int>>& LIF_Sequence_Vector);
    // Classification process
    int Classify(int t, const std::vector<std::vector<int>>& LIF_Sequence_Vector);

    // Initialize the synapse array of the network
    void InitSynapses();
    void SaveSynapses(const std::string& filename);
    void LoadSynapses(const std::string& filename);
    void PrintSynapses() const;
public:
    int InputNeuron;
    int OutputNeuron;
    std::vector<std::vector<Synapse>> Synapses;
    std::vector<Neuron> Neurons;
};

#endif // NET_H
