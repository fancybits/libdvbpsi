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
    else if(a->i_ca_system_id < b->i_ca_system_id) return 1;
    else return 0;
}
