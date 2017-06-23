/*****************************************************************************
 * demux.c: DVB subtables demux functions.
 *----------------------------------------------------------------------------
 * Copyright (C) 2001-2010 VideoLAN
 * $Id$
 *
 * Authors: Johan Bilien <jobi@via.ecp.fr>
 *          Jean-Paul Saman <jpsaman@videolan.org>
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
 *----------------------------------------------------------------------------
 *
 *****************************************************************************/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#include <assert.h>

#include "dvbpsi.h"
#include "dvbpsi_private.h"
#include "psi.h"
#include "demux.h"
#include "chain.h"

/*****************************************************************************
 * dvbpsi_AttachDemux
 *****************************************************************************
 * Creation of the demux structure
 *****************************************************************************/
bool dvbpsi_AttachDemux(dvbpsi_t *            p_dvbpsi,
                        dvbpsi_demux_new_cb_t pf_new_cb,
                        void *                p_new_cb_data)
{
    return dvbpsi_chain_demux_new(p_dvbpsi,
                                  (dvbpsi_callback_new_t)pf_new_cb,
                                  NULL, p_new_cb_data);
}

/*****************************************************************************
 * dvbpsi_demuxGetSubDec
 *****************************************************************************
 * Finds a subtable decoder given the table id and extension
 *****************************************************************************/
dvbpsi_demux_subdec_t *dvbpsi_demuxGetSubDec(dvbpsi_demux_t * p_demux,
                                              uint8_t i_table_id,
                                              uint16_t i_extension)
{
    dvbpsi_t *p_dvbpsi = (dvbpsi_t *)p_demux;
    return (dvbpsi_demux_subdec_t *)dvbpsi_decoder_chain_get(p_dvbpsi, i_table_id, i_extension);
}

/*****************************************************************************
 * dvbpsi_Demux
 *****************************************************************************
 * Sends a PSI section to the right subtable decoder
 *****************************************************************************/
void dvbpsi_Demux(dvbpsi_t *p_dvbpsi, dvbpsi_psi_section_t *p_section)
{
    assert(p_dvbpsi);
    assert(p_dvbpsi->p_decoder);

    dvbpsi_decoder_t *p_demux = (dvbpsi_decoder_t *)p_dvbpsi->p_decoder;
    if (p_demux && p_demux->pf_gather)
        p_demux->pf_gather(p_dvbpsi, p_section);
}

/*****************************************************************************
 * dvbpsi_DetachDemux
 *****************************************************************************
 * Destroys a demux structure
 *****************************************************************************/
void dvbpsi_DetachDemux(dvbpsi_t *p_dvbpsi)
{
    assert(p_dvbpsi);
    assert(p_dvbpsi->p_decoder);

    if (!dvbpsi_chain_demux_delete(p_dvbpsi))
        dvbpsi_error(p_dvbpsi, "demux", "could not free chain_demux decoder resources");
}

/*****************************************************************************
 * dvbpsi_NewDemuxSubDecoder
 *****************************************************************************
 * Allocate a new demux sub table decoder
 *****************************************************************************/
dvbpsi_demux_subdec_t *dvbpsi_NewDemuxSubDecoder(const uint8_t i_table_id,
                                                 const uint16_t i_extension,
                                                 dvbpsi_demux_detach_cb_t pf_detach,
                                                 dvbpsi_demux_gather_cb_t pf_gather,
                                                 dvbpsi_decoder_t *p_decoder)
{
    assert(0);
    return NULL;
}

/*****************************************************************************
 * dvbpsi_DeleteDemuxSubDecoder
 *****************************************************************************
 * Free a demux sub table decoder
 *****************************************************************************/
void dvbpsi_DeleteDemuxSubDecoder(dvbpsi_demux_subdec_t *p_subdec)
{
    assert(0);
}

/*****************************************************************************
 * dvbpsi_AttachDemuxSubDecoder
 *****************************************************************************/
void dvbpsi_AttachDemuxSubDecoder(dvbpsi_demux_t *p_demux, dvbpsi_demux_subdec_t *p_subdec)
{
    assert(0);
}

/*****************************************************************************
 * dvbpsi_DetachDemuxSubDecoder
 *****************************************************************************/
void dvbpsi_DetachDemuxSubDecoder(dvbpsi_demux_t *p_demux, dvbpsi_demux_subdec_t *p_subdec)
{
    assert(0);
}
