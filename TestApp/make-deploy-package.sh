#!/bin/bash

if [ ! -f TestApp.pro ] ; then
  echo Wrong directory. TestApp.pro must reside in CWD.
  exit 1
fi

which cqtdeployer
if [ 0 != $? ] ; then
  echo "[ FAILURE ] Console Qt Deployer(cqtdeployer) is not installed."
  exit 1
else
  echo "[ OK ] Console Qt Deployer(cqtdeployer) installed"
fi

which 7z
if [ 0 != $? ] ; then
  echo "[ FAILURE ] 7z is not installed."
  exit 1
else
  echo "[ OK ] 7z is installed."  
fi

which make
if [ 0 != $? ] ; then
  echo "[ FAILURE ] make is not installed."
  exit 1
else
  echo "[ OK ] make is installed."  
fi

if [ -d build.dir ] ; then
  rm -rf build.dir/*
else
  mkdir build.dir
fi

cd build.dir

qmake -config release ../TestApp.pro
if [ 0 != $? ] ; then
  echo "[ FAILURE ] Failed to configure qmake build."
  exit 1
fi

make
if [ 0 != $? ] ; then
  echo "[ FAILURE ] Failed to build application."
  exit 1
fi

make clean
if [ 0 != $? ] ; then
  echo "[ FAILURE ] Failed to clean build folder."
  exit 1
fi

cd ..

if [ -d deploy.dir ] ; then
  rm -rf deploy.dir/*
else
  mkdir deploy.dir
fi

cd deploy.dir

cqtdeployer -bin ../build.dir/TestApp -qmake $(which qmake) -qmlDir /usr/lib/x86_64-linux-gnu/qt5/qml -targetDir TestApp
if [ 0 != $? ] ; then
  echo "[ FAILURE ] Failed to create a deployment directory."
  exit 1
fi

7z a TestApp
if [ 0 != $? ] ; then
  echo "[ FAILURE ] Failed compress deploy dir."
  exit 1
fi

rm -rf TestApp

cd ..

echo "[ SUCCESS ] Done!"
