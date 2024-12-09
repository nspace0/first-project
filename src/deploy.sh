#!/bin/bash


ssh-keyscan -H 10.10.0.2 >> ~/.ssh/known_hosts
scp src/cat/s21_cat src/grep/s21_grep admin1@10.10.0.2:/usr/local/bin
ssh admin1@10.10.0.2 "chmod +x /usr/local/bin/s21_cat /usr/local/bin/s21_grep"
