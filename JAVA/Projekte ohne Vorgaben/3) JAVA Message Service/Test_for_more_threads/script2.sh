#!/bin/bash
( { echo "aa" ; sleep 3 ; echo "watch LIDL" ; sleep 3 ; echo "buy ALDI 10" ; sleep 3 ;  echo "quit" ; sleep 3 ;} | java -jar client.jar) &
( { echo "bb" ; sleep 3 ; echo "watch ALDI" ; sleep 3 ; echo "buy LIDL 10" ; sleep 3 ;  echo "quit" ; sleep 3 ;} | java -jar client.jar)