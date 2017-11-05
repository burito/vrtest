/*
Copyright (c) 2015-2016 Daniel Burke

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#include <stdio.h>

int main(void)
{
	int byte;
	int i;
	char rgba[4];
	
	while( 1 )
	{
		for(i=0; i<4; i++)
		{
			byte = getc(stdin);
			if(byte == EOF)return 0;
			rgba[i] = byte;
		}
		
		putc(rgba[2], stdout);
		putc(rgba[1], stdout);
		putc(rgba[0], stdout);
		putc(rgba[3], stdout);

		putc(0, stdout);
		putc(0, stdout);
		putc(0, stdout);
		putc(0, stdout);
	}
	return 0;
}
