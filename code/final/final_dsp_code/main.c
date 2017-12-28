//background only once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cas.h"
#include "stdio.h"
#include "evmdm6437.h"
#include "evmdm6437_dip.h"
#include "evmdm6437_led.h"

extern Int16 video_loopback_test();

// size for buffer_in: 720 * 480 / 2, the reason is explained below. 
#define Pixels 172800
#define vWidth 720
#define vHeight 480
#define BufferNum 30

#define TOTAL_CARS 2
#define COLOR_SIZE TOTAL_CARS+1
#define CAR_THRESHOLD 50
#define COL_THRESHOLD 4400
#define TRAINING_FRAMES 30
#define PRED_FACTOR 4
#define TURN_DIST 60 

// Boundaries
#define T_BOUND_DIST 100
#define B_BOUND_DIST 75
#define L_BOUND_DIST 20
#define R_BOUND_DIST 20
#define HYST 10
// Define a space on memory for save the information input and output (Interface data)
Uint32 buffer_out[Pixels]; //from 0x80000000
Uint32 buffer_in[Pixels]; //from 0x800A8C00, which is the same as 4 (bytes for integer) * Pixels

Uint8 buffer1[Pixels*2];
//Uint8 buffer2[vWidth * vHeight];
//Uint8 buffer3[vWidth * vHeight];
//Uint8 buffer4[vWidth * vHeight];

int final_transmmit_flag = 0;
int frame_skip = 1;
int counter = 0;

struct Label_sums curr_centroids[COLOR_SIZE];
struct Label_sums prev_centroids[2];
struct Coordinates curr_position[2];
struct Coordinates prev_position[2];
struct Coordinates pred_position[2];
struct Object car[2];//0 -> blue  1-> orange
//struct background bg_y_frame[Pixels*2];
//struct rgb_background bg_rgb_frame[Pixels*2];
//struct rgb rgb_buffer[Pixels*2];

struct Background bg_r_frame[Pixels*2];
struct Background bg_g_frame[Pixels*2];
struct Background bg_b_frame[Pixels*2];
// Define the position of the data (refer to linker.cmd)
// Internal memory L2RAM ".l2ram" 
// External memory DDR2 ".ddr2"
#pragma DATA_SECTION(buffer_out,".ddr2")
#pragma DATA_SECTION(buffer_in,".ddr2")
#pragma DATA_SECTION(buffer1, ".user")
//#pragma DATA_SECTION(buffer2, ".user")
//#pragma DATA_SECTION(buffer3, ".user")
//#pragma DATA_SECTION(rgb_buffer, ".user")
//#pragma DATA_SECTION(bg_rgb_frame, ".user")
//#pragma DATA_SECTION(bg_c_frame, ".user")
#pragma DATA_SECTION(bg_r_frame, ".user")
#pragma DATA_SECTION(bg_g_frame, ".user")
#pragma DATA_SECTION(bg_b_frame, ".user")
// buffer_in represents one input frame which consists of two interleaved frames.
// Each 32 bit data has the information for two adjacent pixels in a row.
// Thus, the buffer holds 720/2 integer data points for each row of 2D image and there exist 480 rows.
//
// Format: yCbCr422 ( y1 | Cr | y0 | Cb )
// Each of y1, Cr, y0, Cb has 8 bits
// For each pixel in the frame, it has y, Cb, Cr components
//

/* Initializations*/
int transmit_direction(Uint16 direction, Uint16 car_select, Uint16 is_final);
Int32 total_fg_pixels = 0;

