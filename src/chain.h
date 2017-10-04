/*****************************************************************************
 * chain.h
 *
 * Copyright (C) 2015 VideoLAN
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

/*!
 * \file <chain.h>
 * \author Jean-Paul Saman <jpsaman@videolan.org>
 * \brief Chain PSI table decoders for demuxing based on table_id and extension.
 *
 * Chain PSI table decoders for demuxing based on table_id and extension. The
 * decoder chain is kept inside the dvbpsi_t handle at the dvbpsi_t::dvbpsi_decoder_t
 * member variable.
 *
 * @note: The use of demux.h API's has been discontinued and replaced by the API's in
 * this description. The table below shows how a mapping from old to new API's:
 *
 * -----------------------------------------------------------------------------------
 * - Old demux API from demux.h             | is replaced by chain.h                 -
 * -----------------------------------------------------------------------------------
 * - dvbpsi_AttachDemux                     | @see dvbpsi_demux_chain_new            -
 * - dvbpsi_DetachDemux                     | @see dvbpsi_demux_chain_delete         -
 * - dvbpsi_Demux                           | no replacement                         -
 * - dvbpsi_demuxGetSubDecoder              | @see dvbpsi_decoder_chain_get          -
 * - dvbpsi_NewDemuxSubDecoder              | no replacement                         -
 * - dvbpsi_AttachDemuxSubDecoder           | @see dvbpsi_decoder_chain_add          -
 * - dvbspi_DetachDemuxSubDecoder           | @see dvbpsi_decoder_chain_remove       -
 * -----------------------------------------------------------------------------------
 *
 * Note that for dvbpsi_Demux(), dvbpsi_NewDemuxSubDecoder(), dvbpsi_AttachDemuxSubDecoder(),
 * and dvbspi_DetachDemuxSubDecoder() no replacement API is available. These functions are
 * discontinued since the dvbpsi_subdec_t indirection has been removed. A dvbpsi_decoder_t
 * can be added in a demux chain directly using dvbpsi_decoder_chain_add() or removed using
 * dvbpsi_decoder_chain_remove() API.
 * It is IMPORTANT to use the API call dvbpsi_demux_chain_new(), since that will install
 * the callback function dvbpsi_decoder_chain_demux(). This will do the same job dvbpsi_Demux()
 * function had in the subsdecoder architecture of demux.h.
 */

#ifndef _DVBPSI_CHAIN_H_
#define _DVBPSI_CHAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * dvbpsi_chain_demux_new
 *****************************************************************************/
/*!
 * \fn bool dvbpsi_chain_demux_new(dvbpsi_t *p_dvbpsi, dvbpsi_callback_new_t pf_new,
                              dvbpsi_callback_del_t pf_del, void *p_data)
 * \brief dvbpsi_chain_demux_new creates a decoder for demuxing PSI tables and subtables.
 * \param p_dvbpsi pointer to dvbpsi_t handle
 * \param pf_new callback function for calling the specific PSI table/subtable attach function
 * \param pf_del callback function for calling the specific PSI table/subtable detach function
 * \param p_data pointer to data that must be passed to PSI table/subtable its
 *        dvbpsi_xxx_attach() function
 * \return true on success, false on failure
 */
bool dvbpsi_chain_demux_new(dvbpsi_t *p_dvbpsi, dvbpsi_callback_new_t pf_new,
                              dvbpsi_callback_del_t pf_del, void *p_data);

/*****************************************************************************
 * dvbpsi_chain_demux_delete
 *****************************************************************************/
/*!
 * \fn bool dvbpsi_chain_demux_delete(dvbpsi_t *p_dvbpsi)
 * \brief dvbpsi_chain_demux_delete walks the chain of PSI (sub-)table decoders
 * and removes them from the chain before calling its pf_detach callback. The pointers
 * to the PSI (sub-)table decoders are no longer valid after this function has been called.
 * Nor is the decoder chain list valid.
 * \param p_dvbpsi pointer to dvbpsi_t handle
 * \return true on success, false on failure
 */
bool dvbpsi_chain_demux_delete(dvbpsi_t *p_dvbpsi);

/*****************************************************************************
 * dvbpsi_decoder_chain_add
 *****************************************************************************/
/*!
 * \fn bool dvbpsi_decoder_chain_add(dvbpsi_t *p_dvbpsi, dvbpsi_decoder_t *p_decoder)
 * \brief Adds decoder to the chain in handle 'p_dvbpsi' at dvbpsi_t::dvbpsi_decoder_t
 * \param p_dvbpsi pointer to dvbpsi_t handle
 * \param p_decoder pointer to dvbpsi_decoder_t for adding to chain
 * \return true on success, false on failure
 */
bool dvbpsi_decoder_chain_add(dvbpsi_t *p_dvbpsi, dvbpsi_decoder_t *p_decoder);

/*****************************************************************************
 * dvbpsi_decoder_chain_remove
 *****************************************************************************/
/*!
 * \fn bool dvbpsi_decoder_chain_remove(dvbpsi_t *p_dvbpsi, const dvbpsi_decoder_t *p_decoder)
 * \brief Deletes decoder from the chain in handle 'p_dvbpsi' at dvbpsi_t::dvbpsi_decoder_t
 * \note Use @see dvbpsi_decoder_chain_get() to find the decoder pointer, then call
 * dvbpsi_decoder_chain_remove() to remove the pointer from the chain. The caller is
 * responsible for freeing the associated memory of the just removed decoder pointer
 * and needs to call decoder_delete(p_decoder).
 * \param p_dvbpsi pointer to dvbpsi_t handle
 * \param p_decoder pointer to dvbpsi_decoder_t for deletion from chain
 * \return true on success, false on failure
 */
bool dvbpsi_decoder_chain_remove(dvbpsi_t *p_dvbpsi, const dvbpsi_decoder_t *p_decoder);

/*****************************************************************************
 * dvbpsi_decoder_chain_get
 *****************************************************************************/
/*!
 * \fn dvbpsi_decoder_t *dvbpsi_decoder_chain_get(dvbpsi_t *p_dvbpsi, const uint16_t table_id, const uint16_t extension)
 * \brief Gets decoder from the chain based on given table_id and extension.
 * If extension is 0, then the search is performed on table_id only and the first match is returned.
 * \param p_dvbpsi pointer to dvbpsi_t handle
 * \param table_id PSI table id to get
 * \param extension PSI subtable id to get
 * \return returns pointer to dvbpsi_decoder_t on success, or NULL on failure
 */
dvbpsi_decoder_t *dvbpsi_decoder_chain_get(dvbpsi_t *p_dvbpsi, const uint16_t table_id, const uint16_t extension);

#ifdef __cplusplus
};
#endif

#else
#error "Multiple inclusions of chain.h"
#endif
