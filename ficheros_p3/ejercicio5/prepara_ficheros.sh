#!/bin/bash

function usage {
	echo Parameter incorrect
}

if [ $# != 1 ]; then
	usage && exit -1
fi

if [ -d $1 ]; then
	rm -r $1/
fi

mkdir $1
mkdir $1/subdir
touch $1/fichero1
echo '0123456789' > $1/fichero2
ln -s $1/fichero2 $1/enlaceS
ln $1/fichero2 $1/enlaceH

for f in subdir fichero1 fichero2 enlaceS enlaceH;
do
	stat $1/$f
    echo ''
done

exit 0
