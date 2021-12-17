CPMFORMAT=avrcpm
ESPFORMAT=espcpm

diskimage: cpm.bin cpmdsk0/*
	mkfs.cpm -f ${CPMFORMAT} -b cpm.bin disk0
	cd cpmdsk0; for x in *; do cpmcp -f ${CPMFORMAT} ../disk0 $$x 0:$$x; done; cd ..
	../bin/format disk0
	mkfs.cpm -f ${ESPFORMAT} disk1
	../bin/format disk1
	mkfs.cpm -f ${ESPFORMAT} disk2
	../bin/format disk2
	mkfs.cpm -f ${ESPFORMAT} disk3
	../bin/format disk3
	mkfs.cpm -f ${ESPFORMAT} disk4
	../bin/format disk4
	mkfs.cpm -f ${ESPFORMAT} disk5
	../bin/format disk5

cpm.bin: boot.bin cpm22.bin 
	dd conv=sync bs=128 count=1  if=boot.bin   > cpm.bin
	dd conv=sync bs=128 count=50 if=cpm22.bin >> cpm.bin

boot.bin: boot.p
	p2bin -r \$$2000-\$$2046 boot.p boot.bin

boot.p: boot.asm
	asl -L -cpu 8080 boot.asm

cpm22.bin: cpm22.p
	p2bin -r \$$DC00-\$$F7BC cpm22.p cpm22.bin

cpm22.p: cpm22.asm
	asl -L -cpu 8080 cpm22.asm

clean:
	rm -f *.bin *.p
	rm -f cpm.bin disk0 disk1 disk2 disk3 disk4 disk5

install: disk0 dummy
	rm -f ../../data/*
	cp disk0 ../../data/disk0
	cp disk1 ../../data/disk1
	cp disk2 ../../data/disk2
	cp disk3 ../../data/disk3
	cp disk4 ../../data/disk4
	cp disk5 ../../data/disk5

dummy:
