/*****************************************************************************
 * chain.c: decoder chain test
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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

/* The libdvbpsi distribution defines DVBPSI_DIST */
#ifdef DVBPSI_DIST
#include "../src/dvbpsi.h"
#include "../src/dvbpsi_private.h"
#include "../src/psi.h"
#include "../src/chain.h"
#include "../src/descriptor.h"
#include "../src/tables/pat.h"
#include "../src/tables/pmt.h"
#include "../src/tables/bat.h"
#include "../src/descriptors/dr.h"
#else
#include <dvbpsi/dvbpsi.h>
#include "../src/dvbpsi_private.h"
#include <dvbpsi/psi.h>
#include <dvbpsi/chain.h>
#include <dvbpsi/descriptor.h>
#include <dvbpsi/pat.h>
#include <dvbpsi/pmt.h>
#include <dvbpsi/bat.h>
#include <dvbpsi/dr.h>
#endif

#define TEST_PASSED(msg) fprintf(stderr, "test %s -- PASSED\n", (msg));
#define TEST_FAILED(msg) fprintf(stderr, "test %s -- FAILED\n", (msg));

/* debug */
#define _TEST_CHAIN_DEBUG
#ifdef _TEST_CHAIN_DEBUG /* debug */
static void dvbpsi_decoder_chain_dump(dvbpsi_t *p_dvbpsi)
{
    dvbpsi_decoder_t *p = (dvbpsi_decoder_t *)p_dvbpsi->p_decoder;
    while (p) {
        dvbpsi_debug(p_dvbpsi, "dump chain", "found decoder %d:%d",
                     p->i_table_id, p->i_extension);
        p = p->p_next;
    }
}
#endif

static void message(dvbpsi_t *handle, const dvbpsi_msg_level_t level, const char* msg)
{
    switch(level)
    {
        case DVBPSI_MSG_ERROR: fprintf(stderr, "Error: "); break;
        case DVBPSI_MSG_WARN:  fprintf(stderr, "Warning: "); break;
        case DVBPSI_MSG_DEBUG: fprintf(stderr, "Debug: "); break;
        default: /* do nothing */
            return;
    }
    fprintf(stderr, "%s\n", msg);
}

static bool chain_add(dvbpsi_t *p_dvbpsi, const int count)
{
    for (int i = 0; i < count; i++) {
        dvbpsi_decoder_t *p_dec = calloc(1,sizeof(dvbpsi_decoder_t));
        if (p_dec == NULL) {
            fprintf(stderr, "out of memory\n");
            return false;
        }
        p_dec->i_table_id = i;
        p_dec->i_extension = i;

        if (!dvbpsi_decoder_chain_add(p_dvbpsi, p_dec)) {
            fprintf(stderr, "failed to add decoder to chain\n");
            return false;
        }

        assert(p_dvbpsi->p_decoder);
        /* verify adding to chain */
        p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, i, i);
        if (p_dec == NULL) {
            fprintf(stderr, "could not find decoder (%d:%d)\n", i, i);
            return false;
        }
    }
    return true;
}

static bool chain_add_table_extension(dvbpsi_t *p_dvbpsi, const int count)
{
    for (int i = 0; i < count; i++) {
        dvbpsi_decoder_t *p_dec = calloc(1,sizeof(dvbpsi_decoder_t));
        if (p_dec == NULL) {
            fprintf(stderr, "out of memory\n");
            return false;
        }
        const int i_extension = i + 1;
        p_dec->i_table_id = i;
        p_dec->i_extension = i_extension;

        if (!dvbpsi_decoder_chain_add(p_dvbpsi, p_dec)) {
            fprintf(stderr, "failed to add decoder to chain\n");
            return false;
        }

        assert(p_dvbpsi->p_decoder);
        /* verify adding to chain */
        p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, i, i_extension);
        if (p_dec == NULL) {
            fprintf(stderr, "could not find decoder (%d:%d)\n", i, i_extension);
            return false;
        }
    }
    return true;
}

