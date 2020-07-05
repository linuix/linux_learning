
if [ -n $1 ]
then
echo param is not null
fi

if test -f $1 
then 
    echo $1 is exsist
else
    echo  $1 not exsist
fi
exit 0