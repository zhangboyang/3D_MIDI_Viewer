#include "pch.h"


int main(int argc, char *argv[])
{
	GLGraphics::Instance()->InitGlut(&argc, argv);


	GLGraphics::Instance()->Run(); // should never return
	system("pause");
	return 0;
}