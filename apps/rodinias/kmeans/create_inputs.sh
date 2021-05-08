#! /bin/bash

echo -e "Creating input file."
cd inputGen 
make
./datagen 1000
mv 1000_34.txt ../