void init_car_positions(){
	int t = 0;
	
	car[0].curr_position.i = T_BOUND_DIST+10;
	car[0].curr_position.j = L_BOUND_DIST+10;
	car[0].prev_position.i = 0;
	car[0].prev_position.j = 0;
	car[0].transmit_flag = 0;
	car[1].curr_position.i = B_BOUND_DIST-10;
	car[1].curr_position.j = R_BOUND_DIST-10;
	car[1].prev_position.i = 0;
	car[1].prev_position.j = 0;
	car[1].transmit_flag = 0;
	
}
void init_bg_frame(){
	Int32 i, j, k = 0;
	Int32 temp = 0;
	Int32 index = 0;
	Uint8 cr,cb;
	Uint8 y1,y0;
	Int8 cr_conv, cb_conv, y0_conv, y1_conv;
	Uint8 r,g,b;
	Uint16 r_cont,g_cont,b_cont;
	for(i=0; i<vHeight; ++i){
		for (j=0; j<vWidth/2; ++j){
			index = i * vWidth/2 + j;
			temp = buffer_in[index];
			cr = (0x00FF0000 & temp) >> 16;
			cb = (0x000000FF & temp);
			y1 = (0xFF000000 & temp) >> 24;
			y0 = (0x0000FF00 & temp) >> 8;
			
			if(y0 < 16){ y0 = 238 + y0;}
			if(y1 < 16){ y1 = 238 + y1;}
			
    		cb_conv = cb - 128;
    		cr_conv = cr - 128;
    		
    		r_cont = y1 + cr_conv + (cr_conv >> 2) + (cr_conv >> 3) + (cr_conv >> 5);
   			g_cont = y1 - ((cb_conv >> 2) + (cb_conv >> 4) + (cb_conv >> 5)) - ((cr_conv >> 1) + (cr_conv >> 3) + (cr_conv >> 4) + (cr_conv >> 5));
    		b_cont = y1 + cb_conv + ((cb_conv >> 1) + (cb_conv >> 2) + (cb_conv >> 6));
    		
    		if(r_cont > 255){r = 255;}
    		else{r = r_cont;}
    		if(g_cont > 255){g = 255;}
    		else{g = g_cont;}
    		if(b_cont > 255){b = 255;}
    		else{b = b_cont;}
    		
    		
			bg_r_frame[k+1].mean = r;
			bg_g_frame[k+1].mean = g;
			bg_b_frame[k+1].mean = b;
			bg_r_frame[k+1].var = 20;
			bg_g_frame[k+1].var = 20;
			bg_b_frame[k+1].var = 20;
			
			//Convert to RGB
    		r = y0 + cr_conv + (cr_conv >> 2) + (cr_conv >> 3) + (cr_conv >> 5);
   			g = y0 - ((cb_conv >> 2) + (cb_conv >> 4) + (cb_conv >> 5)) - ((cr_conv >> 1) + (cr_conv >> 3) + (cr_conv >> 4) + (cr_conv >> 5));
    		b = y0 + cb_conv + ((cb_conv >> 1) + (cb_conv >> 2) + (cb_conv >> 6));
			
			if(r_cont > 255){r = 255;}
    		else{r = r_cont;}
    		if(g_cont > 255){g = 255;}
    		else{g = g_cont;}
    		if(b_cont > 255){b = 255;}
    		else{b = b_cont;}
			

			bg_r_frame[k].mean = r;
			bg_g_frame[k].mean = g;
			bg_b_frame[k].mean = b;
			bg_r_frame[k].var = 0;
			bg_g_frame[k].var = 0;
			bg_b_frame[k].var = 0;
			k +=2;
			
		}
	}
}
void Copy(void){
	Int32 i, j,k = 0;
	Uint32 y1, y0, cr, cb;
	Uint32 index = 0;
	
	for(i=0; i<vHeight; ++i){
		for (j=0; j<vWidth/2; ++j) {
			index = i * vWidth/2 + j;
//			y1 = buffer_in[index] >> 24;
//			y0 = (0x0000FF00 & buffer_in[index]) >> 8;
//			cr = (0x00FF0000 & buffer_in[index]) >> 16;
//			cb = (0x000000FF & buffer_in[index]);
			buffer_out[index] = buffer_in[index];
//			if(y0 > 220 && y1 > 220){
//				buffer_out[index] = 0x00000000;
//			}
//			buffer_out[i * vWidth/2 + j] = (0xFF00FF00 & buffer_in[i * vWidth/2 + j]) + 0x00000000;
//			else{
//				buffer_out[index] = buffer_in[index];
//			}
////			buffer_out[85000] = 0xF080F080;
//			y0 = buffer1[k];
//			y1 = buffer1[k+1];
//			if(y0 > 200 && y1 > 200){
//				buffer_out[index] = 0xF080F080;
//			}
//			else{
//				buffer_out[index] = 0x00800080;
//			}
//			k+=2;
		}
	}
}

