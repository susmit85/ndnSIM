/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2011 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Ilya Moiseenko <iliamo@cs.ucla.edu>
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "ccn.h"
#include "ccn_charbuf.h"
#include "ccn_coding.h"
#include "ccn_indexbuf.h"

/**
 * Encode and sign a ContentObject.
 * @param buf is the output buffer where encoded object is written.
 * @param Name is the ccnb-encoded name from ccn_name_init and friends.
 * @param SignedInfo is the ccnb-encoded info from ccn_signed_info_create.
 * @param data pintes to the raw data to be encoded.
 * @param size is the size, in bytes, of the raw data to be encoded.
 * @param digest_algorithm may be NULL for default.
 * @param private_key is the private key to use for signing.
 * @returns 0 for success or -1 for error.
 */
int
ccn_encode_ContentObject(struct ccn_charbuf *buf,
                         const struct ccn_charbuf *Name,
                         /*const struct ccn_charbuf *SignedInfo,*/
                         const void *data,
                         size_t size
                         /*const char *digest_algorithm,
                         const struct ccn_pkey *private_key*/
                         )
{
    int res = 0;
    
    res |= ccn_charbuf_append_tt(buf, CCN_DTAG_ContentObject, CCN_DTAG);
    //res |= ccn_encode_Signature(buf, digest_algorithm,
    //                            NULL, 0, signature, signature_size);
    res |= ccn_charbuf_append_charbuf(buf, Name);
    //res |= ccn_charbuf_append_charbuf(buf, SignedInfo);
    res |= ccnb_append_tagged_blob(buf, CCN_DTAG_Content, data, size);
    res |= ccn_charbuf_append_closer(buf);
    //free(signature);
    ccn_charbuf_destroy(&content_header);
    return(res == 0 ? 0 : -1);
}

/**
 * Append a tagged BLOB
 *
 * This is a ccnb-encoded element with containing the BLOB as content
 * @param c is the buffer to append to.
 * @param dtag is the element's dtab
 * @param data points to the binary data
 * @param size is the size of the data, in bytes
 * @returns 0 for success or -1 for error.
 */
int
ccnb_append_tagged_blob(struct ccn_charbuf *c,
                        enum ccn_dtag dtag,
                        const void *data,
                        size_t size)
{
    int res;
    
    res = ccn_charbuf_append_tt(c, dtag, CCN_DTAG);
    if (size != 0) {
        res |= ccn_charbuf_append_tt(c, size, CCN_BLOB);
        res |= ccn_charbuf_append(c, data, size);
    }
    res |= ccn_charbuf_append_closer(c);
    return(res == 0 ? 0 : -1);
}