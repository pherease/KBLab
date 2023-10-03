vector<vector<double> > read_file(string file_path){

    ifstream inpFile;
    inpFile.open(file_path);

    string line;
    double value;

    vector<vector<double> > Dpoints;
    vector<double> time_code;

    while (getline(inpFile, line)){
        istringstream iss(line);
        int i_col = 0;
        if (Dpoints.empty()){
            while (iss >> value) {
                vector<double> temp;
                temp.push_back(-1*value);
                Dpoints.push_back(temp);
            }
        }

        else {
            while (iss >> value) {
                Dpoints[i_col].push_back(-1*value);
                i_col += 1;
            }
        }
    }

    for (int i = 0; i<Dpoints[0].size(); i++){
        time_code.push_back(i*2.5e-9);
    }

    vector<vector<double> > data;
    data.push_back(time_code);

    for (int i = 0; i<Dpoints.size(); i++){
        data.push_back(Dpoints[i]);
    }

    return data; // data[0] is just time domain (0, 2.5e-9, 5e-9, 7.5e-9, ... ), real data points starts with data[1]. 
}

vector<double> landau_fit(vector<vector<double> > data, int i){
    TGraph *gr = new TGraph(data[i].size(), &data[0][0], &data[i][0]);
    gr->SetTitle("");

    TF1 *land_fit = new TF1 ("land_fit","[0]*TMath::Landau(x,[1],[2])", data[0][0], data[0].back());
    land_fit->SetParameters(1, 0.125e-6, 2.5e-9); 

    gr->Fit(land_fit, "Q");

    if (gr->GetFunction("land_fit")) {
        vector<double> peak = {land_fit->GetMaximumX(), land_fit->GetMaximum()};
        
        double v_high = peak[1]*0.8;
        double v_low = peak[1]*0.2;

        double rise_l = land_fit->GetX(v_low, data[0][0], peak[0]);
        double rise_h = land_fit->GetX(v_high, data[0][0], peak[0]);
        double fall_l = land_fit->GetX(v_low, peak[0], data[0].back());
        double fall_h = land_fit->GetX(v_high, peak[0], data[0].back());

        double rise_t = rise_h - rise_l;
        double fall_t = fall_l - fall_h;
        double auc = land_fit->Integral(0, data[0].back());

        vector<double> res = {rise_t, fall_t, auc, peak[0], peak[1]};

        delete gr;
        delete land_fit;

        return res;
    }

    else {
        vector<double> res = {-1.0, -1.0, -1.0, -1.0, -1.0};

        delete gr;
        delete land_fit;

        return res;
    }


}

void write_csv(const vector<vector<double> >& matrix, const string& filename) {
    ofstream file(filename);
    for (const auto& row : matrix) {
        for (size_t j = 0; j < row.size(); ++j) {
            file << row[j];
            if (j != row.size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }
    file.close();
}

void write_json(unordered_map<string, any> summ, string output_file){
    ofstream jsonFile(output_file);
    jsonFile << "{\n";
    bool first = true;
    
    for (const auto& pair : summ) {
        if (!first) {
            jsonFile << ",\n";
        }
        
        jsonFile << "  \"" << pair.first << "\": ";

        if (pair.second.type() == typeid(int)) {
            jsonFile << any_cast<int>(pair.second);
        } else if (pair.second.type() == typeid(double)) {
            jsonFile << any_cast<double>(pair.second);
        } else if (pair.second.type() == typeid(string)) {
            jsonFile << "\"" << any_cast<string>(pair.second) << "\"";
        }
        else if (pair.second.type() == typeid(vector<double>)) {
                const vector<double>& vec = any_cast<vector<double>>(pair.second);
                jsonFile << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i != 0) {
                        jsonFile << ", ";
                    }
                    jsonFile << vec[i];
                }
                jsonFile << "]";
        } else {
            jsonFile << "\"<unknown type>\"";
        }

        first = false;
    }
    jsonFile << "\n}\n";
    jsonFile.close();
}

vector<string> get_file_paths(){

    vector<string> filePaths;

    for (const auto &entry : __fs::filesystem::directory_iterator("/Users/efemyuksel/Desktop/KB/data")) {
        if (entry.is_regular_file()) {
            filePaths.push_back("/Users/efemyuksel/Desktop/KB/data/" + entry.path().filename().string());
        }
    }
    return filePaths;
}

unordered_map<string, any> dset_summ(string filePath){
    size_t slashPos = filePath.find_last_of("/\\");
    string filename = filePath.substr(slashPos + 1);

    vector<string> parts;
    istringstream iss(filename);
    string temp;

    while (getline(iss, temp, '_')) {
        parts.push_back(temp);
    }

    unordered_map<string, any> summ;

    summ["source"] = parts[0];
    summ["scintillator"] = parts[1];
    summ["set_size"] = stoi(parts[2].substr(0, parts[2].size() - 3));
    summ["threshold"] = stod(parts[3].substr(2, parts[3].size() - 4));
    summ["set_no"] = stoi(parts[4]);

    vector<vector<double> > data = read_file(filePath);    

    vector<vector<double> > res;
    vector<double> means{0, 0, 0, 0, 0};

    for (int i = 1; i < data.size(); i++){
        res.push_back(landau_fit(data, i));
        for (int j = 0; j < means.size(); j++){
            means[j] += res[i-1][j];
        }
    }

    for (int j = 0; j < means.size(); j++){
        means[j] = means[j]/data.size();
    }
    summ["means"] = means;

    return summ;
}

