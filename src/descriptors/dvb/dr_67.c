/*****************************************************************************
 * dr_67.c
 * Copyright (C) 2016 VideoLAN
 * $Id$
 *
 * Authors: Jean-Paul Saman <jpsaman@videolan.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#include "../../dvbpsi.h"
#include "../../dvbpsi_private.h"
#include "../../descriptor.h"

#include "dr_67.h"

/*****************************************************************************
 * dvbpsi_decode_dvb_transport_stream_dr
 *****************************************************************************/
dvbpsi_dvb_transport_stream_dr_t *dvbpsi_decode_dvb_transport_stream_dr(dvbpsi_descriptor_t *p_descriptor)
{
    dvbpsi_dvb_transport_stream_dr_t *p_decoded;

    /* Check the tag */
    if (!dvbpsi_CanDecodeAsDescriptor(p_descriptor, 0x67))
        return NULL;

    /* Don't decode twice */
    if (dvbpsi_IsDescriptorDecoded(p_descriptor))
        return p_descriptor->p_decoded;

    /* Check the length */
    if (p_descriptor->i_length != 0x03)
        return NULL;

    /* three values that shall carry: 0x44, 0x56, 0x42 (ASCI "DVB") */
    if ((p_descriptor->p_data[0] != 0x44) ||
        (p_descriptor->p_data[1] != 0x56) ||
        (p_descriptor->p_data[2] != 0x42))
        return NULL;

    p_decoded = (dvbpsi_dvb_transport_stream_dr_t *)
                    calloc(1, sizeof(dvbpsi_dvb_transport_stream_dr_t));
    if (!p_decoded)
        return NULL;

    /* three values that shall carry: 0x44, 0x56, 0x42 (ASCI "DVB") */
    p_decoded->p_data[0] = p_descriptor->p_data[0];
    p_decoded->p_data[1] = p_descriptor->p_data[1];
    p_decoded->p_data[2] = p_descriptor->p_data[2];

    p_descriptor->p_decoded = (void*)p_decoded;
    return p_decoded;
}

/*****************************************************************************
 * dvbpsi_gen_dvb_transport_stream_dr
 *****************************************************************************/
dvbpsi_descriptor_t *dvbpsi_gen_dvb_transport_stream_dr(dvbpsi_dvb_transport_stream_dr_t *p_decoded,
                                      bool b_duplicate)
{
    /* three values that shall carry: 0x44, 0x56, 0x42 (ASCI "DVB") */
    if ((p_decoded->p_data[0] != 0x44) ||
        (p_decoded->p_data[1] != 0x56) ||
        (p_decoded->p_data[2] != 0x42))
        return NULL;

    /* Create the descriptor */
    dvbpsi_descriptor_t * p_descriptor = dvbpsi_NewDescriptor(0x67, 3, NULL);
    if (!p_descriptor)
        return NULL;

    /* Encode data */
    p_descriptor->p_data[0] = p_decoded->p_data[0];
    p_descriptor->p_data[1] = p_decoded->p_data[1];
    p_descriptor->p_data[2] = p_decoded->p_data[2];

    if (b_duplicate)
    {
        /* Duplicate decoded data */
        p_descriptor->p_decoded =
                dvbpsi_DuplicateDecodedDescriptor(p_decoded,
                                                  sizeof(dvbpsi_dvb_transport_stream_dr_t));
    }

    return p_descriptor;
}
