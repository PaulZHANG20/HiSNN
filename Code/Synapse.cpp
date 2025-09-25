#include "Synapse.h"
#include "Net.h"
#include "LookUp.h" // Ensure LookUp.h is included to use preloaded data
#include <fstream>

void PreloadPowerData() {
    Read_csv_xyz_power(Par.LookUp_Power);
}
Synapse::Synapse(double DefaultRes, Neuron* neuron)
    : Resistance(DefaultRes), neuron(neuron) {
} // Initialize neuron pointer

// Write and read Resistance
void Synapse::SetRes(double Res) { Resistance = Res; }
double Synapse::GetRes() const {
    //  std::cout << "Get Resistance: " << Resistance << std::endl;
    return Resistance;
}
// Update Resistance
void Synapse::UpdateRes(int t, const std::vector<int>& LIF_Sequence) {
    Resistance = STDP(t, LIF_Sequence, Resistance);
}
// Calculate voltage change according to time t
double Synapse::GetDeltaV(int t, const std::vector<int>& LIF_Sequence) const {
    return LIF(t, LIF_Sequence, Resistance);
}
// Get the power of a single synapse
double Synapse::GetPower() const {
    double Vmem_now = neuron->GetVmem();
    double tun_now = ((Par.V_PL - Par.V_Rst) / Resistance)
        / (Par.Length_Fe * Par.Width_Fe * 1e4);
    double power = FindClosest_xyz_power(Vmem_now, tun_now);
    return power;
}
