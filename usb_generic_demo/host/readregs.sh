cat ../../regparse/registers-mx220.txt | while read reg addr; do echo $reg $addr $(./usbtool getreg $addr 2>&1 | awk '/^Reg/ { print $7 }'); done
