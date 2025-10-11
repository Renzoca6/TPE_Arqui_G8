include Makefile.inc

SAMPLE_DATA=0001-sampleDataModule.bin

all: sampleCodeModule sampleDataModule shell

sampleCodeModule:
	cd SampleCodeModule; make

sampleDataModule:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

shell: 
	cd shell; make

clean:
	cd SampleCodeModule; make clean
	cd shell; make clean
	rm -rf *.bin


.PHONY: sampleCodeModule shell all clean