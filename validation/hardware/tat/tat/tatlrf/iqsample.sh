#! /bin/sh

if [ $# -ne 2 ]         
then                    
	exit 1
fi

# escape_sed()
# read a stream from stdin and escape characters in text that could be interpreted as
# special characters by sed
escape_sed() {
 sed \
  -e 's/\//\\\//g' \
  -e 's/\&/\\\&/g'
}

file=$1
increment=$2
tmp_file=/usr/local/lib/tat/tatlrf/iqsample.tmp

tail -n 1 $file > $tmp_file
record=`cat $tmp_file`

if [ "$record" != "" ]
then
    # 
    old_nb=`cut -d , -f 9 $tmp_file`
    new_nb=`expr $old_nb + $increment`

    old_pat=`cut -d , -f 1-9 $tmp_file|escape_sed`
    new_pat=`cut -d , -f 1-8 $tmp_file|escape_sed`,$new_nb
    ending=`cut -d , -f 10- $tmp_file`

    sed "s/$old_pat/$new_pat/" $file > $tmp_file
    
    mv $tmp_file $file
else
    echo "inconsistent file: $file"
fi

