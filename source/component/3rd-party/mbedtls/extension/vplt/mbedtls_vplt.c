/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "component/vsf_component_cfg.h"

#if VSF_USE_MBEDTLS == ENABLED

#include <mbedtls/version.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/certs.h>
#include <mbedtls/md5.h>
#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>
#include <mbedtls/aes.h>
#include <mbedtls/pk.h>
#include <mbedtls/error.h>
#include <mbedtls/debug.h>

#include "./mbedtls_vplt.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

__VSF_VPLT_DECORATOR__ vsf_mbedtls_vplt_t vsf_mbedtls_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_mbedtls_vplt_t, 0, 0, true),
    VSF_APPLET_VPLT_ENTRY_MOD(mbedtls, vsf_mbedtls_vplt),

    // <mbedtls/version.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_version_get_number),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_version_get_string),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_version_get_string_full),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_version_check_feature),

    // <mbedtls/error.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_strerror),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_high_level_strerr),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_low_level_strerr),

    // <mbedtls/debug.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_debug_set_threshold),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_debug_print_msg),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_debug_print_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_debug_print_buf),
#if defined(MBEDTLS_BIGNUM_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_debug_print_mpi),
#endif
#if defined(MBEDTLS_ECP_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_debug_print_ecp),
#endif
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_debug_print_crt),
#endif
#if defined(MBEDTLS_ECDH_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_debug_printf_ecdh),
#endif

    // <mbedtls/md5.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_md5_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_md5_free),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_md5_clone),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_md5_starts_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_md5_update_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_md5_finish_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_internal_md5_process),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_md5_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_md5_self_test),

    // <mbedtls/sha1.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha1_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha1_free),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha1_clone),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha1_starts_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha1_update_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha1_finish_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_internal_sha1_process),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha1_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha1_self_test),

    // <mbedtls/sha256.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha256_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha256_free),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha256_clone),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha256_starts_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha256_update_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha256_finish_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_internal_sha256_process),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha256_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_sha256_self_test),

    // <mbedtls/aes.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_free),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_setkey_enc),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_setkey_dec),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_crypt_ecb),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_internal_aes_encrypt),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_internal_aes_decrypt),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_self_test),
#if defined(MBEDTLS_CIPHER_MODE_XTS)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_xts_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_xts_free),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_xts_setkey_enc),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_xts_setkey_dec),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_crypt_xts),
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_crypt_cbc),
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_crypt_cfb128),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_crypt_cfb8),
#endif
#if defined(MBEDTLS_CIPHER_MODE_OFB)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_crypt_ofb),
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_aes_crypt_ctr),
#endif

    // <mbedtls/pk.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_info_from_type),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_free),
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_restart_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_restart_free),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_setup),
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_setup_opaque),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_wrap_as_opaque),
#endif
#if defined(MBEDTLS_PK_RSA_ALT_SUPPORT)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_setup_rsa_alt),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_get_bitlen),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_can_do),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_verify),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_verify_restartable),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_verify_ext),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_sign),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_sign_restartable),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_decrypt),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_encrypt),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_check_pair),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_debug),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_get_name),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_get_type),
#if defined(MBEDTLS_PK_PARSE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_parse_key),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_parse_public_key),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_parse_subpubkey),
#   if defined(MBEDTLS_FS_IO)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_parse_keyfile),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_parse_public_keyfile),
#   endif
#endif
#if defined(MBEDTLS_PK_WRITE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_write_key_der),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_write_pubkey_der),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_write_pubkey),
#   if defined(MBEDTLS_PEM_WRITE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_write_pubkey_pem),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_write_key_pem),
#   endif
#endif
#if defined(MBEDTLS_FS_IO)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_pk_load_file),
#endif

    // <mbedtls/x509_crt.h>
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_parse_der),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_parse_der_with_ext_cb),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_parse_der_nocopy),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_parse),
#   if defined(MBEDTLS_FS_IO)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_parse_file),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_parse_path),
#   endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_parse_subject_alt_name),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_info),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_verify_info),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_verify),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_verify_with_profile),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_verify_restartable),
#   if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_verify_with_ca_cb),
#   endif
#   if defined(MBEDTLS_X509_CHECK_KEY_USAGE)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_check_key_usage),
#   endif
#   if defined(MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_check_extended_key_usage),
#   endif
#   if defined(MBEDTLS_X509_CRL_PARSE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_is_revoked),
#   endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_free),
#   if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_restart_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crt_restart_free),
#   endif
#endif
#if defined(MBEDTLS_X509_CRT_WRITE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_version),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_serial),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_validity),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_issuer_name),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_subject_name),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_subject_key),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_issuer_key),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_md_alg),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_extension),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_basic_constraints),
#   if defined(MBEDTLS_SHA1_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_subject_key_identifier),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_authority_key_identifier),
#   endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_key_usage),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_set_ns_cert_type),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_free),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_der),
#endif
#if defined(MBEDTLS_PEM_WRITE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509write_crt_pem),
#endif

    // <mbedtls/x509_crl.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crl_parse_der),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crl_parse),
#if defined(MBEDTLS_FS_IO)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crl_parse_file),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crl_info),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crl_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_x509_crl_free),

    // <mbedtls/ctr_drgb.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_seed),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_free),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_set_prediction_resistance),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_set_entropy_len),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_set_nonce_len),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_set_reseed_interval),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_reseed),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_update_ret),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_random_with_add),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_random),
