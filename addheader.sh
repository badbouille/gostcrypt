#!/bin/bash

FILES=$(find . -name *.c -o -name *.cpp -o -name *.h)

for file in $FILES; do
	exec 5< $file
	read l1 <&5
	read l2 <&5
	read l3 <&5
	c1=$(echo $l1 | egrep "^\/\/$")
	c3=$(echo $l3 | egrep "^\/\/$")
	[ ! "$c1" ]  && continue
	[ ! "$c3" ]  && continue
	author=$(echo "$l2" | sed 's|// Created by \(.\+\) on \(.\+\).|\1|g')
	date=$(echo "$l2" | sed 's|// Created by \(.\+\) on \(.\+\).|\2|g')
	echo "$file: $author / $date"
	tail -n +4 $file > tmp1.txt || continue
	cat header.txt \
		| sed "s|\[file\]|$(basename $file)|g" \
		| sed "s|\[author\]|$author|g" \
		| sed "s|\[date\]|$date|g" \
		| cat - tmp1.txt > tmp2.txt || continue
	mv tmp2.txt $file
	rm tmp1.txt
	echo "+success"
done

echo "All done"

