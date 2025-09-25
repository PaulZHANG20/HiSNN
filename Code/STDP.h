#ifndef STDP_H
#define STDP_H

#include <vector>
#include "Param.h"
#include "LookUp.h"

double Get_Voltage_STDP(int t, int LIF_Point);
double STDP(int t, int LIF_Point, double Resistance);
double STDP(int t, const std::vector<int>& LIF_Sequence, double Resistance);

#endif
