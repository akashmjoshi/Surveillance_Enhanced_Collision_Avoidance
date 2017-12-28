#ifndef CAS_H_
#define CAS_H_
#include "evmdm6437.h"
#include "evmdm6437_dip.h"
#include "evmdm6437_led.h"

struct Background{
	Int32 mean;
	Int32 var;
};

struct rgb{
	Uint8 r;
	Uint8 g;
	Uint8 b;
};

struct RGB_background{
	Int32 r_mean;
	Int32 r_var;
	Int32 b_mean;
	Int32 b_var;
	Int32 g_mean;
	Int32 g_var;
};

struct Label_sums{
	Int32 i_sum;
	Int32 j_sum;
	Int32 count;
};

struct Coordinates{
	Int32 i;
	Int32 j;
};

struct Object{
	struct Coordinates curr_position;
	struct Coordinates prev_position;
	struct Coordinates pred_position;
	struct Coordinates target_position;
};
#endif /*CAS_H_*/
