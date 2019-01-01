#mpirun -np 32 -machinefile machinefile -N 4 ./advmates-calc -d ../examples/Tokyo/ -t 7200000 -q
mpirun -np 16 -machinefile machinefile -N 2 ./advmates-calc -d ../examples/Tokyo/ -t 7200000 -q
mpirun -np 8 -machinefile machinefile -N 1 ./advmates-calc -d ../examples/Tokyo/ -t 7200000 -q
mpirun -np 4 -machinefile machinefile -N 1 ./advmates-calc -d ../examples/Tokyo/ -t 7200000 -q
