#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_trap.h>

static int sbi_ecall_random_handler(unsigned long extid, unsigned long funcid,
				    struct sbi_trap_regs *regs,
				    struct sbi_ecall_return *out)
{
	unsigned long value;
	switch (funcid) {
	case SBI_EXT_RANDOM_GET:
		asm volatile("csrr %0, cycle" : "=r"(value));
		value ^= value >> 16;
		value *= 0x9e3779b97f4a7c15UL;
		value ^= value >> 32;
		out->value = value;
		return 0;
	case SBI_EXT_RANDOM_GET_CSR:
		switch (regs->a0) {
		case 0xf11:
			asm volatile("csrr %0, mvendorid" : "=r"(value));
			break;
		case 0xf12:
			asm volatile("csrr %0, marchid" : "=r"(value));
			break;
		case 0xf13:
			asm volatile("csrr %0, mimpid" : "=r"(value));
			break;
		case 0x301:
			asm volatile("csrr %0, misa" : "=r"(value));
			break;
		default:
			return SBI_ERR_INVALID_PARAM;
		}
		out->value = value;
		return 0;
	default:
		return SBI_ENOTSUPP;
	}
}

struct sbi_ecall_extension ecall_random = {
	.name	     = "random",
	.extid_start = SBI_EXT_RANDOM,
	.extid_end   = SBI_EXT_RANDOM,
	.handle	     = sbi_ecall_random_handler,
};
