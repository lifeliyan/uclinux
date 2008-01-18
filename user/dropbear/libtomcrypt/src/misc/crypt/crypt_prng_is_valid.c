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
  @file crypt_prng_is_valid.c
  Determine if PRNG is valid, Tom St Denis
*/

/*
   Test if a PRNG index is valid
   @param idx   The index of the PRNG to search for
   @return CRYPT_OK if valid
*/
int prng_is_valid(int idx)
{
   LTC_MUTEX_LOCK(&ltc_prng_mutex);
   if (idx < 0 || idx >= TAB_SIZE || prng_descriptor[idx].name == NULL) {
      LTC_MUTEX_UNLOCK(&ltc_prng_mutex);
      return CRYPT_INVALID_PRNG;
   }
   LTC_MUTEX_UNLOCK(&ltc_prng_mutex);
   return CRYPT_OK;
}

/* $Source: /cvs/libtom/libtomcrypt/src/misc/crypt/crypt_prng_is_valid.c,v $ */
/* $Revision: 5081 $ */
/* $Date: 2007-03-16 22:47:36 +0100 (Fri, 16 Mar 2007) $ */
