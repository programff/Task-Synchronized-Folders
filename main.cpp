#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <filesystem>
#include <string> 

using namespace std;

void logToFileAndConsole(const string& message, const string& logFilePath) {
    // Log message to console
    cout << message << endl;
    
    // Log message to file
    ofstream logfile(logFilePath, ios_base::app);
    if (logfile.is_open()) {
        logfile << message << endl;
        logfile.close();
    } else {
        cerr << "Failed to open log file for writing" << endl;
    }
}

//Creates Source and Replica folders if they don't exist already
void createFolders(const string& sourceDir, const string& replicaDir,const string&logFilePath) {
    // Check if Source directory exists, if not, creates it
    if (!filesystem::exists(sourceDir)) {
            filesystem::create_directory(sourceDir);
            logToFileAndConsole("Source directory created: ",logFilePath);
    }
    else {
       logToFileAndConsole("Source directory already exists: ",logFilePath);
    }

    // Check if replica directory exists, if not, creates it
    if (!filesystem::exists(replicaDir)) {
       
            filesystem::create_directory(replicaDir);
            logToFileAndConsole("Replica directory created: ",logFilePath);
        } 
    else {
        logToFileAndConsole("Replica directory already exists: ",logFilePath);
    }
}
//Iterates through Source to check for any new files that aren't in replica and copy them
void synchronizeFolders(const string& sourceDir, const string& replicaDir,const string& logFilePath) {
    string fileName;
    string filePath;
    string replicaFilePath;
    
    for (const auto & pathIteration : filesystem::directory_iterator(sourceDir)){
        filePath=pathIteration.path().u8string();
        fileName=filePath.replace(0,sourceDir.length(),"");
        replicaFilePath=replicaDir+fileName;

        //If the current path leads to a directory and it doesn't exist in Replica it's copied 
        //If it exists we move into that directory and iterate it to check for any new files in it
        if(filesystem::is_directory(pathIteration.path())){
            if(!filesystem::exists(replicaFilePath)){
                filesystem::copy(pathIteration.path(),
                (filesystem::path)replicaFilePath,filesystem::copy_options::none);
                logToFileAndConsole("Folder "+fileName+" Copied",logFilePath);
                }

            synchronizeFolders(pathIteration.path().u8string(),replicaFilePath,logFilePath);
        }
        //If the current path leads to a file and it doesn't exist in Replica it's copied
        else if(!filesystem::exists(replicaFilePath)){
                filesystem::copy_file(pathIteration.path(),
               (filesystem::path)replicaFilePath,filesystem::copy_options::none);
               filesystem::last_write_time(replicaFilePath,filesystem::last_write_time(pathIteration.path()));
               logToFileAndConsole("File "+fileName+" Copied",logFilePath);
               }

        //If the current path leads to a file and it exists it checks the modified time to see if there were any changes made to it
        else{
            auto sourceTime = filesystem::last_write_time(pathIteration.path());
            if(filesystem::last_write_time(replicaFilePath) != sourceTime){
                filesystem::remove(replicaFilePath);
                filesystem::copy_file(pathIteration.path(),
               (filesystem::path)replicaFilePath);
               filesystem::last_write_time(replicaFilePath,filesystem::last_write_time(pathIteration.path()));
               logToFileAndConsole("File "+fileName+" Copied",logFilePath);
               }
        }
     
        }
    }
    


//Iterates through Replica to check if there's any files there that don't exist in Source and procedes to delete them
void synchronizeFoldersDelete(const string& replicaDir, const string& sourceDir,const string& logFilePath){
    
    string fileName;
    string filePath;
    string replicaFilePath;
    string sourceFilePath;
    
 
    for (const auto& pathIteration : filesystem::directory_iterator(replicaDir)) {
        replicaFilePath=pathIteration.path().u8string();
        filePath = pathIteration.path().u8string();
        fileName = filePath.replace(0, replicaDir.length(), "");
        sourceFilePath = sourceDir + fileName;
        
        //Same logic applies as before: If it's a directory and it doesn't exist in Source it's deleted along all the files
        //If not it moves inside that directory and iterates it
        if (filesystem::is_directory(pathIteration.path())) {
            if(!filesystem::exists(sourceFilePath)){
                filesystem::remove_all(replicaFilePath);
                logToFileAndConsole("Folder "+fileName+" Deleted",logFilePath);
                continue;
            }

            synchronizeFoldersDelete(pathIteration.path().u8string(),sourceFilePath,logFilePath);
        }

        // Delete file from replica if it doesn't exist in Source
        else if (!filesystem::exists(sourceFilePath)) {
                filesystem::remove(replicaFilePath);
                logToFileAndConsole("File "+fileName+" Deleted",logFilePath);
        }
        //Since there is a possibility of someone modifying manually the files in Replica, this else statement-
        //-copies the file from Source to make sure they are the same files
        else{
            auto replicaTime = filesystem::last_write_time(pathIteration.path());  
            //Since copy_file essentially creates a new file with a new modified time and then copies the data inside it,-
            //-it is necessary to make it equal to the Source's last write time so it isn't confused by an actual modification

            //Note: There's flags to make copy_file overwrite the file and make it so it keeps the Source last write time -
            //-but currently it doesn't work well in g++
            //Reference - https://github.com/msys2/MSYS2-packages/issues/1937

            if(filesystem::last_write_time(sourceFilePath) != replicaTime){
                filesystem::remove(replicaFilePath);
                filesystem::copy_file(sourceFilePath,
               (filesystem::path)pathIteration.path());
               filesystem::last_write_time(replicaFilePath,filesystem::last_write_time(sourceFilePath));
               }
        }
        
    }

}




int main(int argc, char* argv[]) {
    string sourceDir="";
    string replicaDir="";    
    string logFilePath="";
    int syncInterval;

    //Gets Source folder, Replica folder, synchronization interval, and log file path from the terminal by user input
    cout<< "Write the path for the source folder"<<endl;
    cin >> sourceDir;
    cout<< "Write the path for the replica folder"<<endl;
    cin >> replicaDir;
    cout<< "Write the path for the logfile folder"<<endl;
    cin >> logFilePath;
    cout<< "Write which sync interval the program should have (in seconds)"<<endl;
    cin >> syncInterval;
    createFolders(sourceDir,replicaDir,logFilePath);
    // Main loop for periodic synchronization
    while (true) {
        synchronizeFolders(sourceDir, replicaDir,logFilePath);
        synchronizeFoldersDelete(replicaDir,sourceDir,logFilePath);
        //logToFileAndConsole("Synchronization completed", logFilePath);
        
        // Sleep for synchronization interval
        this_thread::sleep_for(chrono::seconds(syncInterval));  
    }
    
    return 0;
}