static bool chain_find(dvbpsi_t *p_dvbpsi, const int count)
{
    for (int i = 0; i < count; i++) {
        dvbpsi_decoder_t *p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, i, i);
        if (p_dec == NULL) {
            fprintf(stderr, "could not find decoder (%d:%d)\n", i, i);
            return false;
        }
    }
    return true;
}

static bool chain_release(dvbpsi_t *p_dvbpsi, const int count)
{
    for (int i = 0; i < count; i++) {
        dvbpsi_decoder_t *p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, i, i);
        if (p_dec == NULL) {
            fprintf(stderr, "failed to find decoder (%d:%d) in chain\n", i, i);
            return false;
        }
        if (!dvbpsi_decoder_chain_remove(p_dvbpsi, p_dec)) {
            fprintf(stderr, "failed to delete decoder (%d:%d) from chain\n", i, i);
            return false;
        }
        /* NOTE: normally we would call dvbpsi_decoder_delete(p_dec) for
         * a PSI table decoder allocated with dvbpsi_decoder_new(). However
         * for this test the contents of the decoder does not really matter
         * and therefor we allocate with calloc() and free with free(). */
        free(p_dec);
        p_dec = NULL;
    }
    return true;
}

static bool chain_release_with_extension(dvbpsi_t *p_dvbpsi, const int count)
{
    for (int i = 0; i < count; i++) {
        const int i_extension = i + 1;
        dvbpsi_decoder_t *p_dec = dvbpsi_decoder_chain_get(p_dvbpsi, i, i_extension);
        if (p_dec == NULL) {
            fprintf(stderr, "failed to find decoder (%d:%d) in chain\n", i, i_extension);
            return false;
        }
        if (!dvbpsi_decoder_chain_remove(p_dvbpsi, p_dec)) {
            fprintf(stderr, "failed to delete decoder (%d:%d) from chain\n", i, i_extension);
            return false;
        }
        /* NOTE: normally we would call dvbpsi_decoder_delete(p_dec) for
         * a PSI table decoder allocated with dvbpsi_decoder_new(). However
         * for this test the contents of the decoder does not really matter
         * and therefor we allocate with calloc() and free with free(). */
        free(p_dec);
        p_dec = NULL;
    }
    return true;
}

/*****************************************************************************
 * CHAIN TESTS
 *****************************************************************************/
static int run_chain_test(const int i_decoders)
{
    dvbpsi_t *p_dvbpsi = dvbpsi_new(&message, DVBPSI_MSG_DEBUG);
    if (p_dvbpsi == NULL)
        return 1;

    /* Test dvbpsi_decoder_chain_add() */
    if (!chain_add(p_dvbpsi, i_decoders)) {
        TEST_FAILED("dvbpsi_decoder_chain_add");
        goto error;
    }
    TEST_PASSED("dvbpsi_decoder_chain_add");

    /* Test dvbpsi_decoder_chain_get() */
    if (!chain_find(p_dvbpsi, i_decoders)) {
       TEST_FAILED("dvbpsi_decoder_chain_get");
       goto error;
    }
    TEST_PASSED("dvbpsi_decoder_chain_get");

    /* Test dvbpsi_decoder_chain_add() with table extensions */
    if (!chain_add_table_extension(p_dvbpsi, i_decoders)) {
        TEST_FAILED("dvbpsi_decoder_chain_add with extensions");
        goto error;
    }
    TEST_PASSED("dvbpsi_decoder_chain_add with extensions");

#ifdef _TEST_CHAIN_DEBUG
    dvbpsi_decoder_chain_dump(p_dvbpsi);
#endif

    /* Test dvbpsi_decoder_chain_remove() */
    if (!chain_release(p_dvbpsi, i_decoders)) {
        TEST_FAILED("dvbpsi_decoder_chain_remove");
        dvbpsi_delete(p_dvbpsi);
        return 1;
    }
    TEST_PASSED("dvbpsi_decoder_chain_remove");

    /* Test dvbpsi_decoder_chain_remove() */
    if (!chain_release_with_extension(p_dvbpsi, i_decoders)) {
        TEST_FAILED("dvbpsi_decoder_chain_remove with extensions");
        dvbpsi_delete(p_dvbpsi);
        return 1;
    }
    TEST_PASSED("dvbpsi_decoder_chain_remove with extensions");

    p_dvbpsi->p_decoder = NULL;
    dvbpsi_delete(p_dvbpsi);
    fprintf(stderr, "ALL CHAIN TESTS PASSED\n");
    return 0;

error:
    /* cleanup */
    if (!chain_release(p_dvbpsi, i_decoders))
        fprintf(stderr, "failed to cleanup after errors\n");
    p_dvbpsi->p_decoder = NULL;
    dvbpsi_delete(p_dvbpsi);
    return 1;

}

