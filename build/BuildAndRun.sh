#/bin/bash
mkdir ./bin
make clean
make 
make install
rm -rf ./Neutrino
./MakePackfile.sh
cd ./bin
echo "Executing game..."
./Neutrino
echo "Log file output: "
cat ./Neutrino.log

