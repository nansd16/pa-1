/*
 *  CSE202: Data Representation Program
 *  Full name: Natalie Dumler
 *  Full Lehigh Email Address: nad326@lehigh.edu
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


// union value
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
  unsigned shift = x << n; //shifts left
  unsigned msb = x >> (32-n); //inserts msb
  msb &= (1<<n)-1; //normalizes
  return (shift | msb);
}
// shifts x right n positions and insert the n least significant bits to the left of x
unsigned rrotate(unsigned x, int n){ 
  //check if inputs r valid
  unsigned shift = x >> n; //shifts right
  unsigned lsb = x << (32-n);
  //lsb &= (1U<<n)-1;
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
  int unsigned z = x+y; //adds ints
  if((x+y < x || z < x+y) || z < 0){ //tests for overflow
    return UINT_MAX;
  }else{
    return z;
  }
}

// multiplies the binary representation of a float number by 2
// If the binary representation is Infinity or NaN, return Infinity or Nan
// If the result of the multiplication overflows the exponent, return Infinity
unsigned float_twice(unsigned f){ 
  //multiply given float f by 2.0: 0 1000.... 0000... e=0x80000000
  //convert to IEEE 754 
  unsigned int s = (f >> 31) & 1;
  unsigned int e = (f >> 23) & 0xFF;
  unsigned int m = f & 0x7FFFFFFF;
  if(e == 0){//denormalized
    m = m << 1;//m*2
    if(m & 0x800000){//check for overflow
      m = m & 0x7FFFFF;
      e++;
    }
  }else if(e != 0xFF){//normalized
    e++;
    if(e==0xFF){ //overflow, return pos/neg inf
      return (s << 31) | (0xFF << 23);
    }
  }
  /*
  if(e == 0xFF){//overflow
    return (s << 31) | (0xFF << 23); //return pos/neg inf
  }
  */
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
  if(e == 0xFF){ //nan/inf
  return f;
  }
  if(e == 0){//denormalized
    m = m >> 1;//m/2
    if(m & 0x400000){
      m = m & 0x3FFFFF;
      e++;
    }
  }else{//normalized -- if(e != 0xFF)
    e--;
  }
  if(e == 0){//underflow
    return (s << 31);
  }
  return (s<<31) | (e<<23) | (m&0x7FFFFF);
}
int hexCheck(char* argv){ //made to check if input hex values are valid
  char hexVals[] = ("0123456789ABCDEFabcdef");
  size_t len = strlen(argv);
  
  for(size_t i =0; i<len; i++){ //iterates through each char in argv & in hexVals & see if they match
    int found = 0;
    for(int j=0; j<strlen(hexVals); j++){
      if(argv[i] == hexVals[j]){
        found = 1;
        break;
      }
    }
    if(found == 0){ //if a valie in argv is not in hexVals, 0 returned
      return 1;
    }
  }
  return 0;
}

// main function
int main(int argc, char** argv){
    if(argc != 3 && argc != 4){
        printf("Invalid number of arguments");
        exit(0);
    }
    char op[9];
    strcpy(op, argv[1]);
    union value val;
    if((read_hex(&val, argv[2]) < 0) || hexCheck(argv[2]) != 0){ //added to check for valid hex
      printf("Invalid hex value\n");
      exit(0);
    }
    if(strcmp(op, "lrotate") == 0){
      int n = atoi(argv[3]);
      if(n > 31){
        printf("Invalid number of shift positions");
        exit(0);
      }
      else{
        union value result;
        result.uval = lrotate(val.uval, n);
        printf("%08x\n", result.uval);
      }
    }
    else if(strcmp(op, "rrotate") == 0){
      int n = atoi(argv[3]);
      if(n > 31){
        printf("Invalid number of shift positions\n");
        exit(0);
      }
      else{
        union value result;
        result.uval = rrotate(val.uval, n);
        printf("%08x\n", result.uval);
      }
    }
    else if(strcmp(op, "saturate") == 0){
      union value val2;
      read_hex(&val2, argv[3]);
      union value result;
      result.uval = saturate(val.sval, val2.sval);
      printf("%08x %d\n", result.uval, result.sval);
    }
    else if(strcmp(op, "usaturate") == 0){
      union value val2;
      read_hex(&val2, argv[3]);
      union value result;
      result.uval = usaturate(val.uval, val2.uval);
      printf("%08x %u\n", result.uval, result.uval);
    }
    else if(strcmp(op, "twice") == 0){
      union value result;
      result.uval = float_twice(val.uval);
      printf("%08x %e\n", result.uval, result.fval);
    }
    else if(strcmp(op, "half") == 0){
      union value result;
      result.uval = float_half(val.uval);
      printf("%08x %e\n", result.uval, result.fval);
    }else{
      printf("Invalid operation\n");
    }
}
