#include <types.h>

static const char *exceptions[] = {
	"Divide error",
	"Debug exceptions",
	"Non-maskable interrupt (NMI)",
	"Breakpoint",
	"Overflow",
	"Bounds check",
	"Invalid opcode",
	"Coprocessor not available",
	"Double fault",
	"Coprocessor segment overrun",
	"Invalid TSS",
	"Segment not present",
	"Stack exception",
	"General protection exception",
	"Page fault",
	"Intel reserved",
	"Coprocessor error",
};

void info(uint32_t ret_ip, uint32_t ss, uint32_t gs, uint32_t fs,
		uint32_t es, uint32_t ds, uint32_t edi, uint32_t esi,
		uint32_t ebp, uint32_t tmp, uint32_t ebx, uint32_t edx,
		uint32_t ecx, uint32_t eax, uint32_t isr_nr, uint32_t errno,
		uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t old_esp,
		uint32_t old_ss)
{
	clean_screen();

	printf("Exception %d: %s\n\n", isr_nr, exceptions[isr_nr]);
	printf("old_ss:\t%x\told_esp:\t%x\n", old_ss, old_esp);
	printf("eflags:\t%x\n", eflags);
	printf("cs:\t%x\teip:\t%x\n", cs, eip);
	printf("errno:\t%x\tisr_nr:\t%x\n", errno, isr_nr);
	printf("eax:\t%x\tecx:\t%x\n", eax, ecx);
	printf("edx:\t%x\tebx:\t%x\n", edx, ebx);
	printf("esp:\t%x\tebp:\t%x\n", tmp, ebp);
	printf("esi:\t%x\tedi:\t%x\n", esi, edi);
	printf("ds:\t%x\tes:\t%x\n", ds, es);
	printf("fs:\t%x\tgs:\t%x\n", fs, gs);
	printf("ss:\t%x\tret_ip:\t%x\n", ss, ret_ip);
}
