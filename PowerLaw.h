#pragma once
#include <vector>

inline double pow2(double x) { return(x * x); }

bool PowerLaw(std::vector<double> x, std::vector<double> y, double *retSlope, double *retIntercept);
