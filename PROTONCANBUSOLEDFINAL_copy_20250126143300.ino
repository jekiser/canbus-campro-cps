#include <U8g2lib.h>
#include <esp32_can.h>
#include "logo.h"

#define CAN_ID 776 // ID pesan CAN yang ingin dibaca
#define CAN_ID_727 784 // ID can
#define CAN_ID_800 800 // ID can

#define CAN_BAUD_RATE 500000
#define UPDATE_INTERVAL 200 // 200ms
#define TIMEOUT_INTERVAL 60000 // 1 minute
#define MAX_CAN_IDS 50
int canIDs[MAX_CAN_IDS]; // Array to store CAN IDs
int canIDCount = 0; // Counter for stored CAN IDs

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int rpm = 0; 
int temp = 0;
int spd = 0;
int unk1 = 0;
int unk2 = 0;
int unk3 = 0;
int unk4 = 0;
uint8_t data727[8];

const char DEGREE_SYMBOL[] = { 0xB0, '\0' };
unsigned long lastUpdate = 0;
unsigned long lastScrollUpdate = 0;
unsigned long lastDataReceived = 0;


void setup() {
    Serial.begin(115200);
    u8g2.begin();
    displayLogo();

    // Inisialisasi CAN bus
    CAN0.setCANPins(GPIO_NUM_4, GPIO_NUM_5); // Config for shield v1.3+
    CAN0.begin(CAN_BAUD_RATE);
    CAN0.watchFor();
}

void displayLogo() {
    u8g2.clearBuffer();
    u8g2.drawXBMP(0, 0, 128, 64, PROTON_LOGO); // Menampilkan logo
    u8g2.sendBuffer();
    delay(3000); // Tampilkan logo selama 3 detik
}

void loop() {
    CAN_FRAME can_message;
    lastDataReceived = millis(); 

    if (CAN0.read(can_message)) {
      // Check if the CAN ID is already stored
        bool idExists = false;
        for (int i = 0; i < canIDCount; i++) {
            if (canIDs[i] == can_message.id) {
                idExists = true;
                break;
            }
        }
        if (!idExists && canIDCount < MAX_CAN_IDS) {
            canIDs[canIDCount++] = can_message.id;
        }
        if (can_message.id == CAN_ID) {
            int byte1 = can_message.data.byte[0]; // rpm
            int byte2 = can_message.data.byte[1]; // rpm
            int byte3 = can_message.data.byte[2]; // tps pedal
            int byte4 = can_message.data.byte[3]; // tps intake
            int byte5 = can_message.data.byte[4]; // temp
            int byte6 = can_message.data.byte[5]; // switch on off
            int byte7 = can_message.data.byte[6]; // rem 32
            int byte8 = can_message.data.byte[7]; // intake air temp

            rpm = roundRPM((byte1 * 256) + byte2);
            temp = byte5 - 40;
            spd = round((byte3 / 255.0) * 100);
            unk1 = round((byte4 / 255.0) * 100);
            unk2 = byte6;
            unk3 = byte7; // rem 32
            unk4 = byte8 - 40; 
        } 
        else if (can_message.id == CAN_ID_727) {
            for (int i = 0; i < 8; i++) {
                data727[i] = can_message.data.byte[i]; // Simpan semua byte
            }
        }
    }
    if (millis() - lastUpdate >= UPDATE_INTERVAL) {
        lastUpdate = millis();
        draw(); // Memperbarui tampilan
    }

    if (millis() - lastDataReceived >= TIMEOUT_INTERVAL) {
        u8g2.clearBuffer(); 
        u8g2.sendBuffer();
    }
    
}

int roundRPM(int value) {
    if (value > 3000) {
        return ((value + 5) / 10) * 10; // Round to nearest 10
    } 
    return value; // Return as is if >= 9000
}
String byteToBinary(uint8_t byte) {
    String binaryString = "";
    for (int i = 7; i >= 0; i--) {
        binaryString += (byte & (1 << i)) ? '1' : '0';
    }
    return binaryString;
}

void draw() {
    u8g2.clearBuffer();
    
    // Display temp
    u8g2.setFont(u8g2_font_spleen16x32_mf);
    int tempX = (temp >= 100) ? 66 : (temp >= 10) ? 82 : 98;
    u8g2.setCursor(tempX, 20);
    u8g2.print(temp);
    u8g2.setCursor(112, 21);
    u8g2.print(DEGREE_SYMBOL);

    // Display speed
    u8g2.setFont(u8g2_font_spleen16x32_mf);
    int spdX = (spd >= 100) ? 80 : (spd >= 10) ? 96 :  112 ;
    u8g2.setCursor(spdX, 40);
    u8g2.print(spd);
    
    // Display RPM
    u8g2.setCursor(0, 21);
    u8g2.print(rpm);
 
    // Display additional data
    u8g2.setFont(u8g2_font_spleen6x12_mn);
    u8g2.setCursor(0, 30);
    u8g2.print(unk1);
    u8g2.setCursor(25, 30);
    u8g2.print(unk2);
    u8g2.setCursor(0, 39);
    u8g2.print(unk3);
    u8g2.setCursor(25, 39);
    u8g2.print(unk4);
    u8g2.setCursor(0, 48);
    u8g2.print("unk3 (bin): ");
    u8g2.print(byteToBinary(unk3));
u8g2.setFont(u8g2_font_spleen5x8_mf);
u8g2.setCursor(0, 64);
for (int i = 0; i < canIDCount; i++) {
        u8g2.print(canIDs[i], HEX); // Display in hexadecimal format
        if (i < canIDCount - 1) {
            u8g2.print(", "); // Print comma and space if not the last ID
        }
    }
    u8g2.sendBuffer();
}