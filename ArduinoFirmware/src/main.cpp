#include <Arduino.h>
#define PROCES_RESET                    0x00 // 0
#define READ_REGISTER                   0x10 // 1
#define SET_REGISTER                    0x20 // 2
#define SET_REGISTER_BIT                0x30 // 3
#define CLR_REGISTER_BIT                0x40 // 4
#define READ_REGISTER_BIT               0x50 // 5
#define WAIT_UNITL_BIT_IS_SET           0x60 // 6
#define WAIT_UNITL_BIT_IS_CLEARD        0x70 // 7
#define READ_16_BIT_REGISTER_INCR_ADDR  0x80 // 8
#define READ_16_BIT_REGISTER_DECR_ADDR  0x90 // 9
#define REPEAT_LAST_CMD_BUFFER_NUMBER   0xA0 // A
#define SET_DATA                        0xB0 // B
#define RESERVED_CMD1                   0xC0 // C
#define RESERVED_CMD2                   0xD0 // D
#define RESERVED_CMD3                   0xE0 // E
#define RESERVED_CMD4                   0xF0 // F
#define FW_VERSION                      0x08 // 6

uint8_t cmd_byte[256];
uint8_t cmd_addr=0xFF;
uint8_t cmd_executed=0xFF;
uint8_t tmp_data=0;

void setUARTto115200(){
  UCSR0A = 0b00000010;   // transm. speed 2x (zaradi manjega err)
  UCSR0B = 0b10011000;   // RxCi(en),Rx(en), Tx(en)
  UCSR0C = 0b00000110;   //
  UBRR0H = 0;            // baud =
  UBRR0L = 16;           // 115200 , err = 2.1%
  DDRD = 0b00000010;     // tx = ooutput
}
void sendData(uint8_t uart_data){
  while (!(UCSR0A & (1<<UDRE0))){} //wait for transmit enable
  UDR0 = uart_data;
}
void processReset(){
  sendData(FW_VERSION);
}
void readRegister(uint8_t registerAddress){
  sendData(_SFR_MEM8(registerAddress));
}
void setRegister(uint8_t register_address, uint8_t reg_value){
  uint8_t* regAddr;
  regAddr = reinterpret_cast<uint8_t*>(register_address);
  *regAddr = reg_value;
}
void setRegisterBit(uint8_t register_address, uint8_t bit_name){
  uint8_t old_reg_val = _SFR_MEM8(register_address);
  uint8_t new_reg_val = old_reg_val | (1<<bit_name);
  setRegister(register_address, new_reg_val);
}
void clrRegisterBit(uint8_t register_address, uint8_t bit_name){
  uint8_t old_reg_val = _SFR_MEM8(register_address);
  uint8_t new_reg_val = old_reg_val & !(1<<bit_name);
  setRegister(register_address, new_reg_val);
}
void readRegisterBit(uint8_t registerr_address, uint8_t bit){
  uint8_t reg_value = _SFR_MEM8(registerr_address);
  if (reg_value & (1<<bit)) sendData(0x01); else sendData(0x00);
}
void waitUntilBitIsSet(uint8_t register_address, uint8_t bit){
  while (!(_SFR_MEM8(register_address) & (1<<bit))){}
}
void waitUntilBitIsClr(uint8_t register_address, uint8_t bit){
  while (_SFR_MEM8(register_address) & (1<<bit)){}
}
void read16BitRegIncAddr(uint8_t register_address){
  sendData(_SFR_MEM8(register_address++));
  sendData(_SFR_MEM8(register_address));
}
void read16BitRegDecAddr(uint8_t register_address){
  sendData(_SFR_MEM8(register_address--));
  sendData(_SFR_MEM8(register_address));
}
void repeatLastNBytesOfCmds(uint8_t last_n_bytes_of_cmds){
  cmd_executed -= last_n_bytes_of_cmds;
}
void doNextCmd(){
  uint8_t cmd = cmd_byte[cmd_executed] & 0xF0; // read even bytes cmd[0], cmd[2] ...
  uint8_t bit = cmd_byte[cmd_executed++] & 0x0F;
  uint8_t addr= cmd_byte[cmd_executed++];
  switch (cmd) {
    case PROCES_RESET: processReset(); break;
    case READ_REGISTER: readRegister(addr); break;
    case SET_REGISTER: setRegister(addr, tmp_data); break;
    case SET_REGISTER_BIT: setRegisterBit(addr, bit); break;
    case CLR_REGISTER_BIT: clrRegisterBit(addr, bit); break;
    case READ_REGISTER_BIT: readRegisterBit(addr, bit); break;
    case WAIT_UNITL_BIT_IS_SET: waitUntilBitIsSet(addr, bit); break;
    case WAIT_UNITL_BIT_IS_CLEARD: waitUntilBitIsClr(addr, bit); break;
    case READ_16_BIT_REGISTER_INCR_ADDR: read16BitRegIncAddr(addr); break;
    case READ_16_BIT_REGISTER_DECR_ADDR: read16BitRegDecAddr(addr); break;
    case REPEAT_LAST_CMD_BUFFER_NUMBER: repeatLastNBytesOfCmds(addr); break;
    case SET_DATA: tmp_data = addr; break;
    default: break; 
  }
}
void setup() {
  pinMode(13,OUTPUT);
  setUARTto115200();
}
void loop() {
  if (!(cmd_executed==cmd_addr)){
    //TIMSK0 = 0; TCNT0 = 0; TIFR0 = 0;
    while (!(cmd_addr & 0x01)){
      //if (!TIFR0) PORTB=32;
      PORTB=32;
    }
    if (cmd_addr & 1) doNextCmd();
    PORTB=0;
  }
  //TIMSK0 = 1;
}
ISR(USART_RX_vect){
  cmd_byte[cmd_addr++] = UDR0;
}
