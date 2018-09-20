#!/bin/bash

alias vl="valgrind --leak-check=full"
make clean
make
vl ./env
echo finished test1
sleep 2

vl ./env -n 3
echo finished test2
sleep 2

vl ./env -n hello
echo finished test3

vl ./env -t 3
echo finished test4

vl ./env -n 4 TZ=EST5EDT,CST6CDT,MST7MDT,PST8PDT -- date
echo finished test5

vl ./env -n 4 TEMP=EST5EDT,CST6CDT,MST7MDT,PST8PDT TZ=%TEMP -- date
echo finished test6

vl ./env -n 1 TZ=EST5EDT,CST6CDT,MST7MDT,PST8PDT -- date
echo finished test7

vl ./env -n 4 TZPPPPPP=EST5EDT,CST6CDT,MST7MDT,PST8PDT -- date
echo finished test8

vl ./env -n 4 TZ=EST5EDT,CST6CDT,MST7MDT,PST8PDT -- dateIsNotVaild
echo finished test9

vl ./env -n TEMP=EST5EDT,CST6CDT,MST7MDT,PST8PDT TZ=%TEMP -- date
echo finished test10

vl ./env -n 4 TZ=%TEMP TEMP=EST5EDT,CST6CDT,MST7MDT,PST8PDT -- date
echo finished test11

vl ./env -n 4 TEMP=EST5EDT,CST6CDT,MST7MDT,PST8PDT TZ=%TEMP,MST7MDT,PST8PDT,CST6CDT -- date
echo finished test12

vl ./env -n 4 TZ=EST5EDT,CST6CDT, MST7MDT,PST8PDT -- date
echo finished test13
