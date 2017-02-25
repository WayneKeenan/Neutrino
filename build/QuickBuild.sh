#/bin/bash
rm -rf ./bin/Neutrino
make 
make install
rm -rf ./Neutrino
cd ./bin
echo "Executing game..."
echo ''
./Neutrino
echo 'Game terminated...'
echo "Log file output: "
echo ''
cat ./Neutrino.log

