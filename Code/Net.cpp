#include "Net.h"
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <random>

// Constructor
Net::Net(int InputNeuron, int OutputNeuron)
    : InputNeuron(InputNeuron), OutputNeuron(OutputNeuron) {
    Neurons.resize(OutputNeuron, Neuron());
    Synapses.resize(OutputNeuron, std::vector<Synapse>(InputNeuron, Synapse(Par.DefaultRes, &Neurons[0])));
    // When initializing each Synapse object, pass the corresponding Neuron object pointer
    for (int i = 0; i < OutputNeuron; ++i) {
        for (int j = 0; j < InputNeuron; ++j) {
            Synapses[i][j] = Synapse(Par.DefaultRes, &Neurons[i]);
        }
    }
}
// Get the synapse array of the network
std::vector<std::vector<Synapse>> Net::GetSynapses() const { return Synapses; }
// Get the neuron sequence of the network
std::vector<Neuron> Net::GetNeuron() const { return Neurons; }

// Reset the state of the network
void Net::NewStart() {
    for (auto& neuron : Neurons) {
        neuron.NewStart();
    }
}
// Get deltaV of the i-th neuron
double Net::GetNeuronDeltaV(
    int i, int t,
    const std::vector<std::vector<int>>& LIF_Sequence_Vector) const {
    double deltaV_Sum = 0.0;
    for (int j = 0; j < InputNeuron; ++j) {
        deltaV_Sum += Synapses[i][j].GetDeltaV(t, LIF_Sequence_Vector[j]);
    }
    return deltaV_Sum;
}
// Get the power consumption of the network, which is related to the connection strength and input signal
// In any case, all pulses will discharge, so the power consumption is related to the number of pulses
//double Net::GetPower(std::vector<std::vector<int>> LIF_Sequence_Vector) const {
//  double Power = 0.0;
//  for (int i = 0; i < OutputNeuron; ++i) {
//      for (int j = 0; j < InputNeuron; ++j) {
//          Power += Synapses[i][j].GetPower() * LIF_Sequence_Vector[j].size();
//      }
//  }
//  return Power;
//}
// Get the power consumption of the network at each step
double Net::GetStepPower(int t, const std::vector<std::vector<int>>& LIF_Sequence_Vector) const {
    double StepPower = 0.0;
    for (int i = 0; i < OutputNeuron; ++i) {
        for (int j = 0; j < InputNeuron; ++j) {
            if (std::find(LIF_Sequence_Vector[j].begin(), LIF_Sequence_Vector[j].end(), t) != LIF_Sequence_Vector[j].end()) {
                StepPower += Synapses[i][j].GetPower();
            }
        }
    }
    return StepPower;
}

// Learning process
int Net::Learning(
    int t,
    const std::vector<std::vector<int>>& LIF_Sequence_Vector) {

    int FireTag = -1; // Record which neuron fires
    for (int i = 0; i < OutputNeuron; ++i) {
        // Each time step, leakage occurs
        Neurons[i].Check_and_Leaky();
        // Check if the refractory period has passed
        if (!Neurons[i].Refractory(t)) {
            // Get deltaV of the i-th neuron and charge
            Neurons[i].Charge(GetNeuronDeltaV(i, t, LIF_Sequence_Vector));
            // Check if fire
            if (Neurons[i].Fire(t)) {
                FireTag = i;
                // All neurons except the firing one are laterally inhibited
                for (int j = 0; j < OutputNeuron; ++j) {
                    if (j != i) { Neurons[j].Lateral_Inhibition(t); }
                }
                // The firing neuron updates its weights
                for (int j = 0; j < InputNeuron; ++j) {
                    Synapses[i][j].UpdateRes(t, LIF_Sequence_Vector[j]);
                }
            }
        }
    }
    return FireTag;
}

