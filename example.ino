/*
 * Sample Arduino sketch to encode and decode messages.
 */

#include <b64.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Encoding example.
  char msg[] = "Hello Cleveland!";
  char enc_result[b64enclen(sizeof msg)];
  b64enc(msg, enc_result, sizeof msg);
  Serial.println(msg);
  Serial.println("...is encoded as...");
  Serial.println(enc_result);
  Serial.println();

  // Decoding example.
  char enc_msg[] = "SGVsbG8gQ2xldmVsYW5kIQA=";
  char dec_result[b64declen(enc_msg, sizeof enc_msg)];
  int declen = b64dec(enc_msg, dec_result, sizeof enc_msg);
  Serial.print("Bytes decoded: ");
  Serial.println(declen, DEC);
  for (int k=0; k<declen; k++) {
    Serial.print(dec_result[k], DEC);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Decoded message: ");
  Serial.println(dec_result);
}
  
void loop() {
  
}