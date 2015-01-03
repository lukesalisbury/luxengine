/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

extern int32_t lux_map_width;
extern int32_t lux_map_height;
extern fixed lux_map_height_fixed;

bool Lux_Util_PointCollide( LuxRect a, int32_t x, int32_t y );

/*inline uint32_t XY2Screen(int32_t x, int32_t y, int32_t w) { return (uint32_t)(((y/lux_map_height)*w)+(x/lux_map_height)); }*/
inline fixed Screen2X(uint32_t screen, int32_t w) { return (fixed)(screen%w); }
inline fixed Screen2Y(uint32_t screen, int32_t w) { return (fixed)(screen/w); }

