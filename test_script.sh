#!/bin/bash
./my_shell <<EOF
ls
pwd
echo "Shell test" > test.txt
cat < test.txt
exit
EOF
