#include <SPI.h>
#include <Adafruit_PN532.h>
// ########   SPI for PN532:
// #define PN532_SCK (18)
// #define PN532_MISO (19)
// #define PN532_MOSI (23)
#define PN532_SS (4) //
int MemBlock = 9;
Adafruit_PN532 nfc(PN532_SS);

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  SPI.begin();
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.print("PN532 not found");
    while (1)
      ;
  }
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.SAMConfig();
}

void loop()
{
  uint8_t data[] = {
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x0C,
      0x31,
      0x42,
      0x01,
  };

  uint8_t dataLen = 16;

  // Try to read the card's UID
  uint8_t uidLength;
  uint8_t uidBuffer[7];
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uidBuffer, &uidLength))
  {
    Serial.print("Found card with UID: ");
    for (uint8_t i = 0; i < uidLength; i++)
    {
      Serial.print(uidBuffer[i] < 0x10 ? " 0" : " ");
      Serial.print(uidBuffer[i], HEX);
    }
    Serial.println();

    // Authenticate with the card using the default key A
    uint8_t key[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    if (nfc.mifareclassic_AuthenticateBlock(uidBuffer, uidLength, MemBlock, 0, key))
    {
      Serial.println("Authenticated");

      // Write the data to the card
      if (nfc.mifareclassic_WriteDataBlock(MemBlock, data))
      {
        Serial.print("Data written to card: ");
        for (uint8_t i = 0; i < dataLen; i++)
        {
          Serial.print(data[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
      }
      else
      {
        Serial.println("Write failed");
      }
    }
    else
    {
      Serial.println("Authentication failed");
    }
  }
  else
  {
    Serial.println("Card not found");
  }

  delay(5000);
}