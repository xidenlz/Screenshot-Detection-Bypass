### Intro
Since there is no good source to hook BitBlt from gdi32.dll, I decided to release this simple project that will help you understand how to hook BitBlt and enable it to take a clean screenshot of the game window.

### Why BitBlt?
BitBlt is a function from the Windows API that is commonly used to capture screenshots of a game's screen to detect ESP overlays, visual modifications, or other forms of cheating. It is widely used as part of anti-cheat systems, including those found in games such as Valorant, as well as anti-cheat solutions like FairFight, PunkBuster, and Ricochet for Call of Duty.


### About project
This project provides two examples:

1. [internal_hook](https://github.com/xidenlz/Screenshot-Detection-Bypass/tree/main/Hook/internal_hook) that's hooks the BitBlt function.
2. [Dummy](https://github.com/xidenlz/Screenshot-Detection-Bypass/tree/main/Tests) which is just a target application used to test the hook.

The hook includes its own overlay, allowing us to verify whether screenshots remain clean. As shown below, the captured screenshot appears clean:
![here](https://github.com/xidenlz/Screenshot-Detection-Bypass/blob/main/Images/bitblit_ss.bmp)

However, an overlay is actually being rendered and is not detected in the screenshot:
![here](https://github.com/xidenlz/Screenshot-Detection-Bypass/blob/main/Images/real_ss1.png)

Overlay with the function call visible:
![here](https://github.com/xidenlz/Screenshot-Detection-Bypass/blob/main/Images/real_ss2.png)


This should work with most anti-cheat systems. However, depending on the game, additional screenshot related functions may also need to be hooked. If the game relies on BitBlt for screenshot capture, this hook should work perfectly.


### Credits
* [Extreme injector](https://github.com/master131/extremeinjector) by [@master131](https://github.com/master131)
* [MinHook](https://github.com/tsudakageyu/minhook) by the MinHook contributors.