/*****************************************************************************
 * CHAIN DEMUX TESTS
 *****************************************************************************/

/*****************************************************************************
 * DumpMaxBitrateDescriptor
 *****************************************************************************/
static void DumpMaxBitrateDescriptor(dvbpsi_mpeg_max_bitrate_dr_t* bitrate_descriptor)
{
    printf("Bitrate: %d\n", bitrate_descriptor->i_max_bitrate);
}

/*****************************************************************************
 * DumpSystemClockDescriptor
 *****************************************************************************/
static void DumpSystemClockDescriptor(dvbpsi_mpeg_system_clock_dr_t* p_clock_descriptor)
{
    printf("External clock: %s, Accuracy: %E\n",
           p_clock_descriptor->b_external_clock_ref ? "Yes" : "No",
           p_clock_descriptor->i_clock_accuracy_integer *
           pow(10.0, -(double)p_clock_descriptor->i_clock_accuracy_exponent));
}

/*****************************************************************************
 * DumpStreamIdentifierDescriptor
 *****************************************************************************/
static void DumpStreamIdentifierDescriptor(dvbpsi_dvb_stream_identifier_dr_t* p_si_descriptor)
{
    printf("Component tag: %d\n",
           p_si_descriptor->i_component_tag);
}

/*****************************************************************************
 * DumpSubtitleDescriptor
 *****************************************************************************/
static void DumpSubtitleDescriptor(dvbpsi_dvb_subtitling_dr_t* p_subtitle_descriptor)
{
    int a;

    printf("%d subtitles,\n", p_subtitle_descriptor->i_subtitles_number);
    for (a = 0; a < p_subtitle_descriptor->i_subtitles_number; ++a)
    {
        printf("       | %d - lang: %c%c%c, type: %d, cpid: %d, apid: %d\n", a,
               p_subtitle_descriptor->p_subtitle[a].i_iso6392_language_code[0],
                p_subtitle_descriptor->p_subtitle[a].i_iso6392_language_code[1],
                p_subtitle_descriptor->p_subtitle[a].i_iso6392_language_code[2],
                p_subtitle_descriptor->p_subtitle[a].i_subtitling_type,
                p_subtitle_descriptor->p_subtitle[a].i_composition_page_id,
                p_subtitle_descriptor->p_subtitle[a].i_ancillary_page_id);
    }
}

/*****************************************************************************
 * Print_0xb1
 *****************************************************************************/
static void Dump0xb11(uint16_t i_ts_id, dvbpsi_descriptor_t* p_descriptor)
{
    int i;
    uint8_t *pdata;
    unsigned int sub_bouquet_id;
    int num;
    unsigned int formater;
    pdata = p_descriptor->p_data;
    num = (p_descriptor->i_length-2)/9;
    sub_bouquet_id = (((unsigned int)pdata[0]&0xff)<<8)|pdata[1];
    if (sub_bouquet_id != 0xffff)
    {
        printf("sub_bouquet_id!=0xffff\n");
        return;
    }
    if (num*9 != p_descriptor->i_length-2)
    {
        printf("num of private_services error\n");
        return;
    }
    pdata+=2;

    printf("\nts_id: %d, service_num: %d, service_id list: \n",i_ts_id,num);
    formater=0;
    for (i = 0; i < num; i++)
    {
        uint16_t service_id = (((uint16_t)pdata[0]&0xff)<<8)|pdata[1];
        printf("%.4x ", service_id);
        formater++;
        if (0 == formater%16)
        {
            printf("\n");
        }
        pdata+=9;
    }
    printf("\r\n");
}

