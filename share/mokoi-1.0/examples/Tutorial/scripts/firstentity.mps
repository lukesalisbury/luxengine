/* Initialise Three Fixed valiables to hold position */
new Fixed:x = 0.00;
new Fixed:y = 0.00;
new Fixed:z = 0.00;

/* We want to get the position on entity start up, so we use the Init function */
public Init(...)
{
  /* We want to store, position for current entity to x, y variables. A empty sting access current entity, other wise you could use a entity ID to get it's position  */
  EntityGetPosition(x, y, z);
  /* You can get the Display Object store on the map by using EntityGetSetting, more on that on next tutorial */

}

main()
{
  /* Draw object to screen each frame frame */
  GraphicsDraw("tutorial-sheet1.png:2", SPRITE, fround(x)+5, fround(y)+5, z -1.0, 0, 0, BLACK ); /* See Scripting Interface for info on Draw Function */
  // ^ highlight Function and press F1 to open up Help 
}