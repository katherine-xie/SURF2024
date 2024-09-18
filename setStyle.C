#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include "SURF_FUNCTIONS/SURF_FUNCTIONS.h"
R__LOAD_LIBRARY(./SURF_FUNCTIONS/SURF_FUNCTIONS_C.so);


void setStyle() {

    TStyle *style = new TStyle("style", "Style for a Split Canvas"); 

    // Canvas settings 
	style->SetCanvasColor(0); 
	style->SetCanvasBorderMode(0); 
	style->SetCanvasBorderSize(10); 

    // Axis settings
    style->SetLabelFont(44, "XYZ"); // Font applied to X, Y, and Z axes
    style->SetLabelSize(0.05, "XYZ");
    style->SetTitleFont(44, "XYZ");
    style->SetTitleSize(0.06, "XYZ");

}