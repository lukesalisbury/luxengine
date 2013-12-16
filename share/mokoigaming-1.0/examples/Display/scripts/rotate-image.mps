#include <default>
#include <graphics>
#include <entities>
#include <string>
#include <mokoi>

new obj = -1;
new ascale = 0;

public Init( ... )
{
	obj = EntityGetNumber("object-id");
}

public Close()
{
}

main()
{
	ObjectEffect(object:obj, WHITE, ascale/1000);
	ascale += GameFrame() *40;
	ascale %= 360000;

}
