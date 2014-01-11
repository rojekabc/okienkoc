#!/bin/sh
if test ! -d en; then
	mkdir en;
fi
for i in `ls *.[ch]`; do
	./rchange wordstoen $i > en/$i;
done
