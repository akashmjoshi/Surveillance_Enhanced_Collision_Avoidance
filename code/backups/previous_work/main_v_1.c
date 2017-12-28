//reset init
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

// Resolution 720 * 480 (NTSC mode)
#define vWidth 720
#define vHeight 480
#define BufferNum 30

// Define a space on memory for save the information input and output (Interface data)
Uint32 buffer_out[Pixels]; //from 0x80000000
Uint32 buffer_in[Pixels]; //from 0x800A8C00, which is the same as 4 (bytes for integer) * Pixels

//Uint8 buffer1[vWidth * vHeight];
//Uint8 buffer2[vWidth * vHeight];
//Uint8 buffer3[vWidth * vHeight];
//Uint8 buffer4[vWidth * vHeight];




struct background bg_frame[Pixels*2];
struct sums buffer_sum[Pixels*2];
// Define the position of the data (refer to linker.cmd)
// Internal memory L2RAM ".l2ram" 
// External memory DDR2 ".ddr2"
#pragma DATA_SECTION(buffer_out,".ddr2")
#pragma DATA_SECTION(buffer_in,".ddr2")
//#pragma DATA_SECTION(buffer1, ".user")
//#pragma DATA_SECTION(buffer2, ".user")
//#pragma DATA_SECTION(buffer3, ".user")
//#pragma DATA_SECTION(buffer4, ".user")
#pragma DATA_SECTION(bg_frame, ".user")
#pragma DATA_SECTION(buffer_sum, ".user")
// buffer_in represents one input frame which consists of two interleaved frames.
// Each 32 bit data has the information for two adjacent pixels in a row.
// Thus, the buffer holds 720/2 integer data points for each row of 2D image and there exist 480 rows.
//
// Format: yCbCr422 ( y1 | Cr | y0 | Cb )
// Each of y1, Cr, y0, Cb has 8 bits
// For each pixel in the frame, it has y, Cb, Cr components
//
// You can generate a lookup table for color conversion if you want to convert to different color space such as RGB.
// Could refer to http://www.fourcc.org/fccyvrgb.php for conversion between yCbCr and RGB
// 

// Copy data from input buffer to output buffer
void Copy(void){
	Int32 i, j,k = 0;
	Uint32 y1, y0;
	Uint32 temp = 0;
	
	// Copy data from input buffer to output buffer and 
	// draw green square box at the center of view
	for(i=0; i<vHeight; ++i){
		for (j=0; j<vWidth/2; ++j) {
			temp = 0;
			 buffer_out[i * vWidth/2 + j] = (0xFF00FF00 & buffer_in[i * vWidth/2 + j]) + 0x00800080;
//			y0 = buffer4[k];
//			y1 = buffer4[k+1];
//			temp = temp | (y1 << 24);
//			temp = (temp | (y0 << 8 )) + 0x00800080; 
//			buffer_out[i * vWidth/2 + j] = temp;
//			k+=2;
		}
	}
}

void bg_calculation(Uint32 total_frames){
	Int32 i, j;
	Uint8 y1,y0, y_buf1, y_buf2, y_buf3, y_buf4;
	Int32 sumsq = 0;
	int index = 0;
	Int32 sum = 0, meansq = 0; 
	for(i=0; i<vHeight; ++i){
		for (j=0; j<vWidth; j+=1) {
			index = (i * vWidth) + j;
			bg_frame[index].mean = buffer_sum[index].sum/total_frames;
			sumsq =  buffer_sum[index].sumsq/total_frames;
			meansq = (bg_frame[index].mean * bg_frame[index].mean); 
			bg_frame[index].var = sumsq - meansq;
		}
	}
}


void bg_subtraction(){
	Int32 i, j;
	short y1_buf_in, y0_buf_in;
	Int32 diff1,diff0;
	int k = 4, index, m = 0;
	for(i=0; i<vHeight; ++i){
		for (j=0; j<vWidth/2; ++j){
			index = (i * vWidth/2) + j;
			y1_buf_in = buffer_in[index] >> 24;
			y0_buf_in = (0x0000FF00 & buffer_in[index]) >> 8;
			diff1 = (y1_buf_in -  bg_frame[m+1].mean) * (y1_buf_in -  bg_frame[m+1].mean);
			diff0 = (y0_buf_in - bg_frame[m].mean) * (y0_buf_in - bg_frame[m].mean);
//			if( (diff1 > k*bg_frame[m+1].var)){
			if( (diff1 > (bg_frame[m+1].var * k)) && (diff0 > (bg_frame[m].var * k))){
				buffer_out[index] = 0xFF80FF80;
			}
			else{
				buffer_out[index] =  0x00800080;
			}
			m+=2;
		}
	}
}