#if defined(MBEDTLS_FS_IO)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_write_seed_file),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_update_seed_file),
#endif
#if defined(MBEDTLS_SELF_TEST)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ctr_drbg_self_test),
#endif

    // <mbedtls/ssl.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_ciphersuite_name),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_ciphersuite_id),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_setup),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_session_reset),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_endpoint),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_transport),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_authmode),
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_verify),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_rng),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_dbg),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_bio),
#if defined(MBEDTLS_SSL_PROTO_DTLS)
#   if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_cid),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_peer_cid),
#   endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_mtu),
#endif
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_verify),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_read_timeout),
#if defined(MBEDTLS_SSL_RECORD_CHECKING)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_check_record),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_timer_cb),
#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_SRV_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_session_tickets_cb),
#endif
#if defined(MBEDTLS_SSL_EXPORT_KEYS)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_export_keys_cb),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_export_keys_ext_cb),
#endif
#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_async_private_cb),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_get_async_config_data),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_async_operation_data),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_async_operation_data),
#endif
#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY) && defined(MBEDTLS_SSL_SRV_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_dtls_cookies),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_client_transport_id),
#endif
#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_dtls_anti_replay),
#endif
#if defined(MBEDTLS_SSL_DTLS_BADMAC_LIMIT)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_dtls_badmac_limit),
#endif
#if defined(MBEDTLS_SSL_PROTO_DTLS)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_datagram_packing),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_handshake_timeout),
#endif
#if defined(MBEDTLS_SSL_SRV_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_session_cache),
#endif
#if defined(MBEDTLS_SSL_CLI_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_session),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_session_load),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_session_pointer),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_ciphersuites),
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_cid),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_ciphersuites_for_version),
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_cert_profile),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_ca_chain),
#   if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_ca_cb),
#   endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_own_cert),
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_psk),
#   if defined(MBEDTLS_USE_PSA_CRYPTO)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_psk_opaque),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_hs_psk_opaque),
#   endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_hs_psk),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_psk_cb),
#endif
#if defined(MBEDTLS_DHM_C) && defined(MBEDTLS_SSL_SRV_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_dh_param_bin),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_dh_param_ctx),
#endif
#if defined(MBEDTLS_DHM_C) && defined(MBEDTLS_SSL_CLI_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_dhm_min_bitlen),
#endif
#if defined(MBEDTLS_ECP_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_curves),
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_sig_hashes),
#endif
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_hostname),
#endif
#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_hs_own_cert),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_hs_ca_chain),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_hs_authmode),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_sni),
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_set_hs_ecjpake_password),
#endif
#if defined(MBEDTLS_SSL_ALPN)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_alpn_protocols),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_alpn_protocol),
#endif
#if defined(MBEDTLS_SSL_DTLS_SRTP)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_srtp_mki_value_supported),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_dtls_srtp_protection_profiles),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_dtls_srtp_set_mki_value),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_dtls_srtp_negotiation_result),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_max_version),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_min_version),
#if defined(MBEDTLS_SSL_FALLBACK_SCSV) && defined(MBEDTLS_SSL_CLI_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_fallback),
#endif
#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_encrypt_then_mac),
#endif
#if defined(MBEDTLS_SSL_EXTENDED_MASTER_SECRET)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_extended_master_secret),
#endif
#if defined(MBEDTLS_ARC4_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_arc4_support),
#endif
#if defined(MBEDTLS_SSL_SRV_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_cert_req_ca_list),
#endif
#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_max_frag_len),
#endif
#if defined(MBEDTLS_SSL_TRUNCATED_HMAC)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_truncated_hmac),
#endif
#if defined(MBEDTLS_SSL_CBC_RECORD_SPLITTING)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_cbc_record_splitting),
#endif
#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_CLI_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_session_tickets),
#endif
#if defined(MBEDTLS_SSL_RENEGOTIATION)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_renegotiation),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_renegotiation_enforced),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_renegotiation_period),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_renegotiate),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_conf_legacy_renegotiation),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_check_pending),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_bytes_avail),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_verify_result),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_ciphersuite),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_version),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_record_expansion),
#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_output_max_frag_len),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_input_max_frag_len),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_max_out_record_payload),
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_peer_cert),
#endif
#if defined(MBEDTLS_SSL_CLI_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_session),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_handshake),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_handshake_step),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_read),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_write),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_send_alert_message),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_close_notify),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_free),
#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_context_save),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_context_load),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_config_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_config_defaults),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_config_free),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_session_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_session_free),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_tls_prf),

    // <mbedtls/net_sockets.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_connect),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_bind),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_accept),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_poll),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_set_block),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_set_nonblock),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_usleep),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_recv),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_send),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_recv_timeout),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_close),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_net_free),

    // <mbedtls/entropy.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_entropy_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_entropy_free),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_entropy_add_source),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_entropy_gather),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_entropy_func),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_entropy_update_manual),
#if defined(MBEDTLS_ENTROPY_NV_SEED)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_entropy_update_nv_seed),
#endif
#if defined(MBEDTLS_FS_IO)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_entropy_write_seed_file),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_entropy_update_seed_file),
#endif
#if defined(MBEDTLS_SELF_TEST)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_entropy_self_test),
#   if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_entropy_source_self_test),
#   endif
#endif

    // <mbedtls/ssl_ciphersuites.h>
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_list_ciphersuites),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_ciphersuite_from_string),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_ciphersuite_from_id),
#if defined(MBEDTLS_PK_C)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_ciphersuite_sig_pk_alg),
#   if defined(MBEDTLS_USE_PSA_CRYPTO)
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_ciphersuite_sig_pk_psa_alg),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_ciphersuite_sig_pk_psa_usage),
#   endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_get_ciphersuite_sig_alg),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_ciphersuite_uses_ec),
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_ciphersuite_uses_psk),
#if MBEDTLS_VERSION_NUMBER >= 0x03020000
    VSF_APPLET_VPLT_ENTRY_FUNC(mbedtls_ssl_ciphersuite_get_cipher_key_bitlen),
#endif
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#endif      // VSF_USE_MBEDTLS
/* EOF */
