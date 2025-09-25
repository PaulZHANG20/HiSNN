#ifndef NEURON_H
#define NEURON_H

#include <iostream>
#include <vector>
#include "Param.h"

class Neuron {
public:
    // Constructor
    Neuron();

    // Member functions
    void NewStart();
    double GetVmem() const;
    bool Refractory(int t);
    void Vmem_Limit();
    void Charge(double deltaV);
    void Check_and_Leaky();
    bool Fire(int t);
    void Lateral_Inhibition(int t);

private:
    // Static parameters
    double Cap;
    double Leak;
    double Vth;
    double Vrest;
    int Period_Ref;
    // Dynamic parameters
    double Vmem;
    bool Ref;
    int Ref_Point; // Refractory period start time
};

#endif // NEURON_H
