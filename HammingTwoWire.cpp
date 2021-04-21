#include<HammingTwoWire.h>
HammingTwoWire::HammingTwoWire(TwoWire& _input_stream) : input_stream(_input_stream) {
    // this.input_stream = input_stream;
    // BLA::Matrix<7,4> generator_M = {1,0,0,0,1,1,0,
    //                                 0,1,0,0,1,0,1,
    //                                 0,0,1,0,0,1,1,
    //                                 0,0,0,1,1,1,1};
    // BLA::Matrix<7,3> parity_check_M =  {1,1,0,1,1,0,0,
    //                                     1,0,1,1,0,1,0,
    //                                     0,1,1,1,0,0,1};
    // this.generator_M = ~(generator_M);
    // this.parity_check_M = ~(parity_check_M);
}
size_t HammingTwoWire::write(uint8_t byte)
{
    input_stream.write(encode4(byte >> 4));
    input_stream.write(encode4(byte & 0xf));
    return (1);
}
size_t HammingTwoWire::write(const uint8_t *buffer, size_t size)
{
    size_t bytes = 0;
    for(int i = 0;i<size;i++){
        bytes += write(buffer[i]);
    }
    return bytes;
}
int HammingTwoWire::read()
{
  if (available() == 0) return (-1);
  uint8_t nibble = decode8(input_stream.read());
  uint8_t nibble2 = decode8(input_stream.read());
  uint8_t tmp = (uint8_t)((nibble << 4) | (nibble2 & 0b1111));
  Serial.print("read: ");
  Serial.println(tmp,BIN);
  Serial.println(nibble,BIN);
  Serial.println(nibble2,BIN);
  return tmp;
}
int HammingTwoWire::peek()
{
  if (available() == 0) return (-1);
  uint8_t nibble = decode8(input_stream.peek());
  return (uint8_t)((nibble << 4) | decode8(input_stream.peek()));
}
int HammingTwoWire::available()
{
  return (input_stream.available() / 2);
}

void HammingTwoWire::flush()
{
  input_stream.flush();
}
uint8_t HammingTwoWire::encode4(uint8_t nibble){
    // make bits array for easier math
    uint8_t bits[8] = {0,0,0,0,0,0,0,0};
    for (int i=0; i<8; i++) {
        bits[i] = ((1 << i) & nibble) >> i;
    }

    // Serial.print("Encode: ");
    // Serial.print(nibble,BIN);
    // Serial.print(", [ ");
    // for(int i=0;i<8;i++){
    //     Serial.print(bits[i], BIN);
    //     if(i < 7){
    //         Serial.print(", ");
    //     }
    // }
    // Serial.print(" ], ");
    bits[4] = (bits[1] ^ bits[2] ^ bits[3]);
    bits[5] = (bits[0] ^ bits[2] ^ bits[3]);
    bits[6] = (bits[0] ^ bits[1] ^ bits[3]);
    bits[7] = (bits[0] ^ bits[1] ^ bits[2]);

    // bits[2] = !bits[2];
    // bits[3] = !bits[3];
    uint8_t output8 = 0;
    for(int i=0;i<8;i++){
        output8 = output8 | (bits[i] << i);
    }
    // Serial.println(output8,BIN);
    return output8;
}
    // BLA::Matrix<7,3> parity_check_M =  {1,1,0,1,1,0,0,
    //                                     1,0,1,1,0,1,0,
    //                                     0,1,1,1,0,0,1};
uint8_t HammingTwoWire::decode8(uint8_t code){
    // make bits array for easier math
    uint8_t bits[8] = {0,0,0,0,0,0,0,0};
    for (int i=0; i<8; i++) {
        bits[i] = ((1 << i) & code) >> i;
    }
    uint8_t syndrome[4] = {0,0,0,0};
    syndrome[0] = (bits[1] ^ bits[2] ^ bits[3] ^ bits[4]);
    syndrome[1] = (bits[0] ^ bits[2] ^ bits[3] ^ bits[5]);
    syndrome[2] = (bits[0] ^ bits[1] ^ bits[3] ^ bits[6]);
    syndrome[3] = (bits[0] ^ bits[1] ^ bits[2] ^ bits[7]);

    // bits[5] = (bits[1] ^ bits[2] ^ bits[3]);
    // bits[6] = (bits[0] ^ bits[2] ^ bits[3]);
    // bits[7] = (bits[0] ^ bits[1] ^ bits[3]);
    // bits[7] = (bits[0] ^ bits[1] ^ bits[2]);

    // Serial.print("Decode: ");
    // Serial.print(code,BIN);
    // Serial.print(", [ ");
    // for(int i=0;i<8;i++){
    //     Serial.print(bits[i], BIN);
    //     if(i < 7){
    //         Serial.print(", ");
    //     }
    // }
    // Serial.print(" ], ");
    //    Serial.print(", [ ");
    // for(int i=0;i<4;i++){
    //     Serial.print(syndrome[i], BIN);
    //     if(i < 3){
    //         Serial.print(", ");
    //     }
    // }
    // Serial.print(" ], ");
    // created error table in python program, this is a logical version of that table
    // table 
    // 0  :  [0, 1, 1, 1]
    // 1  :  [1, 0, 1, 1]
    // 2  :  [1, 1, 0, 1]
    // 3  :  [1, 1, 1, 0]
    // 4  :  [1, 0, 0, 0]
    // 5  :  [0, 1, 0, 0]
    // 6  :  [0, 0, 1, 0]
    // 7  :  [0, 0, 0, 1]
    int tmp = syndrome[0]+syndrome[1]+syndrome[2]+syndrome[3];
    if(tmp == 1){
        for(int i =3;i<7;i++){
            bits[i] ^= syndrome[i-3];
        }
    }else if(tmp > 1){
        for(int i =0;i<4;i++){
            bits[i] ^= !syndrome[i];
        }  
    }

    // if(syndrome[0] == 1 | syndrome[1] == 1 | syndrome[2] == 1 | syndrome[3] == 1){
    //     Serial.println("Syndrome Wrong");
    // }
    uint8_t output8 = 0;
    for(int i=0;i<8;i++){
        output8 = output8 | (bits[i] << i);
    }
    // Serial.println(output8,BIN);
    return output8;
}