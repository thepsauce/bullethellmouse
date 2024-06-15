#include "bullethellmouse.h"

int
main(void)
{
	int *values;

	gsnew(values, 4);

	for(uint32_t i = 0; i < 1000; i++)
		gsappend(values, &(int) { rand() });

	for(uint32_t i = 0; i < gscount(values); i++)
	{
		printf("%u, %u\n", i, gscount(values));
		gsremove(values, i);
	}
	printf("%u, %u\n", gscapcount(values), gscount(values));

	gsdelete(values);
	return 0;
}
