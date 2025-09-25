#include "Neuron.h"

// Constructor
Neuron::Neuron()
    : Cap(Par.Capacitance), Leak(Par.Leakage), Vth(Par.Vth), Vrest(Par.Vrest), Period_Ref(Par.Period_Ref),
    Vmem(Par.Vrest), Ref(false), Ref_Point(0) {
}
// Neuron reset
void Neuron::NewStart() {
    Vmem = Par.Vrest;
    Ref = false;
    Ref_Point = 0;
}
// Read membrane potential
double Neuron::GetVmem() const { return Vmem; }
// Refractory period
bool Neuron::Refractory(int t) {
    if (Ref && t > Ref_Point + Period_Ref) {
        Ref = false;
    }
    return Ref;
}
// Membrane voltage must be greater than 0
void Neuron::Vmem_Limit() { if (Vmem < 0) { Vmem = 0; } }
// Charge
void Neuron::Charge(double deltaV) {
    Vmem += deltaV;
    Vmem_Limit();
}
// Leakage
void Neuron::Check_and_Leaky() {
    Vmem_Limit();
    double dV = -Leak * Par.Step;
    Vmem += dV;
    Vmem_Limit();
}
// Fire
bool Neuron::Fire(int t) {
    if (Vmem >= Vth) {
        Vmem = Vrest;
        Ref = true;
        Ref_Point = t;
        return true;
    }
    return false;
}
// Lateral inhibition, note that it does not reset the membrane potential
void Neuron::Lateral_Inhibition(int t) {
    Ref = true;
    Ref_Point = t;
}
