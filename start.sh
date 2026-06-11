#!/bin/bash
issueDate="20260418"
magCode="9y03"
prefixDir=""
infName="${magCode}${issueDate}00000000001001"
if [ "$magCode" == "9y03" ]; then
    prefixDir="eco"
elif [ "$magCode" == "9bct" ]; then
    prefixDir="mw"
elif [ "$magCode" == "9gva" ]; then
    prefixDir="wsj"
else
    echo "Invalid magCode"
fi

outDir="/home/balaji/Documents/newsMagazines/${prefixDir}"
echo "gbtest $outDir"

if [ ! -d "$outDir" ]; then
  mkdir "$outDir"
  echo "Directory created: $outDir"
else
  echo "Directory already exists: $outDir"
fi

outfName="${outDir}/${prefixDir}-${issueDate}.pdf"
bearerToken=`cat btoken.txt`
rm -f buildurl.o
rm -f libbuildurl.a
rm -f myLoft
gcc -c buildurl.c -o buildurl.o -lcurl -ljson-c
ar rcs libbuildurl.a buildurl.o
gcc dload.c -L. -o myLoft -lbuildurl -lcurl -ljson-c
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:myLoft
./myLoft ${infName} ${bearerToken}
ls -1| grep "wsj_" > playlist.txt
convert -verbose @playlist.txt ${outfName}
rm -f wsj_*
rm -f playlist.txt
rm -f out.txt