void out_disp(void){
	Int32 i, j,k = 0;
	Uint32 avg_i, avg_j;
	Uint32 curr_obj_index = 0;
	Uint32 prev_obj_index = 0;
	Uint32 pred_obj_index = 0;
	Uint32 cols = vWidth/2;
	for(i=0; i<vHeight; ++i){
		for (j=0; j<cols; ++j) {
			buffer_out[i * cols + j] = buffer_in[i * cols + j];
		}
	}
	for(i = 0; i < 2; i++){
		if(curr_centroids[i].count > CAR_THRESHOLD){
			avg_i = car[i].curr_position.i;
			avg_j = car[i].curr_position.j;
			curr_obj_index = avg_i * cols + avg_j;
			
//			avg_i = car[i].prev_position.i;
//			avg_j = car[i].prev_position.j;
//			prev_obj_index = avg_i * vWidth/2 + avg_j;
			
			avg_i = car[i].pred_position.i;
			avg_j = car[i].pred_position.j;
			pred_obj_index = avg_i * cols + avg_j;
			
//			buffer_out[prev_obj_index] = 0xFF80FF80;
//			buffer_out[prev_obj_index+1] = 0xFF80FF80;
//			buffer_out[prev_obj_index-1] = 0xFF80FF80;
			
			buffer_out[curr_obj_index] = 0xFF80FF80;
			buffer_out[curr_obj_index+1] = 0xFF80FF80;
			buffer_out[curr_obj_index-1] = 0xFF80FF80;
			buffer_out[curr_obj_index-cols] = 0xFF80FF80;
			buffer_out[curr_obj_index+cols] = 0xFF80FF80;
			buffer_out[curr_obj_index-cols+1] = 0xFF80FF80;
			buffer_out[curr_obj_index+cols-1] = 0xFF80FF80;
			buffer_out[curr_obj_index-cols-1] = 0xFF80FF80;
			buffer_out[curr_obj_index+cols+1] = 0xFF80FF80;
			
			buffer_out[pred_obj_index] = 0xFF80FF80;
			buffer_out[pred_obj_index+1] = 0xFF80FF80;
			buffer_out[pred_obj_index-1] = 0xFF80FF80;
			buffer_out[pred_obj_index-cols] = 0xFF80FF80;
			buffer_out[pred_obj_index+cols] = 0xFF80FF80;
			buffer_out[pred_obj_index-cols+1] = 0xFF80FF80;
			buffer_out[pred_obj_index+cols-1] = 0xFF80FF80;
			buffer_out[pred_obj_index-cols-1] = 0xFF80FF80;
			buffer_out[pred_obj_index+cols+1] = 0xFF80FF80;
		}
}
	
}
//Background calculation
void bg_rgb_calculation(int total_frames){
	Int32 i, j, k =0;
	Int32 temp;
	Uint8 cr,cb,y1,y0;
	Int8 cr_conv,cb_conv,y1_conv,y0_conv;
	float d1,d0;
	Uint8 r,g,b;
	Uint16 r_cont,g_cont,b_cont;
	int index = 0;
	for(i=0; i<vHeight; ++i){
		for (j=0; j<vWidth/2; j+=1) {
			index = (i * vWidth/2) + j;
			temp = buffer_in[index];
			cr = (0x00FF0000 & temp) >> 16;
			cb = (0x000000FF & temp);
			y0 = (0x0000FF00 & temp) >> 8;
			
			if(y0 < 16){ y0 = 238 + y0;}


    		cb_conv = cb - 128;
    		cr_conv = cr - 128;
    		
    		r_cont = y0 + cr_conv + (cr_conv >> 2) + (cr_conv >> 3) + (cr_conv >> 5);
   			g_cont = y0 - ((cb_conv >> 2) + (cb_conv >> 4) + (cb_conv >> 5)) - ((cr_conv >> 1) + (cr_conv >> 3) + (cr_conv >> 4) + (cr_conv >> 5));
    		b_cont = y0 + cb_conv + ((cb_conv >> 1) + (cb_conv >> 2) + (cb_conv >> 6));
    		
			if(r_cont > 255){r = 255;}
    		else{r = r_cont;}
    		if(g_cont > 255){g = 255;}
    		else{g = g_cont;}
    		if(b_cont > 255){b = 255;}
    		else{b = b_cont;}
    		
    		bg_r_frame[k].mean =  (r + ((total_frames - 1) * bg_r_frame[k].mean))/total_frames;
			d0 = (r - bg_r_frame[k].mean) * (r - bg_r_frame[k].mean);
			bg_r_frame[k].var = d0/total_frames;
			
			bg_g_frame[k].mean = ( g + ((total_frames - 1) * bg_g_frame[k].mean))/total_frames;
			d0 = (g - bg_g_frame[k].mean) * (g - bg_g_frame[k].mean);
			bg_g_frame[k].var = d0/total_frames;
			
			bg_b_frame[k].mean =  (b + ((total_frames - 1) * bg_b_frame[k].mean))/total_frames;
			d0 = (b - bg_b_frame[k].mean) * (b - bg_b_frame[k].mean);
			bg_b_frame[k].var = d0/total_frames;
			
			k += 2;
		}
	}
}
//Modified version of above
void bg_std_subtraction(){
	Int32 i, j, temp;
	Uint8 cr, cb;
	Uint8 y1, y0;
	Int8 cr_conv,cb_conv,y1_conv,y0_conv;
	Uint8 r,g,b;
	Uint16 r_cont,g_cont,b_cont;
	Int32 diff_r,diff_g, diff_b;
	Int32 cmp_r,cmp_g, cmp_b;
	
	Int32 k = 16; 
	
	Uint32 index, m = 0;
	for(i=0; i<vHeight; ++i){
		for (j=0; j<vWidth/2; ++j){
    		index = (i * vWidth/2) + j;
			temp = buffer_in[index];
			cr = (0x00FF0000 & temp) >> 16;
			cb = (0x000000FF & temp);
			y0 = (0x0000FF00 & temp) >> 8;
			
			if(y0 < 16){ y0 = 238 + y0;}
			
    		cb_conv = cb - 128;
    		cr_conv = cr - 128;
    		
    		r_cont = y0 + cr_conv + (cr_conv >> 2) + (cr_conv >> 3) + (cr_conv >> 5);
   			g_cont = y0 - ((cb_conv >> 2) + (cb_conv >> 4) + (cb_conv >> 5)) - ((cr_conv >> 1) + (cr_conv >> 3) + (cr_conv >> 4) + (cr_conv >> 5));
    		b_cont = y0 + cb_conv + ((cb_conv >> 1) + (cb_conv >> 2) + (cb_conv >> 6));
    		
			if(r_cont > 255){r = 255;}
    		else{r = r_cont;}
    		if(g_cont > 255){g = 255;}
    		else{g = g_cont;}
    		if(b_cont > 255){b = 255;}
    		else{b = b_cont;}


			struct Background temp_r = bg_r_frame[m];
			struct Background temp_g = bg_g_frame[m];
			struct Background temp_b = bg_b_frame[m];
			diff_r = r - temp_r.mean;
			diff_g = g - temp_g.mean;
			diff_b = b - temp_b.mean;
			cmp_r = (temp_r.var+2)*k;
			cmp_b = (temp_g.var+2)*k;
			cmp_g = (temp_b.var+2)*k;
			
			if((diff_r > cmp_r || diff_r < (~cmp_r)+1)
			|| (diff_g > cmp_g || diff_g < (~cmp_g)+1)
			|| (diff_b > cmp_b || diff_b < (~cmp_b)+1)
			){
				buffer1[m+1] = 255;
				buffer1[m] = 255;
				
			}
			else{
				buffer1[m+1] = 0;
				buffer1[m] = 0;
			}
			m+=2;
		}
	}
}

