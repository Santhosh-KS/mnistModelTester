# mnistModelTester

# Software Installation Guide

## Install dependencies

### Install Wt

- Download the tar file from the [Wt website](https://www.webtoolkit.eu/wt/download)
- Extract the file in /opt/ directory 
- install by following the INSTALL document in the extracted folder.

### Install RapidJson

- `git clone https://github.com/Tencent/rapidjson.git /opt/rapidjson`

### Install gtest (For C++ unit tests to compile) OPTIONAL

Follow the instructions provide [here](https://github.com/iat-cener/tonatiuh/wiki/Installing-Google-Test-For-Linux)

## Build

- `git clone https://github.com/Santhosh-KS/mnistModelTester.git`
- `cd mnistModelTester`
- `mkdir build`
- `./ClientRun.sh`

By running ./ClientRun.sh it builds all the components including the unittests and server and client code all at once.

you can find the required binaries in **build** directory.

you need to create a softlink under **mnistModelTester/client/ui/** a link to Wt's resource directory present in **/opt/Wt4.0.4/resources**

- `cd mnistModelTester/client/ui/`
- `ln -s /opt/Wt4.0.4/resources resources`


If you want to change the IP or port number. Feel free to have a look at the **ClientRun.sh** file and modify as required.