/*****************************************************************************
 * DumpDescriptors
 *****************************************************************************/
static void DumpDescriptors(const char* str, dvbpsi_descriptor_t* p_descriptor)
{
#define SYSTEM_CLOCK_DR 0x0B
#define MAX_BITRATE_DR 0x0E
#define STREAM_IDENTIFIER_DR 0x52
#define SUBTITLING_DR 0x59

    while (p_descriptor)
    {
        printf("%s 0x%02x : ", str, p_descriptor->i_tag);
        switch (p_descriptor->i_tag)
        {
        case SYSTEM_CLOCK_DR:
            DumpSystemClockDescriptor(dvbpsi_decode_mpeg_system_clock_dr(p_descriptor));
            break;
        case MAX_BITRATE_DR:
            DumpMaxBitrateDescriptor(dvbpsi_decode_mpeg_max_bitrate_dr(p_descriptor));
            break;
        case STREAM_IDENTIFIER_DR:
            DumpStreamIdentifierDescriptor(dvbpsi_decode_dvb_stream_identifier_dr(p_descriptor));
            break;
        case SUBTITLING_DR:
            DumpSubtitleDescriptor(dvbpsi_decode_dvb_subtitling_dr(p_descriptor));
            break;
            break;
        default:
            printf("\"");
            for(int i = 0; i < p_descriptor->i_length; i++)
                printf("%c", p_descriptor->p_data[i]);
            printf("\"\n");
        }
        p_descriptor = p_descriptor->p_next;
    }
}

/*****************************************************************************
 * DumpDescriptors_verbose
 *****************************************************************************/
static void DumpDescriptors_verbose(uint16_t i_ts_id, dvbpsi_descriptor_t* p_descriptor)
{
    while (p_descriptor)
    {
        if (0xb1 == p_descriptor->i_tag)
        {
            Dump0xb11(i_ts_id,p_descriptor);
        }
        p_descriptor = p_descriptor->p_next;
    }
}

/*****************************************************************************
 * DumpBAT_verbose
 *****************************************************************************/
static void DumpBAT_verbose(void* p_zero, dvbpsi_bat_t* p_bat)
{
    dvbpsi_bat_ts_t* p_ts = p_bat->p_first_ts;
    while (p_ts)
    {
        DumpDescriptors_verbose(p_ts->i_ts_id, p_ts->p_first_descriptor);
        p_ts = p_ts->p_next;
    }
}

/*****************************************************************************
 * DumpBAT
 *****************************************************************************/
static void DumpBAT(void* p_zero, dvbpsi_bat_t* p_bat)
{
    dvbpsi_bat_ts_t* p_ts = p_bat->p_first_ts;
    printf(  "\n");
    printf(  "New active BAT(binary dumped)\n");
    printf(  "  bouquet_id : %d\n",
             p_bat->i_extension);
    printf(  "  version_number : %d\n",
             p_bat->i_version);
    printf(  "    | ts_id \n");
    while(p_ts)
    {
        printf("    | 0x%02x \n",
               p_ts->i_ts_id);
        DumpDescriptors("    |  ]", p_ts->p_first_descriptor);
        p_ts = p_ts->p_next;
    }
    printf(  "\n");
    printf(  "New active BAT(string dumped)\n");
    DumpBAT_verbose(p_zero,p_bat);
    printf("\n");
    dvbpsi_bat_delete(p_bat);
}

/*****************************************************************************
 * DumpPAT
 *****************************************************************************/
