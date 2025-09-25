#ifndef LIF_H
#define LIF_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "Param.h"
#include "LookUp.h"


double Get_Voltage_LIF(
	double Resistance, int Period_Charge, double V_PL, double V_Rst, double C_Fe);

double LIF(int t, const std::vector<int>& LIF_Sequence, double Resistance);

#endif 
