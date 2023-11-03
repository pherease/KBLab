// Libs
#include <iostream>
#include <any>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <string>
#include <typeinfo>

#include <H5Cpp.h>
#include <hdf5.h>

// Root Headers
#include <TROOT.h>
#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1F.h>
#include <TError.h>
#include <TTree.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TFitter.h>
#include "Math/Minimizer.h"
// Namespaces

namespace fs = std::filesystem;
using namespace std;
using namespace H5;

// Settings

// Escape Sequences
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m" 
#define YELLOW  "\033[33m"  
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"   
#define CYAN    "\033[36m"
#define WHITE   "\033[37m" 

// Classes

#include "file.h"
#include "fn.h"