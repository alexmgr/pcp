/*
 *------------------------------------------------------------------
 * test_pcp_client_db.c
 *
 * May 7, 2013, ptatrai
 *
 * Copyright (c) 2013-2013 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcp_client_db.h"
#include "test_macro.h"
#include "pcp_utils.h"

int ret_0_func(pcp_server_t *f, void* data)
{
    return 0;
}

int ret_1_func(pcp_server_t *f, void* data)
{
    return 1;
}

void test_pcp_server_functions()
{
    struct in6_addr ip4, ip6;
        pcp_server_t *s1=NULL, *s2=NULL;
        int si1, si2;
        S6_ADDR32(&ip4)[0] = 0;
        S6_ADDR32(&ip4)[1] = 0;
        S6_ADDR32(&ip4)[2] = htonl(0xffff);
        S6_ADDR32(&ip4)[3] = 0x08080808;


        TEST(get_pcp_server(1)==NULL);
        TEST(get_pcp_server(0)==NULL);
        TEST(get_pcp_server(-1)==NULL);
        si1=pcp_new_server(&ip4, PCP_SERVER_PORT);
        TEST(si1 == 0);
        s1=get_pcp_server(si1);
        TEST(s1!=NULL);
        TEST(get_pcp_server(1)==NULL);
        TEST((s1->af==AF_INET)&&(IN6_ARE_ADDR_EQUAL(&ip4, s1->pcp_ip)));

        si2=pcp_new_server(&ip4, PCP_SERVER_PORT);
        TEST(si2 == 1);
        s2=get_pcp_server(1);
        TEST(s2!=NULL);
        TEST((s2->af==AF_INET)&&(IN6_ARE_ADDR_EQUAL(&ip4, s2->pcp_ip)));
        TEST(get_pcp_server(5)==NULL);

        S6_ADDR32(&ip6)[0] = 0;
        S6_ADDR32(&ip6)[1] = 1;
        S6_ADDR32(&ip6)[2] = 2;
        S6_ADDR32(&ip6)[3] = 3;

        si2=pcp_new_server(&ip6, PCP_SERVER_PORT);
        TEST(si2 == 2);
        s2=get_pcp_server(2);
        TEST(s2!=NULL);
        TEST((s2->af==AF_INET6)&&(IN6_ARE_ADDR_EQUAL(&ip6, s2->pcp_ip)));
        TEST(get_pcp_server(5)==NULL);

        TEST(pcp_new_server(&ip6, PCP_SERVER_PORT) == 3);
        TEST(pcp_new_server(&ip6, PCP_SERVER_PORT) == 4);
        TEST(pcp_new_server(&ip6, PCP_SERVER_PORT) == 5);
        TEST(pcp_new_server(&ip6, PCP_SERVER_PORT) == 6);

        TEST(s1==get_pcp_server_by_ip(&ip4));
        TEST(s2==get_pcp_server_by_ip(&ip6));

        pcp_db_free_pcp_servers();

        TEST(get_pcp_server(1)==NULL);
        TEST(get_pcp_server(0)==NULL);
        TEST(get_pcp_server(-1)==NULL);
        si1=pcp_new_server(&ip4, PCP_SERVER_PORT);
        TEST(si1 == 0);
        s1=get_pcp_server(si1);
        TEST(s1!=NULL);
        TEST(get_pcp_server(1)==NULL);
        TEST((s1->af==AF_INET)&&(IN6_ARE_ADDR_EQUAL(&ip4, s1->pcp_ip)));

        si2=pcp_new_server(&ip6, PCP_SERVER_PORT);
        TEST(si2 == 1);
        s2=get_pcp_server(si2);
        TEST(s2!=NULL);
        TEST((s2->af==AF_INET6)&&(IN6_ARE_ADDR_EQUAL(&ip6, s2->pcp_ip)));
        TEST(get_pcp_server(5)==NULL);

        TEST(pcp_db_foreach_server(NULL, NULL)==PCP_ERR_BAD_ARGS);
        TEST(pcp_db_foreach_server(ret_0_func, NULL)==PCP_ERR_MAX_SIZE);
        TEST(pcp_db_foreach_server(ret_1_func, NULL)==0);
}

int ret_func(pcp_flow_t* f, void*data)
{
    *(pcp_flow_t**)data=f;
    return 1;
}

void test_pcp_flow_funcs()
{
    pcp_flow_t *f1, *f2, *f3;
    uint32_t bucket, cnt;
    struct flow_key_data fkd;
    struct flow_key_data fkd2;
    fkd.operation = 1;
    memset(&fkd.src_ip,0,sizeof(fkd.src_ip));
    fkd2 = fkd;
    cnt = 0;

    fkd.map_peer.protocol = 1;
    fkd2.map_peer.protocol = 1;
    fkd2.map_peer.dst_port++;

    TEST(pcp_create_flow(get_pcp_server(0), NULL)==NULL);
    f1 = pcp_create_flow(get_pcp_server(0), &fkd);
    TEST(f1!=NULL);
    TEST(pcp_db_add_flow(f1)== PCP_ERR_SUCCESS);
    bucket = f1->key_bucket;

    f2 = pcp_create_flow(get_pcp_server(0), &fkd);
    TEST(f2!=NULL);
    TEST(pcp_db_rem_flow(f2)!=PCP_ERR_SUCCESS);
    TEST(pcp_db_add_flow(f2)== PCP_ERR_SUCCESS);
    TEST(f2->key_bucket==f1->key_bucket);

    f3 = pcp_create_flow(get_pcp_server(0), &fkd2);
    TEST(f3!=NULL);
    TEST(pcp_db_add_flow(f3)== PCP_ERR_SUCCESS);
    TEST(f3->key_bucket!=bucket);

    TEST(pcp_db_add_flow(NULL)!= PCP_ERR_SUCCESS);

    TEST(pcp_db_add_flow(pcp_create_flow(get_pcp_server(0), &fkd))== PCP_ERR_SUCCESS);
    TEST(pcp_db_add_flow(pcp_create_flow(get_pcp_server(0), &fkd2))== PCP_ERR_SUCCESS);
    TEST(pcp_db_add_flow(pcp_create_flow(get_pcp_server(0), &fkd2))== PCP_ERR_SUCCESS);
    TEST(pcp_db_rem_flow(NULL)!=PCP_ERR_SUCCESS);
    TEST(pcp_db_rem_flow(f2)==PCP_ERR_SUCCESS);

    f2 = pcp_create_flow(NULL, &fkd);
    TEST(f2!=NULL);
    TEST(pcp_db_add_flow(f2)== PCP_ERR_SUCCESS);
    TEST(f2->key_bucket==f1->key_bucket);

    f2->pcp_msg_buffer=malloc(10);
    f2->pcp_msg_len = 10;
    TEST(pcp_db_rem_flow(f2)==PCP_ERR_SUCCESS);
    TEST(pcp_db_rem_flow(f2)!=PCP_ERR_SUCCESS);
    TEST((f2->pcp_msg_buffer!=NULL)&&(f2->pcp_msg_len==10));

    TEST(pcp_delete_flow_intern(NULL)!=PCP_ERR_SUCCESS);

    TEST(pcp_get_flow(&fkd, 0)==f1);
    TEST(pcp_get_flow(&fkd, 1)==NULL);
    TEST(pcp_get_flow(&fkd2, 0)==f3);

    pcp_flow_clear_msg_buf(NULL);
    f3->pcp_msg_buffer=malloc(10);
    f3->pcp_msg_len = 10;
    pcp_flow_clear_msg_buf(f3);
    TEST((f3->pcp_msg_len==0)&&(f3->pcp_msg_buffer==NULL));
    f3->pcp_msg_buffer=malloc(10);
    f3->pcp_msg_len = 10;

    TEST(pcp_delete_flow_intern(f3)==PCP_ERR_SUCCESS);
    fkd2.map_peer.src_port++;
    TEST(pcp_get_flow(&fkd2, 0)==NULL);
    TEST(pcp_get_flow(NULL, 0)==NULL);

#ifdef PCP_EXPERIMENTAL
    pcp_db_add_md(f1, 11, "test", sizeof("test"));
    TEST(f1->md_val_count==1);
    pcp_db_add_md(f1, 11, "atest", sizeof("atest"));
    TEST(f1->md_val_count==1);
    pcp_db_add_md(f1, 13, "atest", sizeof("atest"));
    pcp_db_add_md(f1, 13,NULL,0);
    pcp_db_add_md(f1, 0,NULL,0);
    pcp_db_add_md(NULL,0,NULL,0);
    TEST(f1->md_val_count==3);
#endif

    TEST(pcp_delete_flow_intern(f1)==PCP_ERR_SUCCESS);
    TEST(pcp_delete_flow_intern(f2)==PCP_ERR_SUCCESS);

    TEST(pcp_db_foreach_flow(NULL,NULL)!=PCP_ERR_SUCCESS);

    while (pcp_db_foreach_flow(ret_func, &f1)==PCP_ERR_SUCCESS)
    {
        pcp_delete_flow_intern(f1);
        cnt++;
    }
    TEST(cnt==3);
}

int main()
{
    pcp_log_level = PCP_DEBUG_NONE;

    test_pcp_server_functions();
    test_pcp_flow_funcs();

    printf("Tests succeeded.\n\n");
    return 0;
}