void StoreSum(){
	Int32 i, j, k = 0;
	Uint8 y1, y0;
	
	
	for(i=0; i<vHeight; ++i){
		for (j=0; j<vWidth/2; ++j) {
			y1 = (0xFF000000 & buffer_in[i * vWidth/2 + j]) >> 24;
			y0 = (0x0000FF00 & buffer_in[i * vWidth/2 +j]) >> 8;
			buffer_sum[k].sum += y0;
			buffer_sum[k+1].sum += y1;
			buffer_sum[k].sumsq += (y0*y0);
			buffer_sum[k+1].sumsq += (y1*y1);
			k+=2;
		}
	}

}
void initSum(){
	Int32 i, j, k = 0;
	Int32 index = 0;
	
	
	for(i=0; i<vHeight; ++i){
		for (j=0; j<vWidth; ++j) {
			index = i*vWidth + j;
			buffer_sum[index].sum = 0;
			buffer_sum[index].sumsq = 0;
		}
	}

}

//void StoreFrame(short counter){
//	Int32 i, j, k = 0;
//	Uint8 y1, y0;
//	
//	if(counter == 0){
//		for(i=0; i<vHeight; ++i){
//			for (j=0; j<vWidth/2; ++j) {
//				y1 = (0xFF000000 & buffer_in[i * vWidth/2 + j]) >> 24;
//				y0 = (0x0000FF00 & buffer_in[i * vWidth/2 +j]) >> 8;
//				buffer1[k] = y0;
//				buffer1[k+1] = y1;
//				k+=2;
////				buffer1[i * vWidth/2 + j] = (0xFF00FF00 & buffer_in[i * vWidth/2 + j]) >> 8;
//			}
//		}
//	}
//	else if(counter == 1){
//		for(i=0; i<vHeight; ++i){
//			for (j=0; j<vWidth/2; ++j) {
//				y1 = (0xFF000000 & buffer_in[i * vWidth/2 + j]) >> 24;
//				y0 = (0x0000FF00 & buffer_in[i * vWidth/2 +j]) >> 8;
//				buffer2[k] = y0;
//				buffer2[k+1] = y1;
//				k+=2;
//			}
//		}
//	}
//	else if(counter == 2){
//		for(i=0; i<vHeight; ++i){
//			for (j=0; j<vWidth/2; ++j) {
//				 y1 = (0xFF000000 & buffer_in[i * vWidth/2 + j]) >> 24;
//				y0 = (0x0000FF00 & buffer_in[i * vWidth/2 +j]) >> 8;
//				buffer3[k] = y0;
//				buffer3[k+1] = y1;
//				k+=2;
//			}
//		}
//	}
//	else{
//		for(i=0; i<vHeight; ++i){
//			for (j=0; j<vWidth/2; ++j) {
//				y1 = (0xFF000000 & buffer_in[i * vWidth/2 + j]) >> 24;
//				y0 = (0x0000FF00 & buffer_in[i * vWidth/2 +j]) >> 8;
//				buffer4[k] = y0;
//				buffer4[k+1] = y1;
//				k+=2;
//			}
//		}
//	}
//}


void main( void )
{
	Int32 total_frames = 0;
	short counter = 0;
	/* Initialize BSL */
	EVMDM6437_init();
    /* Initialize the DIP Switches & LEDs if needed */
    EVMDM6437_LED_init( );
    	
	// Initialize video input/output 
	video_loopback_test();
	initSum();
	while (1){		
       
       StoreSum();
//       	StoreFrame(counter);
//       	Copy();
		bg_subtraction();
       	counter++;
       	total_frames++;
       	
       	if(counter == 4){
       		if(total_frames > 30){
       			initSum();
     			total_frames = 1;
       		}
       		bg_calculation(total_frames);
       		counter = 0;
	    }
   }
}
