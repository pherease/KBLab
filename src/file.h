class file{
    double TIMESTEP = 2.5e-9;
    string filePath;
    string fileName;
    string fileType;
    string folderName;
    string outFolderDir;
    string outFileDir;
    string unSuccFolderDir;
    int n_events;
    int n_dPoints;
    bool read_check = false;

    int n_succesfulFits = 0;
    string scintillator;
    string source;
    string date;
    string detector;
    int set_no;
    double ampRatio;
    double threshold;
    double samplingRate;
    int n_segment;
    int n_pointsPerEvent;

    
    vector<double> time_code = {};
    vector<vector<double>> matrix; 
    vector<vector<double>> fit_output = {{}, {}, {}, {}, {}};
    vector<int> SuccFitsInd;
    vector<TGraph> SuccFitsGraphs;
    vector<int> unSuccFitsInd;
    vector<TGraph> unSuccFitsGraphs;
    vector<TGraph> fitGraphs;
    vector<TH1F*> histograms;
    vector<TFitResultPtr> fitPResVec = {};
    TTree* fTree = new TTree("fTree", "Metadata Tree");

    public:
        void errReadFirst(){
            cout << YELLOW << "ERROR: Read and Fit the data first." << endl;
        }

        void errFillHistFirst(){

            cout << YELLOW << "ERROR: Fill the histograms first to write its output." << endl;           
        }

        file(string filePath_){
            filePath = filePath_;

            // Obtains the file name and its extension, without its directory.
            size_t slashPos_1 = filePath.find_last_of("/\\");
            fileName = filePath.substr(slashPos_1 + 1);
            
            // Obtains the file extension. (txt), (pdf) etc.
            size_t lastDotPos = filePath.find_last_of('.');
            fileType = filePath.substr(lastDotPos + 1);

            size_t slashPos_2 = filePath.substr(0, slashPos_1).find_last_of("/\\");
            folderName = filePath.substr(slashPos_2 + 1, slashPos_1 - slashPos_2 - 1);
            
            outFolderDir = filePath.substr(0, filePath.size() - fileName.size() - folderName.size() - 6) + "out/" + folderName;
            if ((fileType == "txt")||(fileType == "h5")){
                istringstream iss(fileName);
                string temp;

                while (getline(iss, temp, '_')) {
                    if((temp.find("Cs") != -1) || (temp.find("Ba") != -1) || (temp.find("Co") != -1 || (temp.find("NoSource") != -1))){
                        source = temp;
                    }
                    else if ((temp.find("EJ") != -1) || (temp.find("CR") != -1)){
                        scintillator = temp;
                    }
                    else if(temp.find("Seg") != -1){
                        string temp_str = temp.substr(0, temp.size() - 3);
                        n_segment = stoi(temp_str);
                    }
                    else if(temp.find("Amp") != -1){
                        string temp_str = temp.substr(3, temp.size() - 3);
                        ampRatio = stod(temp_str);
                    }
                    else if(temp.find("Th") != -1){
                        string temp_str = temp.substr(2, temp.size() - 4);
                        threshold = stod(temp_str);
                    }
                    else if((temp.find("SiPM") != -1) || (temp.find("PMT") != -1)){
                        detector = temp;
                    }
                    else if(temp.find("MSps") != -1){
                        string temp_str = temp.substr(0, temp.size() - 4);
                        samplingRate = stod(temp_str);
                    }
                    else if(temp.find("Sample") != -1){
                        string temp_str = temp.substr(0, temp.size() - 6);
                        n_pointsPerEvent = stoi(temp_str);
                    }
                    else {
                        set_no = stoi(temp);
                    }
                    
                }
                read_check = true;

                size_t underPos = folderName.find_last_of("_");
                date = folderName.substr(0, underPos) + "/" + folderName.substr(underPos + 1);

                outFileDir = outFolderDir + "/" + fileName.substr(0, fileName.size() - fileType.size() - 1);

                if (!fs::exists(outFileDir)) {
                    fs::create_directories(outFileDir);
                }

            }
        }       
        
        string GetFileName(){
            if (read_check){
                return fileName;
            }

            else{
                errReadFirst();
            }
        }
        
        string GetoutFolderDir(){
            if (read_check){
                return outFolderDir;
            }

            else{
                errReadFirst();
            }            
        }

        string GetFileType(){
            if (read_check){
                return fileType;
            }

            else{
                errReadFirst();
            }
        }

        vector<vector<double>> GetMatrix(){
            if (read_check){
                return matrix;
            }

            else{
                errReadFirst();
            }
        }
        
        /// @brief  Returns analyzed data of all events in the file.
        vector<vector<double>> GetFitResults(){
            if (read_check){
                return fit_output;
            }

            else{
                errReadFirst();
            }
        }
        
        string GetScintillator(){
            if (read_check){
                return scintillator;
            }
            else{
                errReadFirst();
            }
        }

        string GetSource(){
            if (read_check){
                return source;
            }
            else{
                errReadFirst();
            }
       }

        string GetDate(){
            if (read_check){
                return date;
            }
            else{
                errReadFirst();
            }
        }

        int GetSetNo(){
            if (read_check){
                return set_no;
            }
            else{
                errReadFirst();
            }
        }

        double GetThreshold(){
            if (read_check){
                return threshold;
            }
            else{
                errReadFirst();
            }
        }
        
        vector<double> GetTimeCode(){
            return time_code;
        }

        int GetsuccessfulFits(){
            if (read_check){
                return n_succesfulFits;
            }
            else{
                errReadFirst();
            }
        }
        
        vector<TH1F*> GetHistograms(){
            if (histograms.empty()){
                errFillHistFirst();
            }
            else{
                return histograms;
            }
        }

        /// @brief Changes the time-step value. Default value is 2.5e-9.
        void ChangeTimeStep(double new_step){
            TIMESTEP = new_step;
        }
        
        bool IsValidType(){
            if((fileType == "txt")||(fileType == "h5")){
                return true;
            }
            else{
                cout << RED << "File type is not valid: ." << YELLOW  << fileType << RESET << endl;
                return false;
            }
        }
        
        /// @brief Reads the file located in the given path. Transposes and stores the data in it as file.matrix variable.
        void ReadFile(){
            cout << GREEN << "Reading: " << RESET << fileName << endl; 

            if (fileType == "txt"){ // .h5 file reader.
                ifstream inpFile;
                inpFile.open(filePath);

                string line;
                double value;

                while (getline(inpFile, line)){
                    istringstream iss(line);
                    int i_col = 0;
                    if (matrix.empty()){
                        while (iss >> value) {
                            matrix.push_back({-1*value});
                        }
                    }
                    else {
                        while (iss >> value) {
                            matrix[i_col].push_back(-1*value);
                            i_col += 1;
                        }
                    }
                }
                n_events = matrix.size();
                n_dPoints = matrix[0].size();
            }  
            else if (fileType == "h5"){ // .h5 file reader.

                H5File hFile(filePath, H5F_ACC_RDONLY);
                string dsetHead = "/Waveforms/Channel 1/Channel 1 Seg";
                string dsetTail = "Data";

                Group count_group = hFile.openGroup("/Waveforms/Channel 1");

                hsize_t size = count_group.getNumObjs();
                int n_events = int(size);

                for (int i = 0; i < n_events; i++){
                    string datasetPath = dsetHead + to_string(i + 1) + dsetTail;

                    DataSet dset = hFile.openDataSet(datasetPath.c_str());

                    DataSpace dspace = dset.getSpace();

                    hsize_t dims[2];
                    dspace.getSimpleExtentDims(dims, NULL);
                    hsize_t dimsm[1];
                    dimsm[0] = dims[0];
                    DataSpace memspace(1, dimsm);

                    vector<double> temp;
                    temp.resize(dims[0]);
                    dset.read(temp.data(), PredType::NATIVE_DOUBLE, memspace, dspace);

                    for (int j = 0; j < dims[0]; j++) {
                        temp[j] *= -1;
                    }

                    matrix.push_back(temp);

                    if (n_dPoints != dims[0]){
                        n_dPoints = dims[0];
                    }
                }
            }
            for (int i = 0; i < n_dPoints; i++){
                time_code.push_back(TIMESTEP*i);
            }
        }
        
        /// @brief Takes the matrix obtained by ReadFile() argument and applies Landau fit to each event in the matrix. Stores important variables like rise time, fall time, peak voltage etc. Get those results by using GetFitResults() method.
        void LandFit(){
            int ind = 0;
            for (vector<double> event: matrix){
                TGraph *gr = new TGraph(n_dPoints, time_code.data(), event.data());
                gr->SetTitle("");

                TF1 *land_fit = new TF1 ("land_fit","[0]*TMath::Landau(x,[1],[2])", time_code[0], time_code.back()); 

                auto maxVoltIt = max_element(event.begin(), event.end());
                double maxVolt = *maxVoltIt;
                int maxVoltInd = distance(event.begin(), maxVoltIt);

                land_fit->SetParameters(maxVolt, maxVoltInd*TIMESTEP, TIMESTEP);

                TFitResultPtr fit_result = gr->Fit(land_fit, "Q");
                fitPResVec.push_back(fit_result);

                Int_t fitStatus = fit_result;

                if (!fit_result) {
                    n_succesfulFits += 1;

                    vector<double> peak = {land_fit->GetMaximumX(), land_fit->GetMaximum()};
                    
                    double v_high = peak[1]*0.8;
                    double v_low = peak[1]*0.2;

                    double rise_l = land_fit->GetX(v_low, time_code[0], peak[0]);
                    double rise_h = land_fit->GetX(v_high, time_code[0], peak[0]);
                    double fall_l = land_fit->GetX(v_low, peak[0], time_code.back());
                    double fall_h = land_fit->GetX(v_high, peak[0], time_code.back());

                    double rise_t = rise_h - rise_l;
                    double fall_t = fall_l - fall_h;
                    double auc = land_fit->Integral(0, time_code.back());

                    fit_output[0].push_back(rise_t);
                    fit_output[1].push_back(fall_t);
                    fit_output[2].push_back(auc);
                    fit_output[3].push_back(peak[0]);
                    fit_output[4].push_back(peak[1]);

                    SuccFitsInd.push_back(ind);
                    TGraph gr_temp = *gr;
                    fitGraphs.push_back(gr_temp);
                    SuccFitsGraphs.push_back(gr_temp);

                }
                else{
                    unSuccFitsInd.push_back(ind);
                    TGraph gr_temp = *gr;
                    fitGraphs.push_back(gr_temp);
                    unSuccFitsGraphs.push_back(gr_temp);
                }
                ind += 1;
                delete gr;
                delete land_fit;
            }
        }
        
        /// @brief Returns successfull fits to all fits ratio in the string form.  
        string successfulFitRate(){
            if (n_events != 0){
                return to_string(n_succesfulFits) + "/" + to_string(n_events);
            }
            else{
                cout << YELLOW << "ERROR: Read and Fit the data first." << endl;
            }

        }

        void FillHistos(){
            if (fit_output.empty()){
                errFillHistFirst();
            }
            
            else {
                vector<double> bounds;
                for (int i = 0; i < fit_output.size(); i++){
                    double d_min = *min_element(fit_output[i].begin(), fit_output[i].end());
                    double d_max = *max_element(fit_output[i].begin(), fit_output[i].end());
                    bounds.push_back(d_min);
                    bounds.push_back(d_max);
                }

                TH1F *hist_rise_t = new TH1F("hist_rise_t", "Rise Time", 10, bounds[0], bounds[1]);
                TH1F *hist_fall_t = new TH1F("hist_fall_t", "Fall Time", 10, bounds[2], bounds[3]);
                TH1F *hist_auc = new TH1F("hist_auc", "Area Under the Curve", 10, bounds[4], bounds[5]);
                TH1F *hist_peak_x = new TH1F("hist_peak_x", "Peak Time", 10, bounds[6], bounds[7]);
                TH1F *hist_peak_y = new TH1F("hist_peak_y", "Peak Voltage", 10, bounds[8], bounds[9]);

                hist_rise_t->FillN(n_succesfulFits, &fit_output[0][0], NULL);
                hist_fall_t->FillN(n_succesfulFits, &fit_output[1][0], NULL);
                hist_auc->FillN(n_succesfulFits, &fit_output[2][0], NULL);
                hist_peak_x->FillN(n_succesfulFits, &fit_output[3][0], NULL);
                hist_peak_y->FillN(n_succesfulFits, &fit_output[4][0], NULL);

                histograms.push_back(hist_rise_t);
                histograms.push_back(hist_fall_t);
                histograms.push_back(hist_auc);
                histograms.push_back(hist_peak_x);
                histograms.push_back(hist_peak_y);
            }
        }

        void WriteOutFit(int i){
            TGraph *gr_ = new TGraph();
            gr_ = &fitGraphs[i];

            string title = "Event No: " + to_string(SuccFitsInd[i]);
            string savePath = to_string(i) +".pdf";
            
            TCanvas *c = new TCanvas("c", title.c_str(), 1280, 1024);
            gr_->SetTitle(title.c_str());
            gr_->GetXaxis()->SetTitle("Time (s)");
            gr_->GetYaxis()->SetTitle("Voltage (V)");
            gr_->Draw("");

            TFitResultPtr temp_fitRes = fitPResVec[i];

/*
            double scale = temp_fitRes->GetParameter(0);
            double mu = temp_fitRes->GetParameter(1);
            double sigma = temp_fitRes->GetParameter(2);

            cout << YELLOW << "Fit Index: "<< RESET << i << endl;
            cout << YELLOW << "Scale Parameter: " << RESET << scale << endl;
            cout << YELLOW << "Mu Parameter: " << RESET << mu << endl;
            cout << YELLOW << "Sigma Parameter: " << RESET << sigma << endl;
*/
            c->SaveAs(savePath.c_str());

            delete c;
        }

        void WriteOutUnSuccFits(){
            unSuccFolderDir = outFileDir + "/unSuccFits";

            for (int i = 0; i < unSuccFitsGraphs.size(); i++){
                if (!fs::exists(unSuccFolderDir)) {
                    fs::create_directories(unSuccFolderDir);
                }

                TGraph *gr_ = new TGraph();
                gr_ = &(unSuccFitsGraphs[i]);

                string title = "Event No: " + to_string(unSuccFitsInd[i]);
                string savePath = unSuccFolderDir + "/" + to_string(unSuccFitsInd[i]) + ".pdf";
                
                TCanvas *c = new TCanvas("c", title.c_str(), 1280, 1024);
                gr_->SetTitle(title.c_str());
                gr_->GetXaxis()->SetTitle("Time (s)");
                gr_->GetYaxis()->SetTitle("Voltage (V)");
                gr_->Draw("");


                c->SaveAs(savePath.c_str());

                delete c;

            }
        }

        void WriteOut(){
            if (histograms.empty()){
                errFillHistFirst();
            }
            else{
                TFile *outFile = new TFile((outFileDir + "/output.root").c_str(), "RECREATE");

                int temp_n_succesfulFits;
                string temp_scintillator;
                string temp_source;
                string temp_date;
                string temp_detector;
                int temp_set_no;
                double temp_ampRatio;
                double temp_threshold;
                double temp_samplingRate;
                int temp_n_segment;
                int temp_n_pointsPerEvent;

                for (TH1F* hist: histograms){
                    hist->Write();
                }

                fTree->Branch("n_succesfulFits", &n_succesfulFits);
                fTree->Branch("scintillator", &temp_scintillator);
                fTree->Branch("source", &temp_source);
                fTree->Branch("date", &temp_date);
                fTree->Branch("detector", &temp_detector);
                fTree->Branch("set_no", &temp_set_no);
                fTree->Branch("ampRatio", &temp_ampRatio);
                fTree->Branch("threshold", &temp_threshold);
                fTree->Branch("samplingRate", &temp_samplingRate);
                fTree->Branch("n_segment", &temp_n_segment);
                fTree->Branch("n_pointsPerEvent", &temp_n_pointsPerEvent);

                temp_n_succesfulFits = n_succesfulFits;
                temp_scintillator = scintillator;
                temp_source = source;
                temp_date = date;
                temp_detector = detector;
                temp_set_no = set_no;
                temp_ampRatio = ampRatio;
                temp_threshold = threshold;
                temp_samplingRate = samplingRate;
                temp_n_segment = n_segment;
                temp_n_pointsPerEvent = n_pointsPerEvent;
                
                fTree->Fill();
                fTree->Write();
                outFile->Close();

                WriteOutUnSuccFits();
            }
        }
};