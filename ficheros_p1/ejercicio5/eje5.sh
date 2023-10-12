#!/bin/bash

i=0

cat "/etc/passwd" | while IFS=':' read -r var1 var2 var3 var4 var5 var6 var7
do
    echo "[Entry #"$i"]"
    echo "	login= " $var1 
    echo "	enc_pass=  " $var2 
    echo "	uid = " $var3 
    echo "	tgid= " $var4 
    echo "	tuser_name= " $var5 
    echo "	home= " $var6 
    echo "	tshell= " $var7 
    i=$(($i+1))
    
done
