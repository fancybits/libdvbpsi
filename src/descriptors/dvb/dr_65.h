/*****************************************************************************
 * dr_65.h
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
 * \file dr_65.h
 * \author Jean-Paul Saman
 * \brief Scrambling descriptor
 */

#ifndef _DR_65_H
#define _DR_66_H

/*****************************************************************************
 * dvbpsi_dvb_scrambling_dr_s
 *****************************************************************************/
/*!
 * \struct dvbpsi_dvb_scrambling_dr_s
 * \brief Scrambling Descriptor
 *
 * This structure is used to store a decoded Scrambling descriptor.
 *
 * The meaning of i_scrambling_mode is according ETSI EN 300 468 V1.13.1 (2012-04)
 *
 * scrambling_mode   Description
 * ===============   ==========================================================
 *   0x00            Reserved for future use
 *   0x01            This value indicates use of DVB-CSA1.
 *                   It is the default mode and shall be used
 *                   when the scrambling descriptor is not present
 *                   in the program map section
 *   0x02            This value indicates use of DVB-CSA2
 *   0x03            This value indicates use of DVB-CSA3 in standard mode
 *   0x04            This value indicates use of DVB-CSA3 in minimally enhanced mode
 *   0x05            This value indicates use of DVB-CSA3 in fully enhanced mode
 *   0x06 to 0x6F    Reserved for future use
 *   0x70 to 0x7F    ATIS defined (ATIS-0800006, see annex J)
 *   0x80 to 0xFE    User defined
 *   0xFF            Reserved for future use
 */
/*!
 * \typedef struct dvbpsi_dvb_scrambling_dr_s dvbpsi_dvb_scrambling_dr_t
 * \brief dvbpsi_dvb_scrambling_dr_t type definition.
 */
typedef struct dvbpsi_dvb_scrambling_dr_s
{
    uint8_t  i_scrambling_mode; /*!< Scrambling mode */
} dvbpsi_dvb_scrambling_dr_t;

/*****************************************************************************
 * dvbpsi_dvb_scrambling_dr_s
 *****************************************************************************/
/*!
 * \fn dvbpsi_dvb_scrambling_dr_t *dvbpsi_decode_dvb_scrambling_dr(
 *        dvbpsi_descriptor_t *p_descriptor)
 * \brief Decode a Scrambling descriptor (tag 0x65)
 * \param p_descriptor Raw descriptor to decode.
 * \return NULL if the descriptor could not be decoded or a pointer to a
 *         dvbpsi_dvb_scrambling_dr_t structure.
 */
dvbpsi_dvb_scrambling_dr_t *dvbpsi_decode_dvb_scrambling_dr(dvbpsi_descriptor_t *p_descriptor);

/*****************************************************************************
 * dvbpsi_gen_scrambling_dr
 *****************************************************************************/
/*!
 * \fn dvbpsi_descriptor_t *dvbpsi_gen_scrambling_dr(dvbpsi_dvb_scrambling_dr_t *p_decoded,
                                      bool b_duplicate);

 * \brief Scrambling descriptor generator.
 * \param p_decoded pointer to a decoded Scrambling descriptor structure
 * \param b_duplicate if true then duplicate the p_decoded structure into
 * the descriptor
 * \return a pointer to a new descriptor structure which contains encoded data.
 */
dvbpsi_descriptor_t *dvbpsi_gen_dvb_scrambling_dr(dvbpsi_dvb_scrambling_dr_t *p_decoded,
                                      bool b_duplicate);

#endif
