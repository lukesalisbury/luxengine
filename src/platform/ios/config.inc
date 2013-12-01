#ifdef __GNUWIN32__
platform = "win32";
#endif

this->SetNumber("display.bpp", 0);
this->SetString("display.surface", "software");
this->SetBoolean("display.dbuff", true);
this->SetBoolean("display.fullscreen", false);
this->SetNumber("display.scale", 1);
this->SetNumber("display.limit", 60);
this->SetNumber("display.width", 512);
this->SetNumber("display.height", 384);
this->SetNumber("map.width", 512);
this->SetNumber("map.height", 384);
this->SetNumber("screen.width", 512);
this->SetNumber("screen.height", 384);
this->SetBoolean("debug.enable", false);
this->SetBoolean("audio.able", true);
this->SetString("project.title", "Lux Engine" );

this->SetString("project.id", "unknowngame" );

this->SetString("control.set1", "arrows|none|mouse|k97|k115|k100|k113|k119|k101|k13|k273|k274|k275|k276|m1|m2|m3|m4|m5" );
this->SetString("control.set2", "controlaxisA0|none|mouse|b100|b101|b102|b103|b104|b105|b106|b107|b108|b109|b110|m1|m2|m3|m4|m5" );

this->SetBoolean("save.allowed", true );
this->SetString("server.ip", "127.0.0.1" );
this->SetBoolean("server.able", false);
this->SetString("client.entity", "client");
