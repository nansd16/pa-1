#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/*
FILE USED TO TEST THE FUNCTIONS INDIVIDUALLY.
*/


union value{
    unsigned uval;
    int sval;
    float fval;
    unsigned char bytes[4];
};
// reads 8 hex characters from the string input and stores them in array bytes of the union v
// returns -1 if the hexadecimal number is invalid, 0 otherwise
int read_hex(union value *v, char *input){
  for(int i=0; i<4; i++){
    char hold[3] = {input[(3 - i) * 2], input[(3 - i) * 2 + 1], '\0'};
    int num = (int)strtol(hold, NULL, 16);
    v->bytes[i]=(unsigned char) num;
  }
    return 0;
}

// shifts x left n positions and insert the n most significant bits to the right of x
unsigned lrotate(unsigned x, int n){ 
  unsigned shift = x << n;
  unsigned msb = x >> (32-n);
  msb &= (1<<n)-1;
  return (shift | msb);
}
// shifts x right n positions and insert the n least significant bits to the left of x
unsigned rrotate(unsigned x, int n){ 
  unsigned shift = x >> n;
  unsigned lsb = x << (32-n);
  lsb &= (1U<<n)-1;
  return (shift | lsb);
}

// adds two signed integers x and y and returns the sum
// If a positive overflow occurs, it returns INT_MAX
// If a negative overflow occurs, it returns INT_MIN
int saturate(int x, int y){ 
  int z = x+y;
  if(x > 0 && y > 0 && z < 0){ //positive overflow
    //positive overflow occurs if x is pos & y is pos & z is neg
    return INT_MAX;
  }else if((x<0 && y<0 && z>0)){ //negative overflow
    //negative overflow occurs if adding 2 negative numbers that are too small
    return INT_MIN;
  }else{ //no overflow
    return z;
  }
 }

// adds two unsigned integers x and y and returns the sum
// If an overflow occurs, it returns UINT_MAX
unsigned usaturate(unsigned x, unsigned y){ 
  int (unsigned)z = x+y;
  if((z < x || z < y) || z <0){
    return UINT_MAX;
  }else{
    return z;
  }
}

// multiplies the binary representation of a float number by 2
// If the binary representation is Infinity or NaN, return Infinity or Nan
// If the result of the multiplication overflows the exponent, return Infinity
unsigned float_twice(unsigned f){ 
  //multiply given float f by 2.0: 0 1000.... 0000... 0x40000000
  //convert to IEEE 754 
  unsigned int s = (f >> 31) & 1;
  unsigned int e = (f >> 23) & 0xFF;
  unsigned int m = f & 0x7FFFFFFF;
  if(e == 0){//denormalized
    m = m << 1;//m*2
  }else if(e != 0xFF){//normalized
    e++;
  }
  if(e == 0xFF){//overflow
    return (s << 31) | (0xFF << 23); //return pos/neg inf
  }
  return (s<<31) | (e<<23) | (m&0x7FFFFF);

}

// divides the binary representation of a float number by 2
// If the binary representation is Infinity of NaN, return Infinity or NaN
unsigned float_half(unsigned f){ 
  //multiply given float f by 0.5: 00111111000000000000000000000000, 0x3f000000
  //convert to IEEE 754 
  unsigned int s = (f >> 31) & 1;
  unsigned int e = (f >> 23) & 0xFF;
  unsigned int m = f & 0x7FFFFFFF;
  if(e == 0){//denormalized
    m = m >> 1;//m/2
  }else if(e != 0xFF){//normalized
    e--;
  }
  if(e == 0){//underflow
    return (s << 31);
  }
  return (s<<31) | (e<<23) | (m&0x7FFFFF);
}



int main(){
    printf("\nTesting LROTATE:");
    printf("\n%08X\n", lrotate(0x7f400000, 2));
    printf("%08X\n", lrotate(0x7f400000, 8));

    printf("\nTesting RROTATE:");
    printf("\n%08X\n", rrotate(0x7f400000, 2));
    printf("%08X\n", rrotate(0x7f400000, 8));

    printf("\nTesting SATURATE:");
    printf("\n%08X\n", saturate(0x7fffffff, 0x00000010)); //return INT_MAX
    printf("%d\n", saturate(0x84000000,0xa500ffff)); //return INT_MIN
    printf("%08X\n", saturate(0x0000ffff, 0xffff0000)); //

    printf("\nTesting USATURATE:");
    printf("\n%08X\n", usaturate(0xffffffff, 0x00000001)); //return UINT_MAX -- 4294967295
    printf("%u\n", usaturate(0xffffffff, 0x00000001)); //return UINT_MAX -- 4294967295
    printf("%08X\n", usaturate(0x0000ffff, 0xffff0000)); //return 0xffffffff

    printf("\nTesting FLOAT_TWICE:");
    printf("\n%08X\n", float_twice(0xff800000)); //0x3FC00000

    printf("\nTesting FLOAT_HALF:");
    printf("\n%08X\n", float_half(0x7f800777)); //0x3FC00000


    return 0;
}