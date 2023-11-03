void WriteOutFitGraph(string filePath, int i){
    file *f = new file(filePath);
    if (f->IsValidType()){
        f->ReadFile();
        f->LandFit();
        f->WriteOutFit(i);
    }
    delete f;
}

void ReadAnalyzeFile(string filePath, string plot_opt){
    file *f = new file(filePath);
    if (f->IsValidType()){
        f->SetPlotOpt(plot_opt);
        f->ReadFile();
        f->LandFit();
        f->FillHistos();
        f->WriteOut();
    }
    delete f;
}

void ReadAnalyzeFolder(const std::string& folderPath, string plot_opt) {
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            ReadAnalyzeFile(entry.path().string(), plot_opt);
        } else if (entry.is_directory()) {
            ReadAnalyzeFolder(entry.path().string(), plot_opt);
        }
   }
}