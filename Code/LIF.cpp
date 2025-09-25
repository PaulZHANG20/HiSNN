// Physical model + lookup table model
#include "LIF.h"
#include "LookUp.h" // Ensure LookUp.h is included to use Read_csv_xy

// Global variables for preloaded LIF data
std::vector<double> voltages;
std::vector<double> deltaV;

// Preload LIF data
void PreloadLIFData() {
    Read_csv_xy(voltages, deltaV, Par.LookUp_LIF);
}

// Physical model: get gate voltage from FTJ resistance
double Get_Voltage_LIF(double Resistance, int Period_Charge, double V_PL, double V_Rst, double C_Fe) {
    return V_PL - (V_PL - V_Rst) * std::exp(-Period_Charge * (1e-9) / (Resistance * C_Fe));
}

// Output membrane voltage change caused by synapse
double LIF(int t, const std::vector<int>& LIF_Sequence, double Resistance) {
    // Calculate voltage
    double voltage = Get_Voltage_LIF(Resistance, Par.Period_Charge, Par.V_PL, Par.V_Rst, Par.C_Fe);
    // Interpolation calculation
    if (std::find(LIF_Sequence.begin(), LIF_Sequence.end(), t) != LIF_Sequence.end()) {
        return Interpolate_xy(voltages, deltaV, voltage) / Par.Capacitance;
    }
    return 0.0;
}
