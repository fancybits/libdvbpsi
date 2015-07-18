static int my_memcmp(const void *s1, const void *s2, size_t n,
    int (*compar)(const void *, const void *))
{
    for(size_t i = 0 ; i < n ; ++i)
    {
        int cmp = compar(s1, s2);
        if(cmp != 0) return cmp;
    }
    return 0;
}

static int compare_dvbpsi_service_list_service_s(const void *s1, const void *s2)
{
    const struct dvbpsi_service_list_service_s *a = s1, *b = s2;
    if(a->i_service_id < b->i_service_id ||
      a->i_service_type < b->i_service_type) return -1;
    else if(a->i_service_id > b->i_service_id ||
      a->i_service_type > b->i_service_type) return 1;
    else return 0;
}

static int compare_dvbpsi_nvod_ref_t(const void *s1, const void *s2)
{
    const dvbpsi_nvod_ref_t *a = s1, *b = s2;
    if(a->i_transport_stream_id < b->i_transport_stream_id ||
        a->i_original_network_id < b->i_original_network_id ||
        a->i_service_id < b->i_service_id) return -1;
    else if(a->i_transport_stream_id > b->i_transport_stream_id ||
        a->i_original_network_id > b->i_original_network_id ||
        a->i_service_id > b->i_service_id) return 1;
    else return 0;
}

static int compare_dvbpsi_ca_system_t(const void *s1, const void *s2)
{
    const dvbpsi_ca_system_t *a = s1, *b = s2;
    if(a->i_ca_system_id < b->i_ca_system_id) return -1;
    else if(a->i_ca_system_id > b->i_ca_system_id) return 1;
    else return 0;
}

static int compare_dvbpsi_content_t(const void *s1, const void *s2)
{
    const dvbpsi_content_t *a = s1, *b = s2;
    if(a->i_type < b->i_type || a->i_user_byte < b->i_user_byte) return -1;
    else if(a->i_type > b->i_type || a->i_user_byte > b->i_user_byte) return 1;
    else return 0;
}

static int compare_dvbpsi_parental_rating_t(const void *s1, const void *s2)
{
    const dvbpsi_parental_rating_t *a = s1, *b = s2;
    uint32_t a_cc = (a->i_country_code & 0xffffff),
        b_cc = (b->i_country_code & 0xffffff);
    if(a_cc < b_cc || a->i_rating < b->i_rating) return -1;
    else if(a_cc > b_cc || a->i_rating > b->i_rating) return 1;
    else return 0;
}

static int compare_dvbpsi_teletextpage_t(const void *s1, const void *s2)
{
    const dvbpsi_teletextpage_t *a = s1, *b = s2;
    int iso_cmp = memcmp(a->i_iso6392_language_code, b->i_iso6392_language_code,
        sizeof(a->i_iso6392_language_code));
    uint8_t a_tt = (a->i_teletext_type & 0x1f),
        a_tmn = (a->i_teletext_magazine_number & 0x07),
        b_tt = (b->i_teletext_type & 0x1f),
        b_tmn = (b->i_teletext_magazine_number & 0x07);
    if(iso_cmp < 0 || a_tt < b_tt || a_tmn < b_tmn ||
        a->i_teletext_page_number < b->i_teletext_page_number) return -1;
    else if(iso_cmp > 0 || a_tt > b_tt || a_tmn > b_tmn ||
        a->i_teletext_page_number > b->i_teletext_page_number) return 1;
    else return 0;
}