/*Object Detection*/
void obj_classification(){
	Int32 i, j, k = 0, index;
	Uint8 y1, y0;
	Uint8 cr_in, y_in, cb_in;
	Int32 temp;
	int curr_label;
	for(i = 0; i < COLOR_SIZE; i++){
		curr_centroids[i].i_sum = 0;
		curr_centroids[i].j_sum = 0;
		curr_centroids[i].count = 0;
	}
	 for(i=0; i<vHeight; ++i){
		 for (j=0; j<vWidth/2; ++j) {
		 	 index = (i * vWidth/2) + j;
			 y0 = buffer1[k];
			 if(y0 > 200){
			 	temp = buffer_in[index];

			 	y_in = (0x0000FF00 & temp) >> 8;
				cb_in = (0x000000FF & temp);
				cr_in = (0x00FF0000 & temp) >> 16;
				if(y_in > 20){
					if( cb_in - cr_in > 20){
						curr_label = 1; // BLUE
					}
					else if( cr_in - cb_in > 20){
						curr_label = 2; // RED
					}
					else{
						curr_label = 3; // GARBAGE
					}
				}
				else{
					curr_label = 3; // GARBAGE
				}
				
				curr_centroids[curr_label-1].i_sum += i;
				curr_centroids[curr_label-1].j_sum += j;
				curr_centroids[curr_label-1].count += 1;
				total_fg_pixels+=1;
			 }
			 k+=2;
		 }
	 }

}

