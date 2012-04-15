#!/bin/sh

cd Templates
rm -f *.zip
rm -rf CVS

for i in *; do
	if [ -d "$i" ] ; then
		echo Building $i.zip
		cd "$i"
		zip "../$i.zip" *
		addattrib "../$i.zip" "CheckSum::MD5" `md5sum "../$i.zip" | cut -c1-32`
		cd ..
	fi
done