// Classification process
int Net::Classify(
    int t,
    const std::vector<std::vector<int>>& LIF_Sequence_Vector) {

    int FireTag = -1; // Record which neuron fires
    for (int i = 0; i < OutputNeuron; ++i) {
        // Each time step, leakage occurs
        Neurons[i].Check_and_Leaky();
        // Check if the refractory period has passed
        if (!Neurons[i].Refractory(t)) {
            // Get deltaV of the i-th neuron and charge
            Neurons[i].Charge(GetNeuronDeltaV(i, t, LIF_Sequence_Vector));
            // Check if fire
            if (Neurons[i].Fire(t)) {
                FireTag = i;
                std::cout << "Neuron " << i << " fired." << std::endl;
                // All neurons except the firing one are laterally inhibited
                for (int j = 0; j < OutputNeuron; ++j) {
                    if (j != i) { Neurons[j].Lateral_Inhibition(t); }
                }
                // The firing neuron does not update its weights
            }
        }
    }
    return FireTag;
}

// Initialize the synapse array of the network using circuit weights
//void Net::InitSynapses() {
//  std::ifstream file("Circuit.csv");
//  if (!file.is_open()) {
//      throw std::runtime_error("Cannot open file: Circuit.csv");
//  }
//
//  // Skip header
//  std::string line;
//  std::getline(file, line);
//
//  // Create a 2D array to store CSV data
//  std::vector<std::vector<double>> csvData(InputNeuron, std::vector<double>(OutputNeuron));
//
//  // Read CSV file line by line
//  int row = 0;
//  while (std::getline(file, line)) {
//      std::stringstream ss(line);
//      std::string value;
//      int col = 0;
//
//      // Parse each value in the row by column
//      while (std::getline(ss, value, ',')) {
//          if (row >= InputNeuron || col >= OutputNeuron) {
//              throw std::runtime_error("Data in CSV file exceeds the size of the network's synapse array");
//          }
//
//          // Convert string to double and store in 2D array
//          csvData[row][col] = std::stod(value);
//          ++col;
//      }
//
//      if (col != OutputNeuron) {
//          throw std::runtime_error("Number of columns in CSV file does not match the number of output neurons");
//      }
//
//      ++row;
//  }
//
//  if (row != InputNeuron) {
//      throw std::runtime_error("Number of rows in CSV file does not match the number of input neurons");
//  }
//
//  // Fill the synapse array column by column
//  for (int col = 0; col < OutputNeuron; ++col) {
//      for (int row = 0; row < InputNeuron; ++row) {
//          Synapses[col][row] = Synapse(csvData[row][col], &Neurons[col]);
//      }
//  }
//
//  file.close();
//}
// Initialize the synapse array of the network
void Net::InitSynapses() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(Par.ResMin, Par.DefaultRes);

    for (int i = 0; i < OutputNeuron; ++i) {
        for (int j = 0; j < InputNeuron; ++j) {
            double random_resistance = dis(gen);
            Synapses[i][j] = Synapse(random_resistance, &Neurons[i]); // Pass the pointer of the Neuron object
        }
    }
}
// Save the synapse array of the network
void Net::SaveSynapses(const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    for (const auto& row : Synapses) {
        for (const auto& synapse : row) {
            double resistance = synapse.GetRes();
            ofs.write(reinterpret_cast<const char*>(&resistance), sizeof(resistance));
        }
    }
}
// Load the synapse array of the network
void Net::LoadSynapses(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    for (auto& row : Synapses) {
        for (auto& synapse : row) {
            double resistance;
            ifs.read(reinterpret_cast<char*>(&resistance), sizeof(resistance));
            synapse.SetRes(resistance);
        }
    }
}
// Print the synapse array of the network
void Net::PrintSynapses() const {
    for (size_t i = 0; i < Synapses.size(); ++i) {
        for (size_t j = 0; j < Synapses[i].size(); ++j) {
            std::cout << "Synapse[" << i << "][" << j << "] Resistance: " << Synapses[i][j].GetRes() << std::endl;
        }
    }
}
