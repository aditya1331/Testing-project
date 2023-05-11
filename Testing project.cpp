#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <map>
#include <filesystem>
#include <windows.h>
#include <unordered_map>
#include <string>
#include <vector>
namespace fs = std::filesystem;

using namespace std;


void vectorPrint(vector<int> vr)
{
    for (auto i : vr)
        cout << i << endl;
}
void readHeader(string& line, vector<string> &header, fstream& myfile)
{
    while (getline(myfile, line)) 
    { 
        if (!line.compare("ENDSEC;"))
            break;
        header.push_back(line);
    }
    
}
void readData(string& line, vector<string>& data, fstream& myfile)
{
    while (getline(myfile, line))
    {
        if (!line.compare("ENDSEC;"))
            break;
        data.push_back(line);
    }
}

int fetchNumber(int i, string str)
{
    i++;
    string temp = "";
    while (str.length() - i)
    {
        if (isdigit(str[i]))
            temp += str[i];
        else
            break;
        i++;
    }
    int hh = stoi(temp);
    return hh;
}
void mappingEntities(vector<string> &data, map<int,vector<int>> &argumentsMap,  vector<int> &argumentsTrack,int datalen)
{
    int i = 0;
    int n = data.size();
    for (auto str: data)
    {
        i = 0;
        char hash = '#';
        char tempChar = '5';
        int key = fetchNumber(i, str);
        i += to_string(key).length();
        for (;i < str.size();i++)
        {
            if (hash==str[i])
            {
                bool compa = strcmp(&hash, &tempChar);
                int temp = fetchNumber(i, str);
                argumentsMap[key].push_back(temp);
                argumentsTrack[temp] = 0;
                i += to_string(temp).length();
            }        
        }
    }
}
string getPath()
{
    SHELLEXECUTEINFO seinfo;
    ZeroMemory(&seinfo, sizeof(seinfo));
    seinfo.cbSize = sizeof(seinfo);
    seinfo.lpVerb = TEXT("open");
    seinfo.lpFile = TEXT("explorer.exe");
    seinfo.lpParameters = TEXT("/select,\"C:\\\"");
    seinfo.nShow = SW_SHOWDEFAULT;
    if (ShellExecuteEx(&seinfo) == FALSE) {
        cerr << "Error: Unable to open folder." << endl;
        
    }

    // Get the selected folder path
    fs::path folderpath(seinfo.lpFile);
    if (fs::exists(folderpath)) {
        cerr << "Error: Invalid folder path." << endl;
        
    }

    return folderpath.string();  
   
  
}

string getPathBySelecting()
{

    std::string path_str;
    std::cout << "Enter a folder path: ";
    std::getline(std::cin, path_str);

    // Change the current working directory to the selected folder
    int result = std::system(("cd " + path_str).c_str());
    if (result != 0) {
        std::cerr << "Error: Unable to change directory." << std::endl;
      
    }

    // Get the current working directory
    char cwd[1024];
    

    // Print the selected folder path
    std::cout << "Selected Folder Path: " << cwd << std::endl;
    return string(cwd);
}

void commonDiaogBox()
{
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name
    // owner window
    HANDLE hf;              // file handle

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
 
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = L'\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box. 

    if (GetOpenFileName(&ofn) == TRUE)
        hf = CreateFile(ofn.lpstrFile,
            GENERIC_READ,
            0,
            (LPSECURITY_ATTRIBUTES)NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            (HANDLE)NULL);
}
vector<int> singleEntitySeperator(vector<int>& argumentsTrack)
{
    vector<int> vr;
    int n = argumentsTrack.size();
    for (int i = 0;i < n;i++)
        if (argumentsTrack[i] == 1)
            vr.push_back(i);
    return vr;
}
void writeEntity(int i, int n,vector<string>& formattedStrings, vector<string>& data, map<int, vector<int>>& argumentsMap)
{
    string temp = "";
    for (auto k = 0;k < n;k++)
        temp += "\t";
    temp += data[i-1];
    formattedStrings.push_back(temp);
    n++;
    for (auto j : argumentsMap[i])
        writeEntity(j, n, formattedStrings, data, argumentsMap); 

}
void createFile(map<int,vector<int>> & argumentsMap, vector<int>& singleEntities, vector<string>& data,vector<string>& header,string ISO)
{
    fstream file;
    int datalen = argumentsMap.size();
    string location = "E:\\Dassult systems intership\\Internship report\\Template\\Trining report\\codes\\Testing project\\Rectangle_sketch_simplif.step";
    file.open(location, ios::out);
    vector<string> formattedStrings;
    for (auto k : singleEntities)
        writeEntity(k, 0,formattedStrings,data,argumentsMap);
    for (auto k : header)
        file << k << endl;
    file << "ENDSEC;" << endl;
    for (auto k : formattedStrings)
        file << k << endl;
    file << "ENDSEC;" << endl;
    file << ISO << endl;
    file.close();
}
int main() {
    fstream myfile("E:\\Dassult systems intership\\Internship report\\Template\\Trining report\\codes\\Testing project\\Rectangle_sketch.step"); // Open the file
    string line,ISO;
    vector<string> header;
    vector<string> data;
    map<int,vector<int>> argumentsMap;
    vector<int> argumentsTrack;
    int datalen;

    if (myfile.is_open()) { 
        readHeader(line, header, myfile);
        getline(myfile, line);
        getline(myfile, line);     
        readData(line, data, myfile);
        datalen = data.size();
        for (int k = 0;k <= datalen;k++)
            argumentsTrack.push_back(1);
        getline(myfile, line);
        ISO = line;
        myfile.close();
        mappingEntities(data, argumentsMap, argumentsTrack,datalen);
        vector<int> singleEntities = singleEntitySeperator(argumentsTrack);
        singleEntities.erase(singleEntities.begin());
        createFile(argumentsMap,singleEntities,data,header,ISO);
    }
    /*string path = getPath();
    cout << path << endl;*/
    //commonDiaogBox();

 
    return 0;
}
