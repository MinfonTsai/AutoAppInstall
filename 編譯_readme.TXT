zlib 1.2.4 source (zip)
 
Extract the zip, open projects\visualc6\zlib.dsp in Visual Studio (I used 2005) and compile ¡§LIB Release¡¨ (and optionally ¡§LIB Debug¡¨)
 
Copy zlib.h and zconf.h from ¡§include¡¨ to your Visual Studio ¡§include¡¨ directory, and zlib.lib (and zlibd.lib if you made it) to your Visual Studio ¡§lib¡¨ directory.
 
On 64 bit Windows, with Visual Studio 2005, this is ¡§C:\Program Files (x86)\Microsoft Visual Studio 8\VC\¡¨ so adjust for your version of Visual Studio.
 
You now just need to add ¡§zlib.lib¡¨ to your ¡§Linker -> Input -> Additional Dependencies¡¨ line in your C++ project configuration to use it (and optionally zlibd.lib for the debug version).
