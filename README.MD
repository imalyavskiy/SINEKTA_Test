This is a test task to SINEKTA Tomsk
====================================
Repository structure
--------------------
./TestApp 
  - test application sources

./docs
  - the task, some sample data files(*.s1p), and memo article with LERP formulas

./*
  - qt instal/remove scripts, this README.MD

How to use
--------------------
This application based on Qt 5.15.3. 

In order to build the app you need to install Qt first - just run supplied 
script 'install.qt_5.15.3.sh'. It will install all the required packages 
along with QTCreator, docs and examples. Then use QtCreator to open 
the ./TestApp/TestApp.pro project file.

In order to create deployment package first you need to ensure that 'cqtdeployer'
(Console QT Deployer) tool is installed. The cqtdeployer can be obtaibed here - 
https://github.com/QuasarApp/CQtDeployer/releases. Then you need to run the 
'./TestApp/make-deploy-package.sh' script. It will build the TestApp, gather all 
the required files in a single folder and then compress it. The resulting archieve will 
be located at ./TestApp/deploy.dir named as TestApp.7z.

To make sure that all the data are packed with the application in a single place
either copy the archieve to a clean machine or remove qt packages from the system
by means of 'remove.qt_5.15.3.sh' script.

Then extract the archive and run TestApp.sh from the root of the extracted folder.

Cheers! Have fun!

