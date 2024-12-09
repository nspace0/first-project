#!/bin/bash

COUNTER_SUCCESS=0
COUNTER_FAIL=0
DIFF_RES=""
PATTERN="School21"
TEST_FILE="./test_file.txt"
echo "" > log.txt



for var in -i -e -v -c -l -n -s -o -h -f
do
    if [ $var == -f ] 
    then
    TEST1="$var pattern.txt $TEST_FILE"
    else
          TEST1="$var $PATTERN $TEST_FILE"
    fi
          echo "$TEST1"
          ./s21_grep $TEST1 > s21_grep.txt
          grep $TEST1 > grep.txt
          DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
          if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
            then
              (( COUNTER_SUCCESS++ ))
            else
              (( COUNTER_FAIL++ ))
          fi
          rm s21_grep.txt grep.txt
done

for var in -i -v -c -l -n -s -o -h
do
  for var2 in i e v c l n s h f 
  do
    if [ $var == -f ] || [ $var2 == f ] 
    then
    TEST1="$var$var2 pattern.txt $TEST_FILE"
    else
          TEST1="$var$var2 $PATTERN $TEST_FILE"
    fi
      echo "$TEST1"
      ./s21_grep $TEST1 > s21_grep.txt
      grep $TEST1 > grep.txt
      DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
      if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
        then
          (( COUNTER_SUCCESS++ ))
        else
          (( COUNTER_FAIL++ ))
      fi
      rm s21_grep.txt grep.txt

  done
done


echo "SUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"
