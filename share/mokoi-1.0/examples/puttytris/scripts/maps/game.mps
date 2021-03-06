/*********************************************** 
 * Copyright © Luke Salisbury 
 * 
 * You are free to share, to copy, distribute and transmit this work 
 * You are free to adapt this work 
 * Under the following conditions: 
 *  You must attribute the work in the manner specified by the author or licensor (but not in any way that suggests that they endorse you or your use of the work).  
 *  You may not use this work for commercial purposes. 
 *  If you alter, transform, or build upon this work, you may distribute the resulting work only under the same license to this one. 
 * Full terms of use: http://creativecommons.org/licenses/by-nc-sa/3.0/ 
 ***********************************************/ 
 
#include <core> 
#include <helper> 
#include <controller> 
 
const DROPANIM = 160; 
const DROPTIMER = 450; 
const SIDETIMER = 40; 
const CLEARTIMER = 1000; 
 
const PIECES = 8; 
 
const GAMEOVER = -2; 
const NOGAME = -1; 
 
 
//TODO: fixed piece 2 alignment 
new qpieces[8][16] =[ \ 
 [0x0000,0x4000,0x5010,0x0000, 0x0105,0x3200,0x0320,0x0400, 0x0004,0x2003,0x0032,0x0501, 0x0000,0x1050,0x0040,0x0000],\ 
 [0x0000,0x1050,0x2000,0x0100, 0x0502,0x0443,0x3304,0x0205, 0x0001,0x0030,0x4000,0x0000, 0x0000,0x0020,0x5010,0x0000],\ 
 [0x0000,0x0030,0x0500,0x1000, 0x0001,0x5022,0x4443,0x2050, 0x0000,0x0314,0x0200,0x3100, 0x0000,0x0005,0x0000,0x0000],\  
 [0x1050,0x2004,0x0105,0x0000, 0x0042,0x3333,0x4200,0x0000, 0x0501,0x0420,0x5010,0x0000, 0x0000,0x0000,0x0000,0x0000],\ 
 [0x5310,0x0420,0x0531,0x0000, 0x4200,0x0000,0x0042,0x0000, 0x3105,0x2004,0x1053,0x0000, 0x0000,0x0000,0x0000,0x0000],\ 
 [0x0500,0x2041,0x0050,0x0000, 0x0412,0x3333,0x1204,0x0000, 0x5000,0x4120,0x0005,0x0000, 0x0000,0x0000,0x0000,0x0000],\ 
 [0x1542,0x2053,0x0104,0x0000, 0x0431,0x3300,0x0205,0x0000, 0x5020,0x4010,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000],\ 
 [0x0000,0x4321,0x0000,0x0000, 0x3214,0x5555,0x1432,0x0000, 0x0000,0x2143,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000]\ 
]; 
new image[5]{} = [ "puttetris.png:1", "puttetris.png:2", "puttetris.png:3", "puttetris.png:4", "puttetris.png:5" ] 
new next_area[4] = [ 12, 404, 64, 64]; // Next Piece rectangle 
new grid_area[4] = [ 66, 40, 192, 352 ];// Play Areas rectangle 
new grid[12][22] = [ [ 1, ... ], [ 2, ... ], [ 5, ...  ], [ 1, ... ], [ 2, ... ], [ 5, ... ], [ 1, ... ], [ 2, ... ], [ 5, ...  ],[ 1, ... ], [ 2, ...  ], [ 5, ...  ] ]; 
 
new piece_state[5] = [ 0, 0, 0, 0, 0 ]; /*  rot, draw x/y, block x/y */ 
new current_piece = NOGAME; //  
new next_piece = 0; 
new allowed[3]; /* bottom - left - right */ 
 
// Types of blocks that make up the pieces 
new block_types[2] = [ -1, -1 ] ; 
 
new drop_timer = DROPTIMER; 
new drop_anim = DROPANIM; 
new key_timer[3] = [ SIDETIMER, SIDETIMER, SIDETIMER ]; 
 

new clear_timer = 0; 
new clear_lines[4] = [ -1, ...  ] 
new clear_colours[11] = [ \ 
	0xFFFFFF22, \ 
	0xFFFFFF44, \ 
	0xFFFFFF66, \ 
	0xFFFFFF88, \ 
	0xFFFFFFBB, \ 
	0xFFFFFFAA, \ 
	0xFFFFFF88, \ 
	0xFFFFFF66, \ 
	0xFFFFFF44, \ 
	0xFFFFFF22, \ 
	0xFFFFFF22 \ 
]; 
 
new scoreobj[3]; // Display object IDs 
new score[3]; 
new scorestr[64]; 
new save[64] = [ 64 ]; 
 
BlockPosition( p, v ) 
{ 
	return ((v >> p) & 15)%6; 
} 
 
