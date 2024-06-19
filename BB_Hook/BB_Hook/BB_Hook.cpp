#include <iostream>
#include "MinHook.h"

/* Credits:

	https://github.com/Mes2d 

	MIT License

	Copyright (c) 2024 Musaed

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

*/



namespace drawing // Our cheat that we want to hide from BitBlt screenshot 
{
	class draw_stuff
	{
	public:
		bool ESP = true;
		bool ESP_LINE = true;
		bool chams = true;
	};
}
drawing::draw_stuff* m_Draw; // Pointer to access our drawing class 





void TurnOFF() // Our turn off function that will access the cheat class to turn off our drawing
{

	/* Turn off our drawing once the function called */
	m_Draw->chams = false;
	m_Draw->ESP = false;
	m_Draw->ESP_LINE = false;
}

namespace Import
{

		/* Import BitBlt */
		typedef BOOL(WINAPI* tBitBlt)(HDC, int, int, int, int, HDC, int, int, DWORD);
		tBitBlt oBitBlt = nullptr;


		/* Our Hooked BitBlt */
		BOOL WINAPI hkBitBlt(HDC hdcDest, int xDest, int yDest, int width, int height, HDC hdcSrc, int xSrc, int ySrc, DWORD rop) 
		{

			TurnOFF(); // Turn OFF our drawing because BitBlt called

			BOOL result = oBitBlt(hdcDest, xDest, yDest, width, height, hdcSrc, xSrc, ySrc, rop);


			/* Drawing has been turned off, return to result AKA Clean screenshot */
			return result;

			/* Return to drawing */
			if (result != NULL)
			{
				printf("Clean screenshot has been taken\n");

				m_Draw->chams = true;
				m_Draw->ESP = true;
				m_Draw->ESP_LINE = true;
			}
		}
}

void SetupBitBltHook() {
	try 
	{
		MH_Initialize(); 
		MH_CreateHook(&BitBlt, &Import::hkBitBlt, reinterpret_cast<LPVOID*>(&Import::oBitBlt));
		MH_EnableHook(&BitBlt);
		std::cout << "BitBlt Hooked" << std::endl;
	}
	catch (const std::exception& e) 
	{
		std::cerr << "Error setting up BitBlt hook: " << e.what() << std::endl;
	}
}








int main()
{
	SetupBitBltHook();
}