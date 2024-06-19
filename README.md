### Intro
Since there is no good source to hook BitBlt from gdi32.dll, I decided to release this simple project that will help you understand how to hook BitBlt and enable it to take a clean screenshot of the game window.

### Why BitBlt?
BitBlt is a function from the Windows API commonly used to capture screenshots of the game screen to detect any ESP drawing or cheating. It is widely used as an anti-cheating method, for example in Valorant, FairFight, PunkBuster, and Ricochet from Call of Duty.

### About the code
The code is well structured to ensure it is understandable. As you can see, the logic is simple: we have different classes one for our cheat settings and another containing imports for BitBlt and our hooked BitBlt. With this code, when BitBlt is called, our hooked function is triggered. It calls our TurnOFF function to disable our drawing. If the result is not empty, we return to our drawing method.
```cpp
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
```

### Support
I plan to release more content about game hacking techniques that have never been explained before, so please consider starring the repo. It means a lot to me.
