#!/bin/bash
set -x;

chmod -R a+rwx .

truncate -s0 grade.log;
truncate -s0 details.log;
truncate -s0 temp.txt;

g++ -std=c++11 main*.cpp -o as2 -pthread > details.log 2>&1
gcc -std=c11 main*.c -o as2 -lpthread >> details.log 2>&1
echo `ls main*`

name=$(grep -E '\#define\s+STUDENT_NAME\s+' main*.c* | grep -v "George P. Burdell" | cut -d'"' -f2)
cwid=$(grep -E '\#define\s+STUDENT_CWID\s+' main*.c* | grep -v "123456789" | cut -d'"' -f2)
echo $name >> grade.log
echo $cwid >> grade.log

if [ -f as2 ];
then
    echo "40 compiled" >> grade.log 2>&1
else
    echo "0 cannot compile" >> grade.log
    echo "0 cannot compile" >> grade.log
    echo "0 cannot compile" >> grade.log
    echo "0 cannot compile" >> grade.log
    echo "0 cannot compile" >> grade.log
    echo "0 cannot compile" >> grade.log
    echo "0 cannot compile" >> grade.log
    echo "0 cannot compile" >> grade.log
    echo "0 cannot compile" >> grade.log
    exit;
fi

chmod -R a+rwx .

nproc > temp.txt

function run() {
    expected_code=0
    code=0
    single_core_time=0
    time_limit=0

    /usr/bin/time -q -f'%e' ./single_core.out < $1 2>&1 2>> temp.txt
    expected_code=$?
    single_core_time=$(cat temp.txt | tail -n1);

    time_limit=$(perl -E "say ${single_core_time} * 3");

    timeout --signal=SIGKILL "${time_limit}s" /usr/bin/time -q -f'%e' ./as2 < $1 2>&1 2>> temp.txt 1>> details.log &
    wait $!
    code=$?
    multi_core_time=$(cat temp.txt | tail -n1);

    echo $1, expected_code, single_core_time, code, multi_core_time >> details.log;
    echo $1, ${expected_code}, ${single_core_time}, ${code}, ${multi_core_time} >> details.log;

    if [[ ${code} == ${expected_code} ]];
    then
        if [[ $(echo "(${single_core_time}) >= ${multi_core_time} * 1.2" | bc -l) -eq 1 ]];
        then
            echo "10 $1" >> grade.log
        else
            echo "0 $1 you took too long. Check details.log for more info." >> grade.log
        fi
    else
            echo "0 $1 your return code is incorrect. Check details.log for more info." >> grade.log
    fi
}

run tiny2.txt

tiny2_grade=$(cat grade.log | tail -n1 | cut -f1 -d" ")

run tiny4.txt
tiny4_grade=$(cat grade.log | tail -n1 | cut -f1 -d" ")

if [[ ${tiny2_grade} == "0" ]] || [[ ${tiny4_grade} == "0" ]];
then
    echo "0 program does not always return correct code" >> grade.log
    echo "0 program does not always return correct code" >> grade.log
    echo "0 program does not always return correct code" >> grade.log
    echo "0 program does not always return correct code" >> grade.log
    echo "0 program does not always return correct code" >> grade.log
    echo "0 program does not always return correct code" >> grade.log
    echo "0 program does not always return correct code" >> grade.log
    exit;
fi

run small2.txt

run small4.txt

run medium2.txt

run medium4.txt

run big2.txt

run big4.txt


chmod -R a+rwx .

exit 0