BlockType( p, v ) 
{ 
	return BitsGet( v, p*4, 4 ); 
} 
 
BlockSetType( &v, n, p, s ) 
{ 
	n = clamp( n, 0, (1<<s)-1); 
	v |= (n << p); 
} 
 
UpdateScores() 
{ 
	if ( score[0] > save[0] ) 
	{ 
		save[0] = score[0]; 
		//GameSave(1, save); 
	} 
	StringFormat(scorestr, _,"%d", score[0]); 
	ObjectReplace(object:scoreobj[0], scorestr, TEXT); 
	StringFormat(scorestr, _, "%d", score[1]); 
	ObjectReplace(object:scoreobj[1], scorestr, TEXT); 
	StringFormat(scorestr, _,"%d", save[0]); 
	ObjectReplace(object:scoreobj[2], scorestr, TEXT); 
} 
 
/*  
 o: piece type 
 c: left/right 
 u: up/down 
*/ 
CanMove(o, c, u) 
{ 
	if ( o < 0 || o >= PIECES) 
		return false; 
	new i = 0;  
	new line = 0; 
	new x = piece_state[3] - 2; 
	new y = piece_state[4] - 2; 
	 
	while ( i < 16 ) 
	{ 
		line = i/4; 
		if ( BlockPosition(piece_state[0]*4, qpieces[o][i]) ) 
		{ 
			x = (piece_state[3] - 2) + i - (line*4) + c; 
			y = (piece_state[4] - 2) + line + u; 
			 
			if ( x >= 0 && x < 12 && y >= 0 && y <= 21) 
			{	 
				if ( grid[x][y] ) 
					return false; 
			} 
			else 
				return false; 
		} 
		i++; 
	} 
	return true; 
} 
 
CanRotate(o, q) 
{ 
	new i = 0; 
	new line = 0; 
	new x = piece_state[3] - 2; 
	new y = piece_state[4] - 2; 
	while ( i < 16 ) 
	{ 
		line = i/4; 
		if ( BlockPosition(q*4, qpieces[o][i]) ) 
		{ 
			x = (piece_state[3] - 2) + i - (line*4); 
			y = (piece_state[4] - 2) + line; 
			if ( (x >= 0 && x < 12) && (y >= 0 && y <= 21) ) 
			{ 
				if ( grid[x][y] ) 
					return false; 
			} 
			else 
				return false; 
		} 
		i++; 
	} 
	return true; 
} 
 
DrawPiece() 
{ 
	if ( block_types[0] == -1 || current_piece < 0) 
		return; 
	new o = current_piece; 
	new i = 0; 
	new p = 0; 
	new t; 
	new line = 0; 
	new x = grid_area[0] + piece_state[1] - 32; 
	new y = grid_area[1] + piece_state[2] - 32; 
 
	while ( i < 16 ) 
	{ 
		line = i/4; 
 
		p = BlockPosition(  piece_state[0]*4, qpieces[o][i]); 
		if ( p > 0) 
		{ 
			t = BlockType( p-1, block_types[0] ); 
			GraphicsDraw( image[t], SPRITE, \ 
				x + ( (i-(line*4))*16), \ 
				y + (line*16), \ 
				6.000, 0, 0, WHITE); 
			p++; 
		} 
		i++; 
	} 
} 
 
DrawNextPiece() 
{ 
	if ( block_types[1] == -1|| next_piece < 0) 
		return; 
	new o = next_piece; 
	new i = 0; 
	new line = 0; 
	new p = 0; 
	new t; 
	while ( i < 16 ) 
	{ 
		line = i/4; 
		p = BlockPosition(  0, qpieces[o][i] ); 
		if ( p > 0) 
		{ 
			t = BlockType( p-1, block_types[1] ); 
			GraphicsDraw(image[t], SPRITE, \ 
			next_area[0] + ( (i-(line*4))*16), \ 
			next_area[1] + (line*16), \ 
			4.000, 0,0); 
		} 
		i++; 
	} 
} 
 
DrawField() 
{ 
	new y; 
	new x = 0; 
	while (  x < 192  ) 
	{ 
		y = 0; 
		while ( y < 352) 
		{ 
			if ( grid[x/16][y/16] )  
				GraphicsDraw(image[grid[x/16][y/16]-1], SPRITE, x+grid_area[0], y+grid_area[1], 4.000, 0, 0); 
			y += 16; 
		} 
		x += 16; 
	} 
} 
 
ClearField() 
{ 
	new y, x = 0; 
	for ( x = 0; x < 12; x++  ) 
	{ 
		for ( y = 0; y < 22; y++  ) 
		{	 
			grid[x][y] = 0; 
		} 
	} 
} 
 
