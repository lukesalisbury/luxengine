forward public hit();
forward public addScore();

new object:obj;
new Fixed:weapon_delay = 0.00;
new lives = 3;
new score = 0;
new Fixed:x = 248.00;
new Fixed:y = 352.00;
new Fixed:z = 352.00;
new width = 16;

public Init(...)
{
  obj = EntityGetObject();
  EntityGetPosition( x,y, z )
}

public Close()
{

}

main()
{
    Hud();

    if( lives > 0)
    {
        new axis = InputAxis(0);
        new Fixed:movement = GameFrameSeconds() * (axis/15);

        if ( axis != 0 )
        {
            x += movement;
            ObjectPosition(obj, fround(x), fround(y), 4.0, 0, 0);
        }
        if( weapon_delay <= 0.00)
        {
            if ( InputButton(0, 0) == 1)
            {
                weapon_delay = 0.6;
                Shoot();
            }
        }
        else
        {
            weapon_delay -= GameFrameSeconds();
        }
        CheckBorders();
        CollisionSet(SELF, 0, fround(x), fround(y), 16, 16);
    }
    else
    {
        ObjectDelete(obj);
    }
}

Hud()
{
  
}

Shoot()
{
    EntityCreate("bullet", "", fround(x) + 6, fround(y) - 4, 5, MapCurrentIdent());
}

CheckBorders()
{
    if (x >= (512 - width))
    {
        x = (512 - width);
        return;
    }
    else if (x <= 0)
    {
        x = 0;
        return;
    }
}

public hit()
{
    if( lives > 0)
    {
    //lives--;
    }
}

public addScore()
{
    score += 200;
}