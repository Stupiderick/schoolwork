#pragma once
#include "extdefs.h"
#define EXTENSIONS(c) ({\
case 'g': {\
	 ext_0(editor, display, buffer, 'g');\
} break;\
case 'q': {\
	 ext_1(editor, display, buffer, 'q');\
} break;\
case 'w': {\
	 ext_1(editor, display, buffer, 'w');\
} break;\
case 'h': {\
	 ext_2(editor, display, buffer, 'h');\
} break;\
})