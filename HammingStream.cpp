#include<HammingStream.h>
HammingStream::HammingStream(Stream& _input_stream) : input_stream(_input_stream) {
    // this->input_stream = input_stream;
    // BLA::Matrix<7,4> generator_M = {1,0,0,0,1,1,0,
    //                                 0,1,0,0,1,0,1,
    //                                 0,0,1,0,0,1,1,
    //                                 0,0,0,1,1,1,1};
    // BLA::Matrix<7,3> parity_check_M =  {1,1,0,1,1,0,0,
    //                                     1,0,1,1,0,1,0,
    //                                     0,1,1,1,0,0,1};
    // this->generator_M = ~(generator_M);
    // this->parity_check_M = ~(parity_check_M);
}
size_t HammingStream::write(uint8_t byte)
{
    input_stream.write(encode4(byte >> 4));
    input_stream.write(encode4(byte & 0xf));
    return (1);
}
size_t HammingStream::write(const uint8_t *buffer, size_t size)
{
    size_t bytes = 0;
    for(int i = 0;i<size;i++){
        bytes += write(buffer[i]);
    }
    return bytes;
}
int HammingStream::read()
{
  if (available() == 0) return (-1);
  uint8_t nibble = decode8(input_stream.read());
  return (uint8_t)((nibble << 4) | decode8(input_stream.read()));
}
int HammingStream::peek()
{
  if (available() == 0) return (-1);
  uint8_t nibble = decode8(input_stream.peek());
  return (uint8_t)((nibble << 4) | decode8(input_stream.peek()));
}
int HammingStream::available()
{
  return (input_stream.available() / 2);
}

void HammingStream::flush()
{
  input_stream.flush();
}
uint8_t HammingStream::encode4(uint8_t nibble){
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
uint8_t HammingStream::decode8(uint8_t code){
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
    //     for(int i =0;i<4;i++){
    //     bits[i] ^= syndrome[i];
    // }
    bits[4] = 0;
    bits[5] = 0;
    bits[6] = 0;
    bits[7] = 0;
    uint8_t output8 = 0;
    for(int i=0;i<8;i++){
        output8 = output8 | (bits[i] << i);
    }
    // Serial.println(output8,BIN);
    return output8;
}