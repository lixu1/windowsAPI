#include <stdio.h>


#pragma comment (lib,"libTest.lib")
#include "../libTest/lib.h"

int main(int argc, char * argv[])
{
	printf("2+3=%d", add(2, 3));
}