void read_fill_histo(string filePath){
    string filetype = filePath.substr(filePath.find_last_of('.') + 1);
    size_t slashPos = filePath.find_last_of("/\\");
    string filename = filePath.substr(slashPos + 1);

    vector<int> = non_fit_list;
    if (filetype == "txt"){
        vector<vector<double> > data = read_file(filePath);
        vector<vector<double> > res;

        for (int i = 1; i < data.size(); i++){
            vector<double> l_fit = landau_fit(data, i);
            if (l_fit != vector<double>{-1.0, -1.0, -1.0, -1.0, -1.0}){ // Checking Landau Fit is succesful or not.

                if (res.empty()){
                    for (size_t j = 0; j < l_fit.size(); j++){
                        vector<double> temp = {l_fit[j]};
                        res.push_back(temp);
                    }
                }
                else {
                    for (size_t j = 0; j < l_fit.size(); j++){
                        res[j].push_back(l_fit[j]);
                    }
                }
            }

            else{
                
            }


        } // Res is a 5 x Dataset_size matrix. [0], [1], [2], [3], [4] -> rise_t, fall_t, auc, peak_x, peak_y
        
        vector<double> bounds;

        for (int i = 0; i < 5; i++){
            bounds.push_back(*min_element(res[i].begin(), res[i].end()));
            bounds.push_back(*max_element(res[i].begin(), res[i].end()));
        }

        TH1F *hist_rise_t = new TH1F("hist_rise_t", "Rise Time", 10, bounds[0], bounds[1]);
        TH1F *hist_fall_t = new TH1F("hist_fall_t", "Fall Time", 10, bounds[2], bounds[3]);
        TH1F *hist_auc = new TH1F("hist_auc", "Area Under the Curve", 10, bounds[4], bounds[5]);
        TH1F *hist_peak_x = new TH1F("hist_peak_x", "Peak Time", 10, bounds[6], bounds[7]);
        TH1F *hist_peak_y = new TH1F("hist_peak_y", "Peak Voltage", 10, bounds[8], bounds[9]); 

        for (int i = 0; i < res[0].size(); i++){
            hist_rise_t->Fill(res[0][i]);
            hist_fall_t->Fill(res[1][i]);
            hist_auc->Fill(res[2][i]);
            hist_peak_x->Fill(res[3][i]);
            hist_peak_y->Fill(res[4][i]);
        }

        size_t slashPos = filePath.find_last_of("/\\");
        size_t dotPos = filePath.find_last_of("."); 
        string filename = filePath.substr(slashPos + 1, dotPos - slashPos - 1);

        string histo_dir = "/Users/efemyuksel/Desktop/KB/out/" + filename;
        if (!fs::exists(histo_dir)) {
            fs::create_directories(histo_dir);
        }


        TFile *histograms = new TFile((histo_dir + "/histograms.root").c_str(), "RECREATE");
        histograms->cd();
        hist_rise_t->Write();
        hist_fall_t->Write();
        hist_auc->Write();
        hist_peak_x->Write();
        hist_peak_y->Write();
        histograms->Close();

        delete histograms;
        delete hist_rise_t;
        delete hist_fall_t;
        delete hist_auc;
        delete hist_peak_x;
        delete hist_peak_y; 
    }
    else{
        cout << RED << "Not a valid file format (." << YELLOW << filetype << RED << ")" << endl;
    }
}

void write_metadata(string filePath){
    if (filePath.substr(filePath.length() - 4, 4) == ".txt"){
        size_t slashPos = filePath.find_last_of("/\\");
        string filename = filePath.substr(slashPos + 1);

        vector<string> parts;
        istringstream iss(filename);
        string temp;

        while (getline(iss, temp, '_')) {
            parts.push_back(temp);
        }

        unordered_map<string, any> summ;

        summ["source"] = parts[0];
        summ["scintillator"] = parts[1];
        summ["set_size"] = stoi(parts[2].substr(0, parts[2].size() - 3));
        summ["threshold"] = stod(parts[3].substr(2, parts[3].size() - 4));
        summ["set_no"] = stoi(parts[4]);

        size_t dotPos = filePath.find_last_of("."); 
        string filename_ = filePath.substr(slashPos + 1, dotPos - slashPos - 1);
        string metadata_path = "/Users/efemyuksel/Desktop/KB/out/" + filename_ + "/metadata.json";
        write_json(summ, metadata_path);
    }
}