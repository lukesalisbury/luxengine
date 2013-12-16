#include <default>
#include <graphics>
#include <string> 

new string[42]; 

main() 
{
	new pointer1 = InputPointer(0, 0); 
	new pointer2 = InputPointer(1, 0); 
	new mask = MaskGetValue(pointer1, pointer2);
	strformat(string, _,_, "Mouse %d %d - Mask Value: %d", pointer1, pointer2, mask);

	GraphicsDraw(string, TEXT, 10,100,6,0,0); 
}