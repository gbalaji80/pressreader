#!/bin/bash
infName="9gva2026030900000000001001"
outfName='euro-20260309.pdf'
bearerToken=`cat btoken.txt`
#gcc dload.c -I/usr/include/ImageMagick-6 _I/usr/include/x86_64-linux-gnu/ImageMagick-6 -lcurl -ljson-c -lMagickWand -o myLoft
#gcc dload.c -lcurl -ljson-c  -lbuildurl -o myLoft
gcc -c buildurl.c -fPIC -o buildurl.o -lcurl -ljson-c 
gcc -shared -o libbuildurl.so buildurl.o -lcurl -ljson-c 
gcc dload.c -L. -o myLoft -lcurl -ljson-c -lbuildurl
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:myLoft
./myLoft ${infName} ${bearerToken}
#sh getKeys.sh ${infName}
#curl -s -o input.json --oauth2-bearer ${bearerToken} -A "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/145.0.0.0 Mobile Safari/537.36" https://ingress.pressreader.com/services/IssueInfo/GetPageKeys?issue=$1&pageNumber=1&preview=true 
#cat input.json | jq -r '.PageKeys[] | {Key}' > output.json
#jq -r ".Key" output.json > out.txt
#rm -f output.json
#./a.out ${infName} ${bearerToken}
ls -1| grep "wsj_" > playlist.txt
convert -verbose @playlist.txt ${outfName}
rm -f wsj_*
rm -f playlist.txt
rm -f out.txt
