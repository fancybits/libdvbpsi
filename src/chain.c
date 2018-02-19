/*****************************************************************************
 * chain.c: DVB subtables decoder chain functions
 *----------------------------------------------------------------------------
 * Copyright (C) 2015 VideoLAN
 * $Id: $
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
#include "chain.h"

/*****************************************************************************
 * dvbpsi_decoder_chain_demux
 *****************************************************************************
 * Sends a PSI section to the right subtable decoder
 *****************************************************************************/
static void dvbpsi_decoder_chain_demux(dvbpsi_t *p_dvbpsi, dvbpsi_psi_section_t *p_section)
{
    assert(p_dvbpsi);
    assert(p_dvbpsi->p_decoder);

    dvbpsi_decoder_t *p_demux = (dvbpsi_decoder_t *)p_dvbpsi->p_decoder;
    if (p_demux == NULL) {
        dvbpsi_DeletePSISections(p_section);
        return;
    }

    dvbpsi_decoder_t *p_table = NULL;
    p_table = dvbpsi_decoder_chain_get(p_dvbpsi, p_section->i_table_id, p_section->i_extension);
    if (p_table == NULL) {
        if (p_demux->pf_new)
            p_demux->pf_new(p_dvbpsi, p_section->i_table_id, p_section->i_extension, p_demux->p_priv);
    }

    /* Check if new table created */
    p_table = dvbpsi_decoder_chain_get(p_dvbpsi, p_section->i_table_id, p_section->i_extension);
    if (p_table)
       p_table->pf_gather(p_dvbpsi, p_section);
    else
       dvbpsi_DeletePSISections(p_section);
}

/*****************************************************************************
 * dvbpsi_chain_demux_new
 *****************************************************************************
 * Create chain demux decoder
 *****************************************************************************/
bool dvbpsi_chain_demux_new(dvbpsi_t *p_dvbpsi, dvbpsi_callback_new_t pf_new,
                              dvbpsi_callback_del_t pf_del, void *p_data)
{
    assert(p_dvbpsi);

    if (p_dvbpsi->p_decoder) {
        dvbpsi_error(p_dvbpsi, "chain", "cannot initialize decoder chain");
        return false;
    }

    dvbpsi_decoder_t *p_chain = (dvbpsi_decoder_t *)dvbpsi_decoder_new(&dvbpsi_decoder_chain_demux,
                                                                      4096, true,
                                                                      sizeof(dvbpsi_decoder_t));
    if (!p_chain)
        return false;

    p_chain->pf_new = pf_new;
    p_chain->pf_del = pf_del;
    p_chain->p_priv = p_data;

    /* Remomber decoder */
    p_dvbpsi->p_decoder = DVBPSI_DECODER(p_chain);
    return true;
}

/*****************************************************************************
 * dvbpsi_chain_demux_delete
 *****************************************************************************
 * Remove all decoders from list and free all resources.
 *****************************************************************************/
bool dvbpsi_chain_demux_delete(dvbpsi_t *p_dvbpsi)
{
    if (!p_dvbpsi) return false;

    dvbpsi_decoder_t *p_demux = p_dvbpsi->p_decoder;
    if (!p_demux) return false;

    /*
     * Get first subtable decoder other then the one added in
     * dvbpsi_chain_demux_new(). It is deleted as last step
     * in this function.
     */
    dvbpsi_decoder_t *p = p_demux->p_next;
    if (!p) goto out;

    /* Walk linked list */
    /* FIXME: p->pf_del() calls eventually the dvbpsi_XXX_detach() function
     * which walks the list again. This is a waste of time and needs improvement
     * on the mechanism on how to create/delete and attach/detach a subtable decoder.
     */
    dvbpsi_decoder_t *p_next;
    while (p) {
        p_next = p->p_next;
        if (p_demux->pf_del)
            p_demux->pf_del(p_dvbpsi, p->i_table_id, p->i_extension);
        else {
            /*
             * Delete decoder even when there is no pf_del() callback,
             * otherwise memory is leaked.
             */
            dvbpsi_decoder_delete(p);
        }
        p = p_next;
    }

out:
    /* Delete demux decoder */
    dvbpsi_decoder_delete(p_demux);
    p_dvbpsi->p_decoder = NULL;
    return true;
}

/*****************************************************************************
 * dvbpsi_decoder_chain_add
 *****************************************************************************
 * Add decoder to chain
 *****************************************************************************/
bool dvbpsi_decoder_chain_add(dvbpsi_t *p_dvbpsi, dvbpsi_decoder_t *p_decoder)
{
    assert(p_decoder);
    assert(!p_decoder->p_next);

    dvbpsi_decoder_t *p_list = (dvbpsi_decoder_t *) p_dvbpsi->p_decoder;
    while (p_list) {
        if (p_decoder->i_table_id == p_list->i_table_id) {
            if (p_decoder->i_extension == p_list->i_extension) {
                dvbpsi_error(p_dvbpsi, "chain", "decoder (%d:%d) already present",
                             p_decoder->i_table_id, p_decoder->i_extension);
                return false;
            }
            /* insert in reverse sequential order based on i_extension */
            if (p_decoder->i_extension > p_list->i_extension) {
                p_decoder->p_next = p_list->p_next;
                p_list->p_next = p_decoder;
                return true;
            }
        }
        /* append to end of list */
        if (p_list->p_next == NULL) {
            p_list->p_next = p_decoder;
            return true;
        }
        /* insert in sequential order based on i_table_id */
        if (p_decoder->i_table_id < p_list->p_next->i_table_id) {
            p_decoder->p_next = p_list->p_next;
            p_list->p_next = p_decoder;
            return true;
        }
        p_list = p_list->p_next;
    }
    /* empty list */
    p_dvbpsi->p_decoder = p_decoder;
    return true;
}

/*****************************************************************************
 * dvbpsi_decoder_chain_remove
 *****************************************************************************
 * Remove decoder from chain
*****************************************************************************/
bool dvbpsi_decoder_chain_remove(dvbpsi_t *p_dvbpsi, const dvbpsi_decoder_t *p_decoder)
{
    if (!p_decoder) return false;

    dvbpsi_decoder_t **pp_prev = &p_dvbpsi->p_decoder;
    while (pp_prev) {
        if ((p_decoder->i_table_id == (*pp_prev)->i_table_id) &&
            (p_decoder->i_extension == (*pp_prev)->i_extension)) {
            assert(*pp_prev == p_decoder);
            *pp_prev = p_decoder->p_next;
            /* NOTE: caller must call dvbpsi_decoder_delete(p_decoder) */
            return true;
        }
        pp_prev = &(*pp_prev)->p_next;
    }

    dvbpsi_warning(p_dvbpsi, "chain", "decoder (table id: %u, extension: %u) not found",
                   p_decoder->i_table_id, p_decoder->i_extension);
    return false;
}

/*****************************************************************************
 * dvbpsi_decoder_chain_get
 *****************************************************************************
 * Return pointer to decoder in chain
 *****************************************************************************/
dvbpsi_decoder_t *dvbpsi_decoder_chain_get(dvbpsi_t *p_dvbpsi, const uint16_t table_id, const uint16_t extension)
{
    dvbpsi_decoder_t *p = (dvbpsi_decoder_t *)p_dvbpsi->p_decoder;
    while (p) {
        if ((table_id == p->i_table_id) &&
            (extension == p->i_extension))
            return p;
        p = p->p_next;
    }

    return NULL;
}
