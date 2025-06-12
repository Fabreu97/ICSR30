/*****************************************************************************
 * This file contains functions shared between server and client.
 *****************************************************************************
 * Author:  Fernando Abreu
 * Date:    2025-05-13
 *****************************************************************************
 */

#ifndef __INCLUDED_UTILS_H__
#define __INCLUDED_UTILS_H__

/*****************************************************************************
 * LIBRARY
 *****************************************************************************/
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>

/*****************************************************************************
 * MACROS 
 *****************************************************************************/
#define INITIAL_TIMEOUT_SEC     1 // 1 second
#define INITIAL_TIMEOUT_USEC    0
#define ALPHA 0.125

/*****************************************************************************
 * FUNCTIONS
 *****************************************************************************/

void update_timeout(struct timeval* timeout, struct timeval start, struct timeval end);
std::string sha256_to_hex(const unsigned char* hash, unsigned int hash_len);
std::string get_file_sha256(const std::string& filePath);
 #endif // __INCLUDED_UTILS_H__