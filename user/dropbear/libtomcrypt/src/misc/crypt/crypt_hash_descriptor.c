/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtomcrypt.com
 */
#include "tomcrypt.h"

/**
  @file crypt_hash_descriptor.c
  Stores the hash descriptor table, Tom St Denis  
*/

struct ltc_hash_descriptor hash_descriptor[TAB_SIZE] = {
{ NULL, 0, 0, 0, { 0 }, 0, NULL, NULL, NULL, NULL, NULL }
};

LTC_MUTEX_GLOBAL(ltc_hash_mutex)


/* $Source: /cvs/libtom/libtomcrypt/src/misc/crypt/crypt_hash_descriptor.c,v $ */
/* $Revision: 5081 $ */
/* $Date: 2007-03-16 22:47:36 +0100 (Fri, 16 Mar 2007) $ */
