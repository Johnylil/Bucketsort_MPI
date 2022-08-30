len=$((2 ** 23)) #17..20
echo "Array Length is $len"
echo "------------------------------"
echo "------------------------------"
cmd_time=($(./bucket_serial $len))
first_time=${cmd_time[5]}
echo "For Serial Code: "
echo "Total time for execution is $first_time sec"
for j in 1 2 4 8 16 24 32
do
    echo "--------------------------"
    echo "--------------------------"
    cmd_new_time=($(mpirun -machinefile m3 -np $j bucket_parallel_mpi $len)) #2>&1
    second_time=${cmd_new_time[38]} #14
    echo "For $j Buckets the times are: "
    echo "Total time for Scatterv is ${cmd_new_time[5]} sec"
    echo "Total time for Gatherv is ${cmd_new_time[12]} sec"
    echo "Total time for Receive is ${cmd_new_time[19]} sec"
    echo "Total time for Send is ${cmd_new_time[26]} sec"
    echo "Total Communication cost: ${cmd_new_time[31]} sec"
    echo "Total time for execution is $second_time sec"
    echo "Speedup between $j nodes and serial code is"
    echo "scale=5 ; $first_time / $second_time" | bc
    echo "Efficiency for $j nodes is"
    echo "scale=5 ; (($first_time / $second_time) / $j)" | bc
done
echo "------------------------------"
echo "------------------------------"
