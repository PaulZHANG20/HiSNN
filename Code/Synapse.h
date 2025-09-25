#ifndef SYNAPSE_H
#define SYNAPSE_H

#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include "Param.h"
#include "LIF.h"
#include "STDP.h"
#include "Neuron.h"

// Synapse class
class Synapse {
public:
    // Constructor declaration
    Synapse(double DefaultRes, Neuron* neuron);

    // Member function declarations
    void SetRes(double Res);
    double GetRes() const;
    void UpdateRes(int t, const std::vector<int>& LIF_Sequence);
    double GetDeltaV(int t, const std::vector<int>& LIF_Sequence) const;
    double GetPower() const;

private:
    double Resistance; // Resistance
    Neuron* neuron;    // Neuron connected by the synapse
};

#endif
