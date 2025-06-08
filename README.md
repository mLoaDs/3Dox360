# 3Dox360
 3Dox is a 3DO emulator for Xbox360 that's based on 4DO/Freedo.


<p align="center">
  <img width="105" height="180" src="https://github.com/user-attachments/assets/84625650-9855-41fb-9935-8b336aef645f">
</p>

3dox - A 3DO emulator for your Xbox 360. Based on FreeDO/FourDO/3dox Xbox

About a year ago i became interested in porting FreeDO (the 3DO emulator) to the Xbox 360. It turns out that incrediclint had an Xbox 1 port of FreeDO/FourDO port up and running called 3dox. His source code in turn became the core code that I then used for the majority of my work.

incrediclint stopped working on his Xbox 1 port due to the fact that it would not run at a good framerate due to the limitations of Xbox 1 hardware.

The Xbox 360 port initally did not fare much better. Initially it would not run. The FreeDO codebase is not Big Endian friendly. I quickly got it working on big endian and found out that emulation performance was terrible. Averaging 20-25fps.

Generally speaking, the CEL engine code is the main cause for performance issues. I spent quite some time optimizing functions to be more PPC friendly it helped gain 10-15fps or so.

At that point i tried some multi-threading but nothing seemed to help. So i abandoned the project and tossed it in the "not to release pile" due to the fact that the performance was still not great (35-40ps on average)

Fast forward to December 2012 - I decided to take another look at the sourcecode and see if i can speed it up. Long story short,  with a clear head and a fresh look at the code. The idea was to thread the DSP and CEL emulation. Some pain along the way but threading was implemented and working nicely-  Now emulation is (generally) at 60fps.

Features
========

- Fast 3DO emulation.
- Full graphics/sound support
- NVRam saving.
- Pixel Shader support.
- Big Endian friendly (PS3/Wii/Wii(U) port anyone?)
- Source code released.

Pre-Release Info/Issues
=======================

- 1p controller support only at this time.
- Sound has issues (scratchy/off pitch/generally pretty bad). 
- Random pauses and stutters occur.
- Performance issues with some games.
- Savestates are not implemented.
- .ISO extension and .BIN extension are supported only
- The launcher will load games only. There are currently no other options.
- There are graphics issues. Wrong color palette at times. Theres still endian issues to address.
- Game specific timing fixes are not implemented.
- Pixel shaders are working but you cant select one. For now you need go into the shaders folder and rename/copy the shader you want to use and called it 'default.fx'. I've included CRT, HQ4x shaders to use.


Installation Notes
==================

- Place 3DO bios file inside bios folder. File MUST be called bios.rom
- Roms go in the Roms folder. Files must be .ISO or .BIN otherwise they wont work.
- Run 3doLauncher to start.
- Press 'A' to launch the rom. While in game - right thumbstick to quit back to launcher.


Controller Mapping
==================

 Game Select Screen:
 
  A: Launches the game.
	
 In-Game:
 
  B = A
  
  A = B
  
  B = C
  
  Start = P
  
  Back = X
  
  Left Trigger = Left Button
  
  Right Trigger = Right Button
  
  DPad = DPad
  
  Right Thumb Stick Click = Exit to launcher


Credits and Thanks
==================

Freedo Team: Alexander Troosh, Maxim Grishin, Allen Wright, John Sammons, and Felix Lazarev @ www.freedo.org

FourDO Team : JohnnyDude - quite alot of 3dox is based off his code.

incrediclint for his Xbox 1 3dox sourcecode.

Artik @ Logic-Sunrise for supporting the 360 homebrew scene

Razkar @ homebrew-connection.org for supporting the 360 homebrew scene!
