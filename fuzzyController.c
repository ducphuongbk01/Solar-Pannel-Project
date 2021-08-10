/***************************LIBRARY DEFINITION*****************************/

#include <stdio.h>
#include <math.h>
#include "fuzzyController.h"

double hlt_hinhthang(double x, double L, double c1, double c2, double R);
double fuzzyController(double ek_std, double edot_std);
double controller(double vU, double vD, double Ts);
double saturation( double a, double rangeBot, double rangeTop);
double controller2 (double vR, double vL, double Ts);

double ek_1 = 0,ek;
double ek2,ek2_1=0;
double udot_1 = 0, output_1 = 0; 
double output2_1 = 0, udot2_1;


double hlt_hinhthang(double x, double L, double c1, double c2, double R)
{
    if (x < L)
        return 0;
    else if (x<c1)
        return (x-L)/(c1-L);
    else if (x<c2)
        return 1;
    else if (x<R)
        return (R-x)/(R-c2);
    else
        return 0;
}

double fuzzyController(double ek_std, double edot_std)
{
    double e_NB, e_NS, e_ZE, e_PS, e_PB;
    double edot_NE, edot_ZE, edot_PO;
    double y_NB, y_NM, y_NS, y_ZE, y_PS, y_PM, y_PB;
    double y[15];
    double beta[15];
    double num = 0, den = 0;
    double udot = 0;

    //Mo hoa e
    e_NB = hlt_hinhthang(ek_std, -3, -2, -0.8, -0.4);
    e_NS = hlt_hinhthang(ek_std, -0.6, -0.45, -0.35, 0);
    e_ZE = hlt_hinhthang(ek_std, -0.2, -0.05, 0.05, 0.2);
    e_PS = hlt_hinhthang(ek_std, 0, 0.35, 0.45, 0.6);
    e_PB = hlt_hinhthang(ek_std, 0.4, 0.8, 2, 3);

    //Mo hoa edot
    edot_NE = hlt_hinhthang(edot_std, -3, -2, -0.4, 0);
    edot_ZE = hlt_hinhthang(edot_std, -0.2, -0.05, 0.05, 0.2);
    edot_PO = hlt_hinhthang(edot_std,0, 0.4, 2, 3);


    //Khai bao bien ra
    y_NB = -1;
    y_NM = -0.6;
    y_NS = -0.3;
    y_ZE = 0;
    y_PS = 0.3;
    y_PM = 0.6;
    y_PB = 1;

    //Cac quy tac mo - dung menh de dieu kien
    beta[0] = e_NB * edot_NE; y[0] = y_PB;
    beta[1] = e_NB * edot_ZE; y[1] = y_PM;
    beta[2] = e_NB * edot_PO; y[2] = y_PS;

    beta[3] = e_NS * edot_NE; y[3] = y_PM;
    beta[4] = e_NS * edot_ZE; y[4] = y_PS;
    beta[5] = e_NS * edot_PO; y[5] = y_ZE;

    beta[6] = e_ZE * edot_NE; y[6] = y_PS;
    beta[7] = e_ZE * edot_ZE; y[7] = y_ZE;
    beta[8] = e_ZE * edot_PO; y[8] = y_NS;

    beta[9] = e_PS * edot_NE; y[9] = y_ZE;
    beta[10] = e_PS * edot_ZE; y[10] = y_NS;
    beta[11] = e_PS * edot_PO; y[11] = y_NM;

    beta[12] = e_PB * edot_NE; y[12] = y_NS;
    beta[13] = e_PB * edot_ZE; y[13] = y_NM;
    beta[14] = e_PB * edot_PO; y[14] = y_NB;

    //Giai mo dung phuong phap trung binh co trong so
    for (int i = 0; i<15; i++)
        {
            //printf("%f  %f \n", beta[i], y[i]);
            num += beta[i]*y[i];
            den += beta[i];
        }
    udot = num/den;
    return udot;
}

double controller(double vU, double vD, double Ts)
{
    double edot;
    double output, udot;
    ek = vD - vU;
    edot = (ek - ek_1)/Ts;
	  ek_1 = ek;

    //Standardize ek and edot to [-1;1]
    ek = saturation(ek/3.3,-1,1);
    edot = saturation(edot/0.1,-1,1);

    //Fuzzy Controller
    udot = fuzzyController(ek, edot);
    output = output_1 + 0.5*(udot_1 + udot)*Ts*180;
    output = saturation(output,10,90);
    udot_1 = udot;
    output_1 = output;
    return output;
}

double controller2 (double vR, double vL, double Ts)
{
	double edot;
	double output2, udot;
	ek2 = vR - vL;
	edot = (ek2 - ek2_1)/Ts;
	ek2_1 = ek2;
	
	ek2 = saturation(ek2/3.3,-1,1);
	edot = saturation(edot/0.1,-1,1);
	
	udot = fuzzyController(ek2, edot);
	output2 = output2_1 +0.5*(udot2_1 + udot)*Ts*180;
	output2 = saturation(output2,10,170);
	udot2_1 = udot;
	output2_1 = output2;
	return output2;
}

double saturation( double a, double rangeBot, double rangeTop)
{
    if (a>rangeTop)
        a=rangeTop;
    else if (a<rangeBot)
        a=rangeBot;
    else
        a=a;
    return a;
}
