# Folder Synchronization Tool
This C++ program enables the synchronization of folders between a source directory and a replica directory, ensuring that both directories contain the same files and directories.

## Features
### Folder Creation:
* Automatically generates source and replica folders if they do not already exist.

### Periodic Synchronization:
* Periodically monitors the source directory for new files and copies them to the replica directory.

* Checks for files in the replica directory that don't exist in the source directory and deletes them.

* Detects file modifications by comparing modification times provided by the OS:
  * If a modification occurs in the source, it overwrites the corresponding file in the replica.
  * If a modification occurs in the replica, it reverts to the older version from the source.
### Logging:
* Logs every file/folder copy or deletion to a specified log file for future reference
## Requirements
* C++ compiler with support for C++17.

  

### Standard C++ libraries: 
* iostream, fstream, chrono, thread, filesystem, string.
## Build and Execution
### For Windows
* Download and run main.exe provided that the requirements are met.
### Compile the Program:
If you have the source code then:

```
g++ main.cpp -o folder_sync_tool -std=c++17 -lstdc++fs
```

Then run:

```
./folder_sync_tool
```

## Using Task - Syncrhonized Folders
### Inputs:
For the inputs you will be asked to input the path for the source folder, the replica folder, the logfile and finally the synchronization interval where the program checks the folders for changes.

* If you insert the path for a folder that already exists for source or replica inputsit will use it.
* If it doesn't it exist it will create a directory in that path.
* For logfile path input if it doesn't exist it will create the file.
* If it does exist it will use the old file and keep writing on it without deleting the previous content.
* **Note:** The path must not contain non-ASCII characters. 
* For the synchronization input you just have to give a number which will be the seconds the program will wait until it does another check of the folders to see the changes.

![imagem](https://github.com/programff/Task---Synchronized-Folders/assets/166831895/3f59a225-1727-4e4d-904f-9ae5866dd05d)

<sup> **Example 1** - Program execution without an existing source and replica folders<sub>
\
\
\
\
![imagem](https://github.com/programff/Task---Synchronized-Folders/assets/166831895/6f6fe1ea-5e73-43f2-9acb-0c4a798f2c0b)

<sup>**Example 2** - of the program executing with an existing source and replica folder<sub>






