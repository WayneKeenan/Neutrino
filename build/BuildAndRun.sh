#/bin/bash
mkdir ./bin
make clean
make 
make install
rm -rf ./Neutrino
./MakePackfile.sh
cd ./bin
echo ''
echo "Executing game..."
echo ''
./Neutrino
echo 'Game terminated...'
echo "Log file output: "
echo ''
cat ./Neutrino.log