static void DumpPAT(void* p_zero, dvbpsi_pat_t* p_pat)
{
    dvbpsi_pat_program_t* p_program = p_pat->p_first_program;
    printf(  "\n");
    printf(  "New PAT\n");
    printf(  "  transport_stream_id : %d\n", p_pat->i_ts_id);
    printf(  "  version_number      : %d\n", p_pat->i_version);
    printf(  "    | program_number @ [NIT|PMT]_PID\n");
    while (p_program)
    {
        printf("    | %14d @ 0x%x (%d)\n",
               p_program->i_number, p_program->i_pid, p_program->i_pid);
        p_program = p_program->p_next;
    }
    printf(  "  active              : %d\n", p_pat->b_current_next);
    dvbpsi_pat_delete(p_pat);
}

/*****************************************************************************
 * GetTypeName
 *****************************************************************************/
static char* GetTypeName(uint8_t type)
{
    switch (type)
    {
    case 0x00:
        return "Reserved";
    case 0x01:
        return "ISO/IEC 11172 Video";
    case 0x02:
        return "ISO/IEC 13818-2 Video";
    case 0x03:
        return "ISO/IEC 11172 Audio";
    case 0x04:
        return "ISO/IEC 13818-3 Audio";
    case 0x05:
        return "ISO/IEC 13818-1 Private Section";
    case 0x06:
        return "ISO/IEC 13818-1 Private PES data packets";
    case 0x07:
        return "ISO/IEC 13522 MHEG";
    case 0x08:
        return "ISO/IEC 13818-1 Annex A DSM CC";
    case 0x09:
        return "H222.1";
    case 0x0A:
        return "ISO/IEC 13818-6 type A";
    case 0x0B:
        return "ISO/IEC 13818-6 type B";
    case 0x0C:
        return "ISO/IEC 13818-6 type C";
    case 0x0D:
        return "ISO/IEC 13818-6 type D";
    case 0x0E:
        return "ISO/IEC 13818-1 auxillary";
    default:
        if (type < 0x80)
            return "ISO/IEC 13818-1 reserved";
        else
            return "User Private";
    }
}

/*****************************************************************************
 * DumpPMT
 *****************************************************************************/
static void DumpPMT(void* p_zero, dvbpsi_pmt_t* p_pmt)
{
    dvbpsi_pmt_es_t* p_es = p_pmt->p_first_es;
    printf(  "\n");
    printf(  "New active PMT\n");
    printf(  "  program_number : %d\n",
             p_pmt->i_program_number);
    printf(  "  version_number : %d\n",
             p_pmt->i_version);
    printf(  "  PCR_PID        : 0x%x (%d)\n",
             p_pmt->i_pcr_pid, p_pmt->i_pcr_pid);
    DumpDescriptors("    ]", p_pmt->p_first_descriptor);
    printf(  "    | type @ elementary_PID\n");
    while (p_es)
    {
        printf("    | 0x%02x (%s) @ 0x%x (%d)\n",
               p_es->i_type, GetTypeName(p_es->i_type),
               p_es->i_pid, p_es->i_pid);
        DumpDescriptors("    |  ]", p_es->p_first_descriptor);
        p_es = p_es->p_next;
    }
    dvbpsi_pmt_delete(p_pmt);
}

/*****************************************************************************
 * NewSubtable
 *****************************************************************************/
static void NewSubtable(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,
                           void * p_zero)
{
    fprintf(stderr, "Adding subtable %d %d\n", i_table_id, i_extension);
    switch (i_table_id)
    {
    case 0x00: /* PAT */
        if (!dvbpsi_pat_attach(p_dvbpsi, i_table_id, i_extension, DumpPAT, NULL))
            fprintf(stderr, "Failed to attach PAT decoder\n");
        break;
    case 0x02: /* PMT */
        if (!dvbpsi_pmt_attach(p_dvbpsi, i_table_id, i_extension, DumpPMT, NULL))
            fprintf(stderr, "Failed to attach PMT decoder\n");
        break;
    case 0x4a: /* BAT */
        if (!dvbpsi_bat_attach(p_dvbpsi, i_table_id, i_extension, DumpBAT, NULL))
            fprintf(stderr, "failed to attach BAT subdecoder\n");
        break;
    }
}

