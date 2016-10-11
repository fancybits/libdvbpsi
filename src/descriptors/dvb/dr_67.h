/*****************************************************************************
 * dr_67.h
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

/*!
 * \file dr_67.h
 * \author Jean-Paul Saman
 * \brief Transport stream descriptor
 */

#ifndef _DR_67_H
#define _DR_67_H

/*****************************************************************************
 * dvbpsi_dvb_transport_stream_dr_s
 *****************************************************************************/
/*!
 * \struct dvbpsi_dvb_transport_stream_dr_s
 * \brief Transport Stream Descriptor
 *
 * This structure is used to store a decoded Transport Stream descriptor.
 */
/*!
 * \typedef struct dvbpsi_dvb_transport_stream_dr_s dvbpsi_dvb_transport_stream_dr_t
 * \brief dvbpsi_dvb_transport_stream_dr_t type definition.
 */
typedef struct dvbpsi_dvb_transport_stream_dr_s
{
    uint8_t  p_data[3]; /*!< three values that shall carry: 0x44, 0x56, 0x42 (ASCI "DVB") */
} dvbpsi_dvb_transport_stream_dr_t;

/*****************************************************************************
 * dvbpsi_decode_dvb_transport_stream_dr
 *****************************************************************************/
/*!
 * \fn dvbpsi_dvb_transport_stream_dr_t *dvbpsi_decode_dvb_transport_stream_dr(
 *        dvbpsi_descriptor_t *p_descriptor)
 * \brief Decode a Transport Stream descriptor (tag 0x67)
 * \param p_descriptor Raw descriptor to decode.
 * \return NULL if the descriptor could not be decoded or a pointer to a
 *         dvbpsi_dvb_transport_stream_dr_t structure.
 */
dvbpsi_dvb_transport_stream_dr_t *dvbpsi_decode_dvb_transport_stream_dr(dvbpsi_descriptor_t *p_descriptor);

/*****************************************************************************
 * dvbpsi_gen_dvb_transport_stream_dr
 *****************************************************************************/
/*!
 * \fn dvbpsi_descriptor_t *dvbpsi_gen_dvb_transport_stream_dr(dvbpsi_dvb_transport_stream_dr_t *p_decoded,
                                      bool b_duplicate);

 * \brief Transport Stream descriptor generator.
 * \param p_decoded pointer to a decoded Transport Stream descriptor structure
 * \param b_duplicate if true then duplicate the p_decoded structure into
 * the descriptor
 * \return a pointer to a new descriptor structure which contains encoded data.
 */
dvbpsi_descriptor_t *dvbpsi_gen_dvb_transport_stream_dr(dvbpsi_dvb_transport_stream_dr_t *p_decoded,
                                      bool b_duplicate);

#endif
