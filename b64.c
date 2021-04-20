/*
  base64 functions for turning binary data into strings and back again.
  Created April 2021 - David Horton and released to public domain.
  
  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/

#include <b64.h>
#include <Arduino.h>

// b64map - 6-bit index selects the correct character from the base64 
// 'alphabet' as described in RFC4648. Also used for decoding functions.
const char b64map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// b64pad - padding character, also described in RFC4648.
const char b64pad = '=';

size_t b64enclen(size_t unenc_len) {
  size_t enc_len;

  // Padding the unencoded length by 2 for worst case makes calculating
  // easier. If it turns out padding is not needed, integer division will
  // drop the fractional part. The result is the correct length without any
  // of the hassle dealing with padding.
  unenc_len += 2;
  
  // Encoded is four-thirds the unencoded length. Add 1 for NULL terminator.
  enc_len = (unenc_len / 3 * 4) + 1;

  return enc_len;
}

int b64enc(char *unenc, char *enc, size_t unenc_len) {
  unsigned char buffer[4];  // Temp storage for mapping three bytes to four characters.

  // Any input not evenly divisible by three requires padding at the end.
  // Determining what remainder exists after dividing by three helps when
  // dealing with those special cases. 
  int remainder = unenc_len %3;
  
  // Loop through unencoded characters in sets of three at a time. Any one or
  // two characters remaining are dealt with at the end to properly determine
  // their padding.
  int i = 0;
  int j = 0;
  for (i=0; i<unenc_len - remainder; i+=3) {  // Minus padding remainder.

    // Take three bytes of eight bits and map onto four chars of six bits.
    // E.g. ABCDEFGH IJKLMNOP QRSTUVWX => 00ABCDEF 00GHIJKL 00MNOPQR 00STUVWX
    buffer[0] = unenc[i] >> 2;
    buffer[1] = (unenc[i] & 0B00000011) << 4 | unenc[i+1] >> 4;
    buffer[2] = (unenc[i+1] & 0B00001111) << 2 | unenc[i+2] >> 6;
    buffer[3] = unenc[i+2] & 0B00111111;

    // Map the six-bit bytes onto the ASCII characters used in base64.
    enc[j++] = b64map[buffer[0]];
    enc[j++] = b64map[buffer[1]];
    enc[j++] = b64map[buffer[2]];
    enc[j++] = b64map[buffer[3]];
  }

  // The remaining characters are handled differently, because there could
  // be padding. The ammount of padding depends upon if there are one or two
  // characters left over.
  switch (remainder) {
    case 2:
      buffer[0] = unenc[i] >> 2;
      buffer[1] = (unenc[i] & 0B00000011) << 4 | unenc[i+1] >> 4;
      buffer[2] = (unenc[i+1] & 0B00001111) << 2 | unenc[i+2] >> 6;
      enc[j++] = b64map[buffer[0]];
      enc[j++] = b64map[buffer[1]];
      enc[j++] = b64map[buffer[2]];
      enc[j++] = b64pad;
      break;
    case 1:
      buffer[0] = unenc[i] >> 2;
      buffer[1] = (unenc[i] & 0B00000011) << 4;
      enc[j++] = b64map[buffer[0]];
      enc[j++] = b64map[buffer[1]];
      enc[j++] = b64pad;
      enc[j++] = b64pad;
      break;
  }

  // Finish with a NULL terminator since the encoded result is a string.
  enc[j] = '\0';
 
  return j;
}

size_t b64declen(char * enc, size_t enc_len) {
  size_t dec_len;
  
  // Any C-style string not ending with a NULL timinator is invalid.
  // Rememeber to subtract one from the length due to zero indexing.
  if (enc[enc_len - 1] != '\0') return 0;
  
  // Even a single byte encoded to base64 results in a for character
  // string (two chars, two padding.) Anything less is invalid.
  if (enc_len < 4) return 0;

  // Padded base64 string lengths are always divisible by four (after
  // subtracting the NULL terminator) Otherwise, they're not vaild.
  if ((enc_len - 1) %4 != 0) return 0;

  // Maximum decoded length is three-fourths the encoded length.
  dec_len = ((enc_len - 1) / 4 * 3);

  // Padding characters don't count for decoded length.
  if (enc[enc_len - 2] == b64pad) dec_len--;
  if (enc[enc_len - 3] == b64pad) dec_len--;

  return dec_len;
}

int b64dec(char *enc, char *dec, size_t enc_len) {
  unsigned char buffer[4];  // Temp storage for mapping three bytes to four characters.

  // base64 encoded input should always be evenly divisible by four, due to
  // padding characters. If not, it's an error. Note: because base64 is held
  // in a C-style string, there's the NULL terminator to subtract first.
  if ((enc_len - 1) %4 != 0) return 0;

  int padded = 0;
  if (enc[enc_len - 2] == b64pad) padded++;
  if (enc[enc_len - 3] == b64pad) padded++;

  // Loop through encoded characters in sets of four at a time, because there
  // are four encoded characters for every three decoded characters. But, if
  // its not evenly divisible by four leave the remaining as a special case.
  int i = 0;
  int j = 0;

  //Serial.print("Looping until i < ");
  //Serial.println(enc_len - padded - 4);

  for (i=0; i<enc_len - padded - 4; i+=4) {

    //Serial.println(i);

    // Take four chars of six bits and map onto four bytes of eight bits.
    // E.g. 00ABCDEF 00GHIJKL 00MNOPQR 00STUVWX => ABCDEFGH IJKLMNOP QRSTUVWX
    buffer[0] = strchr(b64map, enc[i]) - b64map;
    buffer[1] = strchr(b64map, enc[i+1]) - b64map;
    buffer[2] = strchr(b64map, enc[i+2]) - b64map;
    buffer[3] = strchr(b64map, enc[i+3]) - b64map;
    dec[j++] = buffer[0] << 2 | buffer[1] >> 4;
    dec[j++] = buffer[1] << 4 | buffer[2] >> 2;
    dec[j++] = buffer[2] << 6 | buffer[3];
  }

  // Take care of special case.
  switch (padded) {
    case 1:
      buffer[0] = strchr(b64map, enc[i]) - b64map;
      buffer[1] = strchr(b64map, enc[i+1]) - b64map;
      buffer[2] = strchr(b64map, enc[i+2]) - b64map;
      dec[j++] = buffer[0] << 2 | buffer[1] >> 4;
      dec[j++] = buffer[1] << 4 | buffer[2] >> 2;
      break;
    case 2:
      buffer[0] = strchr(b64map, enc[i]) - b64map;
      buffer[1] = strchr(b64map, enc[i+1]) - b64map;
      dec[j++] = buffer[0] << 2 | buffer[1] >> 4;
      break;
  }
  
  return j;
}