void centroid_calc(void){	
	Int32 i, j,k = 0;
	for(i = 0; i < 2; i++){
		if(curr_centroids[i].count > 20){
			car[i].curr_position.i = curr_centroids[i].i_sum/curr_centroids[i].count;
			car[i].curr_position.j = (curr_centroids[i].j_sum/curr_centroids[i].count);
		}
		if(prev_centroids[i].count > 20){
			car[i].prev_position.i = prev_centroids[i].i_sum/prev_centroids[i].count;
			car[i].prev_position.j = (prev_centroids[i].j_sum/prev_centroids[i].count);
		}
	}
	car[0].pred_position.i = car[0].curr_position.i + (PRED_FACTOR*(car[0].curr_position.i  - car[0].prev_position.i));
	car[0].pred_position.j = car[0].curr_position.j + (PRED_FACTOR*(car[0].curr_position.j  - car[0].prev_position.j));
	car[1].pred_position.i = car[1].curr_position.i + (PRED_FACTOR*(car[1].curr_position.i  - car[1].prev_position.i));
	car[1].pred_position.j = car[1].curr_position.j + (PRED_FACTOR*(car[1].curr_position.j  - car[1].prev_position.j));

}

void update_position(void){
	int i = 0;
	if(car[1].transmit_flag != 0 || car[0].transmit_flag != 0){
		transmit_direction(0,0,1);
	}
	else{
		transmit_direction(0,0,0);
	}
	for(i = 0; i < 2; i++){
		prev_centroids[i].i_sum = curr_centroids[i].i_sum;
		prev_centroids[i].j_sum = curr_centroids[i].j_sum;
		prev_centroids[i].count = curr_centroids[i].count;
		
		Int32 pred_i = car[i].pred_position.i;		
		Int32 pred_j = car[i].pred_position.j;
		if((pred_i > T_BOUND_DIST && pred_i < (480 - B_BOUND_DIST )
		&& pred_j > L_BOUND_DIST && pred_j < (360 - R_BOUND_DIST ))
		&& car[i].transmit_flag == 1){
			car[i].transmit_flag = 0;
		}
		
	}
}