WritePiece(o, r) 
{ 
	if ( o < 0 ) 
		return; 
	new i = 0; 
	new c = 0; 
	new line = 0; 
	new x,y; 
	new p = 0; 
 
	while ( i < 16 ) 
	{ 
		line = i/4; 
		p = BlockPosition( r*4, qpieces[o][i] ); 
		if ( p > 0) 
		{ 
			x = (piece_state[3] - 2) + i - (line*4); 
			y = (piece_state[4] - 2) + line; 
			 
			if ( 0 <= x <= 11 &&  0 <= y <= 21 ) 
			{ 
				grid[x][y] = BlockType( p-1, block_types[0] ) + 1; 
				c++; 
			} 
		} 
		i++; 
	} 
} 
 
RefreshAllow() 
{ 
	allowed[0] = CanMove(current_piece, 0,1); 
	allowed[1] = CanMove(current_piece, -1,0); 
	allowed[2] = CanMove(current_piece, 1,0); 
} 
 
LockPiece() 
{ 
	WritePiece( current_piece, piece_state[0] ); 
	new i = 21; 
	new q = 0; 
 
	clear_lines = [ -1, -1, -1, -1]; 
	while ( i > 0 ) 
	{ 
	 	if ( LineCheck(i) ) 
		{ 
			clear_timer = CLEARTIMER; 
			clear_lines[q] = i; 
			q++; 
		} 
		i--; 
	} 
	NewPiece(); 
} 
 
RandomBlockType(  ) 
{ 
	new r = random(60)%5; 
	if ( r >= 2)	 
		r = 4; 
	else if ( score[1] < 2 && r == 1 )  // No Blue block before completing 2 lines
		r = 0; 
	return r; 
} 
 
RandomizeNextPiece() 
{ 
	next_piece = random(PIECES); 
	block_types[1] = 0; 
	BlockSetType( block_types[1], RandomBlockType(), 0, 4 ); 
	BlockSetType( block_types[1], RandomBlockType(), 4, 4 ); 
	BlockSetType( block_types[1], RandomBlockType(), 8, 4 ); 
	BlockSetType( block_types[1], RandomBlockType(), 12, 4 ); 
	BlockSetType( block_types[1], RandomBlockType(), 16, 4 ); 
} 
 
NewPiece() 
{ 
	piece_state = [ 0, 96, 16, 6, 1 ]; 
	current_piece = next_piece; 
	block_types[0] = block_types[1]; 
	 
	RandomizeNextPiece(); 
 
	RefreshAllow(); 
 
	if ( !allowed[0] && !allowed[1] && !allowed[2] ) 
	{ 
		current_piece = GAMEOVER; 
	} 
} 
 
MoveObject(x,y) 
{ 
	if ( y && allowed[0] ) 
	{ 
		piece_state[4] += y; 
		piece_state[2] = piece_state[4]*16; 
	} 
	if ( x ) 
	{ 
		piece_state[3] += x; 
		if ( piece_state[3] < 0 ) 
			piece_state[3] -= x; 
		piece_state[1] = piece_state[3]*16; 
		 
	} 
	RefreshAllow(); 
} 
 
 
LineCheck( l ) 
{ 
	if ( l < 0 || l > 21 ) 
		return 0; 
	new i = 0; // Column 
	while ( i < 12 ) 
	{ 
	 	if ( !grid[i][l] ) 
			return 0; 
		i++; 
	} 
	return 1; 
 
} 
 
ClearLine(l) 
{ 
	if ( l < 0 ) 
		return; 
	new i = 0; 
	score[1]++; 
	while ( i < 12 ) 
	{ 
		if ( grid[i][l] > 3 ) 
		{ 
			score[0] += 50; 
			grid[i][l] -= 1; 
		} 
		else 
		{ 
			score[0] += 100; 
			grid[i][l] = 0; 
		} 
		i++; 
	} 
} 
 
DropLines(above) 
{ 
	if ( above < 0 ) 
		return; 
	new i = 0; // column 
	new l = above - 1; // Lines 
	while ( l > 0 ) 
	{ 
		i = 0; 
		while ( i < 12 ) 
		{ 
			if ( grid[i][l] != 2 && !grid[i][l+1] ) 
			{ 
				grid[i][l+1] = grid[i][l]; 
				grid[i][l] = 0; 
			} 
			i++; 
		} 
		l--; 
	} 
} 
 
/* 
AnimateDropLines() 
{ 
 
} 
*/ 
 
