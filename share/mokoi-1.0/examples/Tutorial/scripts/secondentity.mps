/* We add a 'public' variable so other entities can access it */
public isplayer = 42;
#pragma unused isplayer // This line tell the compiler that isplayer variable is being used.

/* Initialise Three Fixed variables to hold position */
new Fixed:x = 0.00;
new Fixed:y = 0.00;
new Fixed:z = 0.00;

/* Display Object variable */
new object:obj; // the obj varible is tagged as object.

/* We want to get the position on entity start up, so we use the Init function */
public Init(...)
{
  /* We want to store, position for current entity to x, y variables. A empty sting access current entity, other wise you could use a entity ID to get it's position  */
  EntityGetPosition(x, y, z);
  obj = EntityGetObject() // Get the display object the entity is attacted to.
}


main()
{
  /* Get Player 1 Axis Values and add them to the entity positions */
  x += InputAxis(0) * GameFrameSeconds();
  y += InputAxis(1)* GameFrameSeconds();

  ObjectPosition(obj, fround(x), fround(y), z, 0, 0);

  EntitySetPosition( x, y, z ); // Updates the entity's position
}