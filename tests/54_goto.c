#include <stdio.h>

void fred()
{
	printf("In fred()\n");
	goto done;
	printf("In middle\n");
done:
	printf("At end\n");
}

void joe()
{
	int b = 5678;

	printf("In joe()\n");

	{
		int c = 1234;
		printf("c = %d\n", c);
		goto outer;
		printf("uh-oh\n");
	}

outer:

	printf("done\n");
}

void henry()
{
	int a;

	printf("In henry()\n");
	goto inner;

	{
		int b;
	inner:
		b = 1234;
		printf("b = %d\n", b);
	}

	printf("done\n");
}

void main()
{
	fred();
	joe();
	henry();
}