/*****************************************************************************
 * DelSubtable
 *****************************************************************************/
static void DelSubtable(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension)
{
    fprintf(stderr, "Deleting subtable %d %d\n", i_table_id, i_extension);
    switch (i_table_id)
    {
    case 0x00: dvbpsi_pat_detach(p_dvbpsi, i_table_id, i_extension); break;
    case 0x02: dvbpsi_pmt_detach(p_dvbpsi, i_table_id, i_extension); break;
    case 0x4a: dvbpsi_bat_detach(p_dvbpsi, i_table_id, i_extension); break;
    }
}

/* */
static int run_chain_demux_test(void)
{
    dvbpsi_t *p_dvbpsi = dvbpsi_new(&message, DVBPSI_MSG_DEBUG);
    if (p_dvbpsi == NULL)
        return 1;

    if (!dvbpsi_chain_demux_new(p_dvbpsi, NewSubtable, DelSubtable, NULL))
        goto error;

    uint8_t pkt[2][188];
    for (int b = 0; b < 2; b++)
        memset(pkt[b], 0xff, 188);
    uint8_t pat_pkt [] = {
        0x47, 0x40, 0x00, 0x10, 0x00, 0x00, 0xB0, 0x19, 0x00, 0x01, 0xC0, 0x00,
        0x0A, 0x00, 0x00, 0xE0, 0x12, 0x00, 0x01, 0xE0, 0x42, 0x00, 0x02, 0xE0,
        0x21, 0x00, 0x03, 0xE0, 0x24, 0x19, 0x0E, 0x7B, 0x40,
    }; /* PAT */
    memcpy(pkt[0], pat_pkt, 33);
    uint8_t pmt_pkt[] = {
        0x47, 0x42, 0x12, 0x10, 0x00, 0x02, 0xB0, 0x78, 0x00, 0x0C, 0xC0, 0x00,
        0x00, 0xE0, 0x2A, 0xF0, 0x33, 0x0C, 0x1A, 0x61, 0x62, 0x63, 0x64, 0x65,
        0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
        0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x2A, 0x0C, 0x6D,
        0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x02,
        0x01, 0x65, 0x00, 0x04, 0x68, 0x69, 0x6A, 0x6B, 0x0C, 0xE0, 0x2A, 0xF0,
        0x33, 0x0C, 0x1A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
        0x76, 0x77, 0x78, 0x79, 0x7A, 0x2A, 0x0C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
        0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x02, 0x01, 0x65, 0x00, 0x04,
        0x68, 0x69, 0x6A, 0x6B, 0x05, 0xCB, 0xF0, 0xCA,
    }; /* PMT */
    memcpy(pkt[1], pmt_pkt, 128);

    for (int i = 0; i < 2; i++)
    {
        uint8_t *p_data = pkt[i];
        dvbpsi_packet_push(p_dvbpsi, p_data);
    }

#ifdef _TEST_CHAIN_DEBUG
    dvbpsi_decoder_chain_dump(p_dvbpsi);
#endif

    if (!dvbpsi_chain_demux_delete(p_dvbpsi))
        goto error;

    p_dvbpsi->p_decoder = NULL;
    dvbpsi_delete(p_dvbpsi);
    fprintf(stderr, "ALL CHAIN DEMUX TESTS PASSED\n");
    return 0;

error:
    /* cleanup */
    if (!dvbpsi_chain_demux_delete(p_dvbpsi))
        fprintf(stderr, "Failed to cleanup chain_demux after errors\n");
    p_dvbpsi->p_decoder = NULL;
    dvbpsi_delete(p_dvbpsi);
    return 1;

}

/*****************************************************************************
 * main
 *****************************************************************************/
#define CHAIN_DECODERS (20)
int main(int i_argc, char* pa_argv[])
{
    if (run_chain_test(CHAIN_DECODERS) < 0)
        return 1;
    if (run_chain_demux_test() < 0)
        return 1;

    return 0;
}
