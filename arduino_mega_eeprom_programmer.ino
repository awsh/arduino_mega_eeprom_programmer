/*
IO| 7 6 5 4 3 2 1 0
__|_________________|
n | d a b c d e f g | hex
____________________|
0 | 0 1 1 1 1 1 1 0 | 7e
1 | 0 0 1 1 0 0 0 0 | 30
2 | 0 1 1 0 1 1 0 1 | 6d
3 | 0 1 1 1 1 0 0 1 | 79
4 | 0 0 1 1 0 0 1 1 | 33
5 | 0 1 0 1 1 0 1 1 | 5b
6 | 0 1 0 1 1 1 1 1 | 5f
7 | 0 1 1 1 0 0 0 0 | 70
8 | 0 1 1 1 1 1 1 1 | 7f
9 | 0 1 1 1 1 0 1 1 | 7b

*/

// define pin mapping for ZIF socket

#define ZPIN3 53
#define ZPIN4 52 
#define ZPIN5 51
#define ZPIN6 50
#define ZPIN7 49
#define ZPIN8 48
#define ZPIN9 47
#define ZPIN10 46
#define ZPIN11 45
#define ZPIN12 44
#define ZPIN13 43
#define ZPIN14 42
#define ZPIN15 41
#define ZPIN16 40
#define ZPIN17 39
#define ZPIN18 38
#define ZPIN19 37
#define ZPIN20 36
#define ZPIN21 35
#define ZPIN22 34
#define ZPIN23 33
#define ZPIN24 32
#define ZPIN25 31
#define ZPIN26 30
#define ZPIN27 29
#define ZPIN28 28
#define ZPIN29 27
#define ZPIN30 26
#define ZPIN31 25
#define ZPIN32 24
#define ZPIN33 23
#define ZPIN34 22


// define pin map for 28C16 EEPROMs

#define GND ZPIN20
#define VCC ZPIN32

#define WRITE_ENABLE ZPIN29  // E PIN 21 - Active LOW
#define OUTPUT_ENABLE ZPIN28 // E PIN 20 - Active LOW
#define CHIP_ENABLE ZPIN26    // E PIN 18 - Active LOW

#define ADDRESS_0 ZPIN16  // E PIN 8
#define ADDRESS_1 ZPIN15  // E PIN 7
#define ADDRESS_2 ZPIN14  // E PIN 6
#define ADDRESS_3 ZPIN13  // E PIN 5
#define ADDRESS_4 ZPIN12  // E PIN 4
#define ADDRESS_5 ZPIN11  // E PIN 3
#define ADDRESS_6 ZPIN10  // E PIN 2
#define ADDRESS_7 ZPIN9   // E PIN 1
#define ADDRESS_8 ZPIN31  // E PIN 23
#define ADDRESS_9 ZPIN30  // E PIN 22
#define ADDRESS_10 ZPIN27 // E PIN 19

#define IO_0 ZPIN17 // E PIN 9
#define IO_1 ZPIN18 // E PIN 10
#define IO_2 ZPIN19 // E PIN 11
#define IO_3 ZPIN21 // E PIN 13
#define IO_4 ZPIN22 // E PIN 14
#define IO_5 ZPIN23 // E PIN 15
#define IO_6 ZPIN24 // E PIN 16
#define IO_7 ZPIN25 // E PIN 17


const int ADDRESS_PINS[] = { ADDRESS_0, ADDRESS_1, ADDRESS_2, ADDRESS_3, ADDRESS_4, ADDRESS_5, ADDRESS_6, ADDRESS_7, ADDRESS_8, ADDRESS_9, ADDRESS_10 };
const int IO_PINS[] = { IO_0, IO_1, IO_2, IO_3, IO_4, IO_5, IO_6, IO_7 };

void setAddress(int address) {   
    for (int i = 0; i < (sizeof(ADDRESS_PINS)/sizeof(int)); i++ ) {
        digitalWrite(ADDRESS_PINS[i], address >> i & 1);
    }
}


byte readByte(int address) {
    for ( int i = 0; i < (sizeof(IO_PINS)/sizeof(int)); i++ ) {
        pinMode(IO_PINS[i], INPUT);
    }
    
    setAddress(address);
    
    digitalWrite(OUTPUT_ENABLE, LOW); // output byte from eeprom

    byte data = 0;
    for ( int i = (sizeof(IO_PINS)/sizeof(int)); i >= 0; i-- ) {
        data = (data << 1) + digitalRead(IO_PINS[i]);
    }
    digitalWrite(OUTPUT_ENABLE, HIGH); // stop eeprom outputting byte
  
    return data;
}


void writeByte(int address, byte data) {
    setAddress(address); 
    for (int i = 0; i < (sizeof(IO_PINS)/sizeof(int)); i++ ) {
        pinMode(IO_PINS[i], OUTPUT);
        delayMicroseconds(10);
        digitalWrite(IO_PINS[i], data & 1);
        data = data >> 1;  
    }
    delay(10);
  
    digitalWrite(WRITE_ENABLE, LOW);
    delayMicroseconds(1);
    digitalWrite(WRITE_ENABLE, HIGH);
  
    delay(10);

}

void printContents(int start, int length) {
    for (int base = start; base < length; base += 16) {
        byte data[16];
        for (int offset = 0; offset <= 15; offset += 1) {
            data[offset] = readByte(base + offset);
        }

        char buf[80];
        sprintf(buf, "%03x:  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x      %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

        Serial.println(buf);
    }
}




void eraseEEPROM() {
    Serial.print("Erasing EEPROM");
    for (int address = 0; address < 2048; address += 1) {
        writeByte(address, 0xff);

        if (address % 64 == 0) {
            Serial.print(".");
        }
    }
    Serial.print(" done.");
    Serial.println();
    printContents(0, 2048);
}



void setup() {
 
    Serial.begin(57600);

    // SETUP CHIP

    // power chip
    pinMode(VCC, OUTPUT);
    digitalWrite(VCC, HIGH);
  
    pinMode(GND, OUTPUT);
    digitalWrite(GND, LOW);
  
    // write disabled
    pinMode(WRITE_ENABLE, OUTPUT);
    digitalWrite(WRITE_ENABLE, HIGH);

    // output disabled
    pinMode(OUTPUT_ENABLE, OUTPUT);
    digitalWrite(OUTPUT_ENABLE, HIGH);
  
  
    // chip enabled
    pinMode(CHIP_ENABLE, OUTPUT);
    digitalWrite(CHIP_ENABLE, LOW);

    // set address pins as OUTPUTs
    for (int i = 0; i <= 10; i += 1) {
        pinMode(ADDRESS_PINS[i], OUTPUT);
    }
  
    delay(1000);


    // END SETUP

 
    eraseEEPROM();

    printContents(0, 2048);

    Serial.println("Programming...");

    byte digits[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b };

    // positive digits for multiplexing
    for ( int value = 0; value <= 255; value++ ) {
        writeByte(value, digits[(value % 10)]);
        writeByte(value + 256, digits[(value/10) % 10]);
        writeByte(value + 512, digits[(value/100) % 10]);
        writeByte(value + 768, 0);
    }

    // two's complement
    for ( int value = -128; value <= 127; value++) {
        writeByte((byte)value + 1024, digits[abs(value) % 10]);
        writeByte((byte)value + 1280, digits[abs(value / 10) % 10]);
        writeByte((byte)value + 1536, digits[abs(value / 100) % 10]);
        if (value < 0) {
            writeByte((byte)value + 1792, 0x01);
        } else {
            writeByte((byte)value + 1792, 0);
        }
    }

    printContents(0, 2048);
}

void loop() {}
