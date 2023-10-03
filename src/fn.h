void ReadAnalyzeFile(string filePath){
    file *f = new file(filePath);
    if (f->IsValidType()){
        f->ReadFile();
        f->LandFit();
        f->FillHistos();
        f->WriteOut();
    }
}

void WriteOutFitGraph(string filePath, int i){

        file *f = new file(filePath);
        if (f->IsValidType()){
            f->ReadFile();
            f->LandFit();
            f->WriteOutFit(i);
    }
}

void ReadAnalyzeFolder(const std::string& folderPath) {
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            ReadAnalyzeFile(entry.path().string());
        } else if (entry.is_directory()) {
            ReadAnalyzeFolder(entry.path().string());
        }
   }
}