int transmit_direction(Uint16 direction, Uint16 car_select, Uint16 is_final){
	Uint16 dir_state = 0;
	Uint16 led_state = 0;
	//Direction = 0, 1, 2, 3
	// 3 -> U-turn
	//1 -> Pin81 , 2 -> Pin82, 3 -> Both Pins
	car_select = (car_select << 1);
	Uint16 ready_transmit  = 1;
	Uint16 transmit_state = 0 | (is_final << 2);
	EVMDM6437_I2C_GPIO_getall( I2C_GPIO_GROUP_2, &transmit_state );
	transmit_state = transmit_state & 0xFFF8;
	transmit_state = transmit_state | ready_transmit | car_select;
	EVMDM6437_I2C_GPIO_setall( I2C_GPIO_GROUP_2, transmit_state );
	
	if ( EVMDM6437_I2C_GPIO_getall( LED_I2C_ADDR, &led_state ) ){
        return -1;
	}
	led_state = led_state & 0xFF3F;
	dir_state = direction << 6;
    dir_state = (led_state |  dir_state) ;
    if ( EVMDM6437_I2C_GPIO_setall( LED_I2C_ADDR, dir_state ) ){
        return -1;
    }

	EVMDM6437_I2C_GPIO_getall( I2C_GPIO_GROUP_2, &transmit_state );
	transmit_state = transmit_state & 0xFFF8;
    ready_transmit = 0;
    transmit_state = transmit_state | ready_transmit | car_select;
    EVMDM6437_I2C_GPIO_setall( I2C_GPIO_GROUP_2, transmit_state );
    return 0;	
}

void slope_calc(int select){
	Int32 left_j,left_i,right_i,right_j;
	Int32 pred_i = car[select].pred_position.i;
	Int32 pred_j = car[select].pred_position.j;
	Int32 curr_i = car[select].curr_position.i;
	Int32 curr_j = car[select].curr_position.j;

	if((curr_i - pred_i ==0) || (curr_j - pred_j) == 0){
		transmit_direction(3,select,0);
		return;
	}
	// x2-x1 / y2-y1
	float m = -((float)(curr_j - pred_j))/((float)(curr_i - pred_i));
	float j_mul_factor = (TURN_DIST)/sqrt((m*m)+1);
	float i_mul_factor = j_mul_factor*m;
	
	struct Coordinates point_1, point_2;
	point_1.i = pred_i + (i_mul_factor);
	point_1.j = pred_j + (j_mul_factor);
	point_2.i = pred_i - (i_mul_factor);
	point_2.j = pred_j - (j_mul_factor);
	Int32 value_1 = ((pred_j - curr_j)*(point_1.i - curr_i)) - ((point_1.j -curr_j)*(pred_i - curr_i));
//	Int32 value_2 = (pred_j - curr_j)*(point_2.i - curr_i) - (point_2.j -curr_j)*(pred_i - curr_i);
	if(value_1 < 0){
		 left_j = point_1.j;
		 left_i = point_1.i;
		 right_j = point_2.j;
		 right_i = point_2.i;
	}
	else{
		 left_j = point_2.j;
		 left_i = point_2.i;
		 right_j = point_1.j;
		 right_i = point_1.i;
	}
	
	
	
	if((left_i < T_BOUND_DIST || left_i > (480 - B_BOUND_DIST)
	|| left_j < L_BOUND_DIST || left_j > (360 - R_BOUND_DIST))
	&& (right_i < T_BOUND_DIST || right_i > (480 - B_BOUND_DIST)
	|| right_j < L_BOUND_DIST || right_j > (360 - R_BOUND_DIST))
	&& car[select].transmit_flag == 0){
		transmit_direction(3,select,0);
		car[select].transmit_flag = 1;
	}
	else if((left_i < T_BOUND_DIST || left_i > (480 - B_BOUND_DIST)
	|| left_j < L_BOUND_DIST || left_j > (360 - R_BOUND_DIST))
	&& car[select].transmit_flag == 0){
		transmit_direction(2,select,0);
		car[select].transmit_flag = 1; // RIGHT TURN
	}
	else if(car[select].transmit_flag == 0){
		transmit_direction(1,select,0);
		car[select].transmit_flag = 1;//LEFT TURN
	}
}

