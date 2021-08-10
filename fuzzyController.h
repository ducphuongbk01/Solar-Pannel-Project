/************************LIBRARY DECLEARATION***************************/

double hlt_hinhthang(double x, double L, double c1, double c2, double R);

double fuzzyController(double ek_std, double edot_std);

double controller(double vU, double vD, double Ts);

double saturation( double a, double rangeBot, double rangeTop);

double controller2 (double vR, double vL, double Ts);