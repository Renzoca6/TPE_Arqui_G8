#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <video.h>
#include <realTimeClock.h>
#include "interrups_dispatcher.h"
#include "keyboard_handler.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void * const shellAddress = (void*)0x600000;


typedef int (*EntryPoint)();

void clearBSS(void * bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary() {
	char buffer[10];
    clearBSS(&bss, &endOfKernel - &bss);

	vdPrint("[x64BareBones]"); //POR EJEMPLO ACA 
	vdNewline();
	vdPrint("CPU Vendor:");
	vdPrint(cpuVendor(buffer));
	vdNewline();

	vdPrint("[Loading modules]");
	vdNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	vdPrint("[Done]");
	vdNewline();
	vdNewline();

	vdPrint("[Initializing kernel's binary]");
	vdNewline();


	vdPrint("  text: 0x");
	vdPrintHex((uint64_t)&text);
	vdNewline();
	vdPrint("  rodata: 0x");
	vdPrintHex((uint64_t)&rodata);
	vdNewline();
	vdPrint("  data: 0x");
	vdPrintHex((uint64_t)&data);
	vdNewline();
	vdPrint("  bss: 0x");
	vdPrintHex((uint64_t)&bss);
	vdNewline();

	vdPrint("[Done]");
	vdNewline();
	vdNewline();
	return getStackBase();

}

int main() {
	init_interrupts();
	vdPrint("[Kernel Main]");
	vdNewline();
	vdPrint("  Sample code module at 0x");
	vdPrintHex((uint64_t)sampleCodeModuleAddress);
	vdNewline();
	vdPrint("  Calling the sample code module returned: ");
	vdPrintHex(((EntryPoint)sampleCodeModuleAddress)());
	vdNewline();
	vdNewline();

	vdPrint("  Sample data module at 0x");
	vdPrintHex((uint64_t)sampleDataModuleAddress);
	vdNewline();
	vdPrint("  Sample data module contents: ");
	vdPrint((char*)sampleDataModuleAddress);
	vdNewline();

	char buf1[16];
	getTimeString(buf1);
	vdPrint(buf1);

	while (1){
		if (hasNext()){
		vdPrintChar(getNext().key);
		}
	}

	return 0;
}
