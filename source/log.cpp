#include "log.hpp"


#define VOLADDR(addr, type) (*(type volatile *)(addr))
#define REG_NOCASH_LOG      VOLADDR(0x04FFFA1C, unsigned char)

void no$log(const char* str)
{
	while(*str)
		REG_NOCASH_LOG = *str++;
	REG_NOCASH_LOG = '\n';
}