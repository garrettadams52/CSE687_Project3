# CSE687_Project3

# Project Setup and Build Instructions

This project consists of three components: `MapLibrary`, `ReduceLibrary`, and `MapReduce`. Follow the steps below to configure and build the project correctly.

## Build Order

Ensure that the projects are built in the following order:
1. `MapLibrary`
2. `ReduceLibrary`
3. `MapReduce`

## Preprocessor Definitions

Make sure to set the following preprocessor definitions for each project:

- `MapLibrary`:
  ```plaintext
  MAPLIBRARY_EXPORTS
- `ReduceLibrary`:
  ```plaintext
  REDUCELIBRARY_EXPORTS
- ## Additional Include Directories

All three projects (`MapLibrary`, `ReduceLibrary`, and `MapReduce`) must include the shared directory in their "Additional Include Directories" settings. Add the following path to each project's configuration:

1. **Open Project Properties**:
   - Right-click on the project in Solution Explorer.
   - Select "Properties".

2. **Navigate to Additional Include Directories**:
   - Go to `Configuration Properties` > `C/C++` > `General`.

3. **Add Include Path**:
   - In the "Additional Include Directories" field, add the following path:
     ```plaintext
     .../shared/include
     ```

Make sure to repeat these steps for each project (`MapLibrary`, `ReduceLibrary`, and `MapReduce`) to ensure that they all have the shared include directory configured correctly.

## Runtime Library

Ensure that all three projects (`MapLibrary`, `ReduceLibrary`, and `MapReduce`) use the same runtime library. The runtime library settings must be consistent across all projects.

### Steps to Configure

1. **Open Project Properties**:
   - Right-click on the project in Solution Explorer.
   - Select "Properties".

2. **Navigate to Runtime Library Settings**:
   - Go to `Configuration Properties` > `C/C++` > `Code Generation`.

3. **Set the Runtime Library**:
   - In the "Runtime Library" field, select the same option for all three projects. Common options include:
     - `Multi-threaded DLL (/MD)`
     - `Multi-threaded Debug DLL (/MDd)`

Make sure to repeat these steps for each project (`MapLibrary`, `ReduceLibrary`, and `MapReduce`) to ensure that they all use the same runtime library settings.

# CSE687_Project 4

1. Repeat all previous steps for Stub and Controller Projects.
2. Ensure Project is located in C:\Repo\


## Build Order

Ensure that the projects are built in the following order:
1. 'Stub'
2. `MapLibrary`
3. `ReduceLibrary`
4. `MapReduce`
5. 'Controller'

## Steps to Configure
1. Right click on References under Stub project and add reference to MapReduce Project.
2. Unput Configuration Properties -> Linker -> Input -> Addition Dependencies
   a. MapLibrary.lib
   b. ReduceLibrary.lib
3. Right click on any project -> Build Dependencies -> Project Dependencies
    a. Controller No Dependencies
    b. MapLibrary None
    C. MapReduce depends on MapLibrary and ReduceLibrary
    d. ReduceLibrary None
    e. Stub depends on MapReduce
4. Right click on any project -> Configure Startup Projects
    a. Select Multiple Startup Projects
    b. Select Start for Stub and Controller
    c. Ensure Order is:
        1. Stub
        2. MapLibrary
        3. MapReduce
        4. ReduceLibrary
        5. Controller
5. Clean Solution
6. Build Solution

## How to run in command line
### MAP [filepath]
Map C:\MapReduceProj\Shakespear C:\MapReduceProj\ C:\MapReduceProj\ C:\Repo\CSE687_Project3\MapReduce\x64\Debug\MapLibrary.dll C:\Repo\CSE687_Project3\MapReduce\x64\Debug\ReduceLibrary.dll

### Reduce [filepath]
Reduce C:\MapReduceProj\Shakespear C:\MapReduceProj\ C:\MapReduceProj\ C:\Repo\CSE687_Project3\MapReduce\x64\Debug\MapLibrary.dll C:\Repo\CSE687_Project3\MapReduce\x64\Debug\ReduceLibrary.dll

