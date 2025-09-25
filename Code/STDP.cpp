#include "STDP.h"
#include <vector>
#include <cmath>

// Neural network class, used to implement MATLAB's ANN functionality
class NeuralNetwork {
private:
    // Weights and biases, from "New XLSX Worksheet.xlsx"
    std::vector<std::vector<double>> Fwix;  // Weights from input layer to hidden layer 1
    std::vector<double> Fb0;                // Biases for hidden layer 1
    std::vector<std::vector<double>> Fwiy1; // Weights from hidden layer 1 to hidden layer 2
    std::vector<double> Fb1;                // Biases for hidden layer 2
    std::vector<std::vector<double>> Fwiy2; // Weights from hidden layer 2 to output layer
    std::vector<double> Fb2;                // Biases for output layer

    // Matrix multiplication
    std::vector<double> matrixMultiply(const std::vector<std::vector<double>>& m, const std::vector<double>& v) {
        std::vector<double> result(m.size(), 0.0);
        for (size_t i = 0; i < m.size(); ++i) {
            for (size_t j = 0; j < v.size(); ++j) {
                result[i] += m[i][j] * v[j];
            }
        }
        return result;
    }

    // Vector addition
    std::vector<double> vectorAdd(const std::vector<double>& a, const std::vector<double>& b) {
        std::vector<double> result(a.size(), 0.0);
        for (size_t i = 0; i < a.size(); ++i) {
            result[i] = a[i] + b[i];
        }
        return result;
    }

    // Tansig activation function (hyperbolic tangent sigmoid)
    std::vector<double> tansig(const std::vector<double>& v) {
        std::vector<double> result(v.size(), 0.0);
        for (size_t i = 0; i < v.size(); ++i) {
            result[i] = 2.0 / (1.0 + std::exp(-2.0 * v[i])) - 1.0;
        }
        return result;
    }

    // Purelin activation function (linear)
    double purelin(const std::vector<double>& v) {
        return v[0];  // Single output neuron
    }

public:
    NeuralNetwork() {
        // Initialize weights and biases from Excel data
        Fwix = {
            {0.5172, 6.6560},
            {-6.01, 1.0752},
            {7.0642, 9.0616},
            {10.7497, 5.3320},
            {-0.4900, 6.8951},
            {0.8250, -1.3601},
            {4.8848, -5.4798},
            {-0.9481, -1.5523}
        };
        Fb0 = { 7.1075, 5.6707, 3.0805, -3.9935, 6.5136, 1.2566, -0.1093, 0.3034 };
        Fwiy1 = {
            {-10.6265, 1.2235, 0.3887, 0.6397, -3.7163, 7.3866, 0.4821, 5.8050},
            {-6.0702, -0.3525, -0.0865, -0.3954, -2.1075, -0.8162, 0.4381, 0.8346},
            {3.9545, -3.1466, -2.0626, -2.6770, 1.6468, -1.0837, -0.6669, 1.1968},
            {0.9684, 0.4201, 0.5834, 0.3192, 3.8032, -2.0381, 0.6043, -1.4413},
            {3.5857, 0.5369, 0.1877, 0.4415, 0.8220, -1.9804, 0.0436, -0.2452},
            {3.8197, -3.1244, -2.0009, -3.0673, 1.6510, -1.0821, -0.6554, 1.1203},
            {4.6211, 3.5458, -3.3834, 2.6365, 0.3826, -0.0325, 0.0589, 0.0949},
            {-5.3598, -0.3134, -0.0280, -0.3899, -1.5811, -0.5843, 0.3617, 0.6277}
        };
        Fb1 = { -2.3732, 6.5296, -4.0057, -0.6197, -1.7702, -4.2693, -0.3520, 5.4905
        };
        Fwiy2 = {
            {0.3167, 3.9151, 2.7356, 0.463, -1.7311, -2.8706, -0.947, -5.516}
        };
        Fb2 = { 0.4672 };
    }

    // Forward propagation prediction function
    double predict(const std::vector<double>& input) {
        std::vector<double> hidden1 = tansig(vectorAdd(matrixMultiply(Fwix, input), Fb0));
        std::vector<double> hidden2 = tansig(vectorAdd(matrixMultiply(Fwiy1, hidden1), Fb1));
        double output = purelin(vectorAdd(matrixMultiply(Fwiy2, hidden2), Fb2));
        return output;
    }
};

// Normalization function
double normalize(double x, double min, double max) {
    return -1.0 + 2.0 * (x - min) / (max - min);
}

// Denormalization function
double denormalize(double y, double min, double max) {
    return min + (max - min) * (y + 1.0) / 2.0;
}

// STDP voltage model
double Get_Voltage_STDP(int t, int LIF_Point) {
    int dt = t - LIF_Point;
    double voltage;
    if (dt >= 0) {
        voltage = Par.pmax - ((2 * (Par.pmax - Par.pmin)) / Par.Period_STDP) * dt;
    }
    else {
        voltage = Par.nmax + ((2 * (Par.nmax - Par.nmin)) / Par.Period_STDP) * dt;
    }
    if (voltage > 0 && voltage > Par.pmax) { voltage = Par.pmax; }
    if (voltage > 0 && voltage < Par.pmin) { voltage = Par.pmin; }
    if (voltage < 0 && voltage > Par.nmin) { voltage = Par.nmin; }
    if (voltage < 0 && voltage < Par.nmax) { voltage = Par.nmax; }
    return voltage;
}

// STDP function using ANN
double STDP(int t, int LIF_Point, double Resistance) {
    static NeuralNetwork nn;  // Static instance to avoid repeated initialization

    double Voltage_Applied = Get_Voltage_STDP(t, LIF_Point);
    double Current_NOW = (Par.V_Read / Resistance) / (Par.Width_Fe * Par.Length_Fe * 1e4);

    // Normalize input
    double V_min = -1.4;
    double V_max = 0.7;
    double I_min = 0.0764;
    double I_max = 0.8580;
    double dI_min = -0.6841;
    double dI_max = 1.3478;

    double V_test = normalize(Voltage_Applied, V_min, V_max);
    double I_test = normalize(Current_NOW, I_min, I_max);

    // Use ANN to predict Ratio
    std::vector<double> input = { V_test, I_test };
    double y_pred = nn.predict(input);
    double Ratio = denormalize(y_pred, dI_min, dI_max);

    double Current_THEN = (1 + Ratio) * Current_NOW;
    double Resistance_THEN = (Par.V_Read / Current_THEN) / (Par.Width_Fe * Par.Length_Fe * 1e4);

    if (Resistance_THEN < Par.ResMin) { Resistance_THEN = Par.ResMin; }
    if (Resistance_THEN > Par.ResMax) { Resistance_THEN = Par.ResMax; }

    return Resistance_THEN;
}

// Each change is related to the previous change, to be confirmed
double STDP(int t, const std::vector<int>& LIF_Sequence, double Resistance) {
    double Res = Resistance;
    for (int LIF_Point : LIF_Sequence) {
        Res = STDP(t, LIF_Point, Res);
    }
    return Res;
}