StartGame() 
{ 
	score[0] = score[1] = 0; 
	UpdateScores(); 
	ClearField(); 
	RandomizeNextPiece(); 
	NewPiece(); 
} 
 
 
HandleGame() 
{ 
	DrawField(); 
	if ( clear_timer != 0) 
	{ 
		new q = 0; 
		while( q < 4 ) 
		{ 
			if ( clear_lines[q] != -1 ) 
				GraphicsDraw("", RECTANGLE, grid_area[0] , grid_area[1] + (clear_lines[q] *16), 5.000, grid_area[2] , 16, clear_colours[(clear_timer/100)]); 
			q++; 
		} 
		if ( TimerCountdown(clear_timer) ) 
		{ 
			q = 0; 
			while( q < 4 ) 
			{ 
				ClearLine(clear_lines[q]); 
				q++; 
			} 
			q = 0; 
			while( q < 4 ) 
			{ 
				DropLines(clear_lines[q]); 
				q++; 
			}		 
			UpdateScores(); 
			clear_timer = 0; 
		} 
	} 
	else 
	{ 
 
		if ( TimerCountdown(drop_anim) ) 
		{ 
			drop_anim += DROPANIM; 
		} 
 
		/* Repeatable key presses */
		// If button is held down (2) for longer the SIDETIMER then reset button to 0
		if ( InputButton(BUTTON_ARROWRIGHT) == 2 ) 
			if ( TimerCountdownWithReset(key_timer[0], SIDETIMER) ) 
				InputButtonSet( BUTTON_ARROWRIGHT, 0); 

		if ( InputButton(BUTTON_ARROWLEFT) == 2 ) 
			if ( TimerCountdownWithReset(key_timer[1], SIDETIMER) )
				InputButtonSet( BUTTON_ARROWLEFT, 0 ); 

		if ( InputButton(BUTTON_ARROWDOWN) == 2 ) 
			if ( TimerCountdownWithReset(key_timer[2], SIDETIMER) )
				InputButtonSet( BUTTON_ARROWDOWN, 0 ); 
		/* Repeatable key presses */


		// Watch Input
		if ( InputButton(BUTTON_ARROWRIGHT) == 1  && allowed[2] ) 
			MoveObject( 1, 0); 
		else if ( InputButton(BUTTON_ARROWLEFT) == 1 && allowed[1]  ) 
			MoveObject( -1, 0); 

		if ( InputButton(BUTTON_ARROWDOWN) == 1 ) 
			MoveObject( 0,1 ); 
 
		if ( CanRotate(current_piece, (piece_state[0]+1)%4) ) 
		{ 
			if ( InputButton(BUTTON_ARROWUP) == 1 ) 
			{ 
				piece_state[0]++; 
				piece_state[0] %= 4; 
				RefreshAllow(); 
			} 
		} 
 
 
		DrawPiece(); 
		DrawNextPiece();	 
 
		if ( TimerCountdown(drop_timer) ) 
		{ 
			drop_timer += DROPTIMER; 
			if ( !allowed[0] ) 
				LockPiece(); 
			else 
				MoveObject(0, 1); 
		} 
	 
	} 
 
} 
 
GameOverScreen() 
{ 
	GraphicsDraw("Game Over", TEXT, grid_area[0] + (grid_area[2]/2) - 76, grid_area[1] + (grid_area[3]/2) - 16, 6.000, 14,14); 
	GraphicsDraw("Press ❼ to Play Again", TEXT, grid_area[0] + (grid_area[2]/2) - 76, grid_area[1] + (grid_area[3]/2), 6.000, 14,14); 
	if ( InputButton(BUTTON_MENU) == 1 ) 
	{ 
		StartGame() 
	} 
 
} 
 
NewGameScreen() 
{ 
	GraphicsDraw("Press ❼ to Play", TEXT, grid_area[0] + (grid_area[2]/2) - 76, grid_area[1] + (grid_area[3]/2) - 8, 6.000, 14,14); 
	if ( InputButton(BUTTON_MENU) == 1 ) 
	{ 
		StartGame(); 
	} 
} 
 
 
public Init(...) 
{ 
	GameGetDetails(1, save); 
 
	SheetReference("puttetris.png", 1) 
 
	scoreobj[0] = ObjectCreate("0", TEXT, 200, 408, 6.0, 0, 0, 0x000000FF); 
	scoreobj[1] = ObjectCreate("0", TEXT, 200, 432, 6.0, 0, 0, 0x000000FF); 
	scoreobj[2] = ObjectCreate("1", TEXT, 200, 456, 6.000, 0, 0, 0x000000FF); 
 
	StringFormat(scorestr, _, "%d", save[0]); 
	 
	ObjectReplace(object:scoreobj[2], scorestr, TEXT); 
 
	StartGame(); 
 
} 
 
public Close() 
{ 
	GameSave(1, save); 
	SheetReference("puttetris.png", -1) 
} 
 
main() 
{ 
	ConsoleOutput("%d-%d-%d", scoreobj[0],scoreobj[1],scoreobj[2] );
	if ( current_piece == NOGAME ) 
	{ 
		NewGameScreen(); 
	} 
	else if ( current_piece == GAMEOVER ) 
	{ 
		GameOverScreen(); 
	} 
	else 
	{ 
		HandleGame(); 
	} 
	 
 
} 
