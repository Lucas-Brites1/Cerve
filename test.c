#include "utils.h"
#include <stdio.h>

int main() {
	char* a = "Lucas";
	char* b = "Brites";
	char* conc = str_concat(a, b);
	printf("%s", conc);
	return 0;
}
