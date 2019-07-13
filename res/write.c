#include <stdio.h>
#include <string.h>


void elix_cstring_sanitise( char * string ) {
	size_t pos = 0;
	size_t length = strlen(string);
	while( string[pos] != 0 ) {
		if ( string[pos] == '\\' && string[pos+1] == '$' ) {
			memmove(&string[pos], &string[pos+1], length);
		}
		pos++;
		length--;
	}
}

int main(int argc, char *argv[]) {
	if ( argc > 1) {
		elix_cstring_sanitise(argv[1]);
		printf("%s\n", argv[1]);
	}
	return 0;
}