void check_in_bounds(int select){
	
	Int32 pred_i = car[select].pred_position.i;
	Int32 pred_j = car[select].pred_position.j;
	
	if((pred_i < T_BOUND_DIST || pred_i > (480 - B_BOUND_DIST)
	|| pred_j < L_BOUND_DIST || pred_j > (360 - R_BOUND_DIST))
	&& car[select].transmit_flag == 0){
		transmit_direction(3,select,0);
//		slope_calc(select);
		car[select].transmit_flag = 1;
	}
}

void turn_car(){
	Int32 left_j,left_i,right_i,right_j;
	Int32 car1_pred_i = car[1].pred_position.i;
	Int32 car1_pred_j = car[1].pred_position.j;
	Int32 car1_curr_i = car[1].curr_position.i;
	Int32 car1_curr_j = car[1].curr_position.j;
	Int32 car0_pred_i = car[0].pred_position.i;
	Int32 car0_pred_j = car[0].pred_position.j;
	
	Int32 value_1 = ((car1_pred_j - car1_curr_j)*(car0_pred_i - car1_curr_i))
		-((car0_pred_j -car1_curr_j)*(car1_pred_i - car1_curr_i));
	if(value_1 <= 0 && car[1].transmit_flag == 0){
		//car 0 is on the left
		transmit_direction(2,1,0);
		car[1].transmit_flag = 1; // RIGHT TURN
	}
	else if(car[1].transmit_flag == 0){
		transmit_direction(1,1,0);
		car[1].transmit_flag = 1;//LEFT TURN
	}
}
void check_collision(){
	Int32 car_0_pred_i = car[0].pred_position.i;
	Int32 car_0_pred_j = car[0].pred_position.j;
	Int32 car_1_pred_i = car[1].pred_position.i;
	Int32 car_1_pred_j = car[1].pred_position.j;
	
	Int32 dist = ((car_0_pred_i - car_1_pred_i)*(car_0_pred_i - car_1_pred_i) + 
			(car_0_pred_j - car_1_pred_j)*(car_0_pred_j - car_1_pred_j));
	
	if(dist < COL_THRESHOLD){
//		int prev_flag = car[1].transmit_flag;
		car[1].transmit_flag = 0;
		turn_car();
//		car[1].transmit_flag = prev_flag;
//		transmit_direction(3,1);
	}
//	else if(car[1].transmit_flag == 1 && dist > COL_THRESHOLD){
//		car[1].transmit_flag = 0;
//	}
}
	
void main( void )
{
	Int32 total_frames = 0;
	/* Initialize BSL */
	EVMDM6437_init();
    /* Initialize the DIP Switches & LEDs if needed */
    EVMDM6437_LED_init( );
    EVMDM6437_DIP_init( );
	// Initialize video input/output 
	video_loopback_test();
	
	EVMDM6437_LED_off(0);
	init_car_positions();
	init_bg_frame();
	while(total_frames < TRAINING_FRAMES){
		total_frames++;
		bg_rgb_calculation(total_frames);
	}
	EVMDM6437_LED_on(0);
	total_frames = 0;
	Uint16 dip0, dip1;
	while (1){
			bg_std_subtraction();
			obj_classification();
			centroid_calc();
//			Copy();
//			dip0 = EVMDM6437_DIP_get(0);
//			dip1 = EVMDM6437_DIP_get(1);
//			
//			if(dip0 == DIP_DOWN){
//				transmit_direction(1);
//				_waitusec(500000);
//			}
//			if(dip1 == DIP_DOWN){
//				transmit_direction(2);
//			}
//			if(dip1 == DIP_DOWN && dip0 == DIP_DOWN){
//				transmit_direction(3);
//			}
			out_disp();
			check_in_bounds(1);
			check_in_bounds(0);
			check_collision();
			update_position();
			total_fg_pixels = 0;
   }
}
