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

#ifndef b64.h
#define b64.h

#include <stddef.h>
#include <string.h>

/*
 * b64enclen
 *   Given number of binary bytes, calculate the number of characters needed
 *   to represent the data as base64. Include room for null terminator, since
 *   the results of encoding will be a C-style string.
 * Parameters:
 *   unenc_len - the length of the unencoded array of characters. A simple
 *    'sizeof unencoded' will work.
 * Returns:
 *   size_t number or characters required for base64 encoded message plus a
 *   NULL terminator. Suitable for array declarations such as:
 *   'char output[b64enclen(sizeof unencoded)]'
 */
size_t b64enclen(size_t unenc_len);

/*
 * b64enc
 *   Given an unencoded array of bytes (binary data) and its length, fill
 *   the encoded array with a base64 representation of the input. b64enclen()
 *   should be used to properly size the character array that will hold the
 *   encoded output.
 * Parameters:
 *   unenc - pointer to a character array with the contents to be encoded.
 *   enc - pointer to a byte array that will be filled with base64 output.
 *   unenc_len - length of the string pointed to by unenc. Can be found with
 *     'sizeof unenc'
 * Returns:
 *   Integer representing the number of bytes encoded.
 */
int b64enc(char *unenc, char *enc, size_t unenc_len);

/*
 * b64declen
 *   Given a base64 encoded string and its length, perform a number of 
 *   tests to validate the string. Then, calculate the number of bytes
 *   needed to represent the binary data after decoding.
 * Parameters:
 *   enc - a pointer to the base64 encoded string.
 *   enc_len - the length of the encoded string (i.e. 'sizeof enc'.)
 * Returns:
 *   size_t number of characters required for the decoded message or
 *   0 in the case of an invalid base64 encoded string.
 */
size_t b64declen(char * enc, size_t enc_len);

/*
 * b64dec
 *   Given a base64 encoded string and its length, fill the decoded array
 *   with the decoded binary representation of the input. b64declen() should
 *   be used to properly size the array intended to hold the encoded output.
 */
int b64dec(char *enc, char *dec, size_t enc_len);

#endif
