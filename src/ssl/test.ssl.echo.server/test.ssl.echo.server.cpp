// test.ssl.echo.server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#pragma comment ( lib , "ssleay32" )
#pragma comment ( lib , "libeay32" )
#pragma comment ( lib , "ws2_32" )

/* serv.cpp  -  Minimal ssleay server for Unix
   30.9.1996, Sampo Kellomaki <sampo@iki.fi> */
/* mangled to work with SSLeay-0.9.0b and OpenSSL 0.9.2b
   Simplified to be even more minimal
   12/98 - 4/99 Wade Scholine <wades@mail.cybg.com> */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include <openssl/rsa.h>       /* SSLeay stuff */
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

/* define HOME to be dir for key and cert files... */
//#define HOME "./"

/* ���� ������ */
//#define CERTF  HOME "server-req.pem"
/* ���� ���Ű */
//#define KEYF  HOME  "server-key.pem"

#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }

// SSL �ڵ彦��ũ �޽�����ȯ ������ �˷��ִ� �ݺ��Լ�
void  ssl_info_callback(const SSL *s, int where, int ret);

int pem_password(char *buf, int size, int rwflag, void *userdata) {
	//strncpy_s( buf , "1331" , size );
	strncpy(buf, "1331",size);
	buf[size - 1] = '\0';
	return(strlen(buf));
}
// ȭ�鿡 ǥ�� �ϱ� ���� ���� BIO����
BIO * errBIO;

int main (int argc, char **argv)
{
	WSADATA ws;
	WSAStartup( MAKEWORD(2,2) , &ws );
    int err;
    int listen_sd;
    int sd;
    struct sockaddr_in sa_serv;
    struct sockaddr_in sa_cli;
    size_t client_len;

    const SSL_METHOD *meth;
    SSL_CTX* ctx;
    SSL*     ssl;
    X509*    client_cert;

    char*    str;
    char     buf [4096];

    /* SSL preliminaries. We keep the certificate and key with the context. */
    // ��� ���� ��Ʈ�� �ε�
    SSL_load_error_strings();
    // ��� �˰��� �ε�
    SSLeay_add_ssl_algorithms();
    // SSL ����3 �������� ���
    meth = SSLv3_method();
    //meth = TLSv1_server    // create a new SSL_CTX object as framework for TLS/SSL enabled functions
    // SSL ���ؽ�Ʈ ����
    ctx = SSL_CTX_new (meth);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(2);
    }

	SSL_CTX_set_default_passwd_cb( ctx , &pem_password );
    // SSL �ڵ彦��ũ �޽�����ȯ ������ �˷��ִ� �ݺ��Լ�
    SSL_CTX_set_info_callback(ctx,ssl_info_callback);

    // �ڽ��� �������� ���Ͽ��� �ε��Ѵ�.
    if (SSL_CTX_use_certificate_file(ctx, "crt.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(3);
    }

    fprintf(stderr, "======== PEM pass phrease confirm here\n");
    // �ڽ��� ����Ű�� ���Ͽ��� �ε��Ѵ�.
    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr,"======== PEM pass phrase does not match the password\n");
        ERR_print_errors_fp(stderr);
        exit(4);
    }

    // ���� �������� ����Ű�� �´��� Ȯ�� �Ѵ�.
    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr,"Private key does not match the certificate public key\n");
        exit(5);
    }

    listen_sd = socket (AF_INET, SOCK_STREAM, 0);
    CHK_ERR(listen_sd, "socket");

    memset (&sa_serv, '\0', sizeof(sa_serv));
    sa_serv.sin_family      = AF_INET;
    sa_serv.sin_addr.s_addr = INADDR_ANY;
    sa_serv.sin_port        = htons (1111);          /* Server Port number */

    err = bind(listen_sd, (struct sockaddr*) &sa_serv, sizeof (sa_serv));
    CHK_ERR(err, "bind");

    err = listen (listen_sd, 5);
    CHK_ERR(err, "listen");

    client_len = sizeof(sa_cli);
    sd = accept (listen_sd, (struct sockaddr*) &sa_cli, (int*)&client_len);
    CHK_ERR(sd, "accept");

	closesocket(listen_sd);

    printf ("Connection from %lx, port %d\n",
    sa_cli.sin_addr.s_addr, sa_cli.sin_port);

    // TCP connection is ready. Do server side SSL.
    // create a new SSL structure for a connection
    // SSL ����ü ����
    ssl = SSL_new (ctx);
    CHK_NULL(ssl);

    // connect the SSL object with a file descriptor
    // ����� ���ϰ� SSL���� ����
    SSL_set_fd (ssl, sd);
    // ���� �߿��� �Լ�, Ŭ���̾�Ʈ���� �ʱ� �������, �� �ڵ彦��ũ ������ ����
    printf ("SSL_accept start =========================\n");
    err = SSL_accept (ssl);
    CHK_SSL(err);
    printf ("SSL_accept end =========================\n");

    // Get the cipher - opt
    // ���� Ŭ���̾�Ʈ�� ���ǵ� ��ȣȭ �Ķ���������� ����
    printf ("SSL connection using %s\n", SSL_get_cipher (ssl));
    printf ("SSL connection using %s\n", SSL_CIPHER_get_name(SSL_get_current_cipher(ssl)));

    // Get client's certificate (note: beware of dynamic allocation) - opt
    client_cert = SSL_get_peer_certificate (ssl);


    if (client_cert != NULL)
    {
        printf ("Client certificate:\n");

        str = X509_NAME_oneline (X509_get_subject_name (client_cert), 0, 0);
        CHK_NULL(str);
        printf ("\t subject: %s\n", str);
		OPENSSL_free (str);

        str = X509_NAME_oneline (X509_get_issuer_name  (client_cert), 0, 0);
        CHK_NULL(str);
        printf ("\t issuer: %s\n", str);
        OPENSSL_free (str);

        /* We could do all sorts of certificate verification stuff here before
           deallocating the certificate. */

        X509_free (client_cert);
    }
    else
    {
        printf ("Client does not have certificate.\n");
    }

    // DATA EXCHANGE - Receive message and send reply.
    // Ŭ���̾�Ʈ�� ���� SSL ����� ���� �޽��� ����
    err = SSL_read (ssl, buf, sizeof(buf) - 1);
    CHK_SSL(err);

    buf[err] = '\0';
    // ���� �����͸� ȭ�鿡 ǥ��
    printf ("Got %d chars:'%s'\n", err, buf);

    // Ŭ���̾�Ʈ���� SSL ����� ���� �޽��� ����
    err = SSL_write (ssl, "I hear you.", strlen("I hear you."));
    CHK_SSL(err);
    /* Clean up. */
    closesocket (sd);
    SSL_free (ssl);
    SSL_CTX_free (ctx);
	getchar();
}

// SSL �ڵ彦��ũ �޽�����ȯ ������ �˷��ִ� �ݺ��Լ�
void  ssl_info_callback(const SSL *s, int where, int ret)
{
  char * writeString;
  int w;
  // ���� � �޽��� ��ȯ ���������� ��Ÿ��
  w = where & ~SSL_ST_MASK;

  // Ŭ���̾�Ʈ�� ���� ���� ��
  if (w & SSL_ST_CONNECT)
    writeString="SSL_connect";
  // ������ ������ �޾��� ��
  else if (w & SSL_ST_ACCEPT)
    writeString="SSL_accept";
  // �� �� ���� ���
  else
    writeString="undefined";

    fprintf(stderr, "======== writeString = [%s]\n", writeString);

  // �Ϲ����� �ڵ彦��ũ �������� �޽����� ���
  if (where & SSL_CB_LOOP)
  {
    // SSL_state_string_long(s) �Լ��� ���� ���� ����Ǵ� �޽����� �������� ǥ��
    BIO_printf(errBIO,"%s:%s\n",writeString,SSL_state_string_long(s));
    fprintf(stderr, "======== writeString = [%s], SSL_state_string_long(s) = [%s]\n", 
        writeString, SSL_state_string_long(s));
  }
  else if (where & SSL_CB_ALERT)
  { // alert ���������� ���
    writeString=(where & SSL_CB_READ)?"read":"write";
    BIO_printf(errBIO,"SSL3 alert %s:%s:%s\n",writeString,SSL_alert_type_string_long(ret),SSL_alert_desc_string_long(ret));
    fprintf(stderr, "======== writeString = [%s], SSL_alert_type_string_long(ret) = [%s], SSL_alert_desc_string_long(ret) = [%s]\n", 
          writeString, SSL_alert_type_string_long(ret), SSL_alert_desc_string_long(ret));
  }
  else if (where & SSL_CB_EXIT)
  { // ���� ������ ���
    if (ret == 0) {
      BIO_printf(errBIO,"%s:failed in %s\n",writeString,SSL_state_string_long(s));
      fprintf(stderr,"======== writeString = [%s], SSL_state_string_long(s) = [%s]\n", 
          writeString, SSL_state_string_long(s));
    }
    else if (ret < 0)
    {
      BIO_printf(errBIO,"%s:error in %s\n",writeString,SSL_state_string_long(s));
      fprintf(stderr,"======== writeString = [%s], SSL_state_string_long(s) = [%s]\n", 
          writeString, SSL_state_string_long(s));
    }
  }
  return ;
}

/*

int _tmain(int argc, _TCHAR* argv[])
{
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
	ERR_load_BIO_strings();

	BIO* server;
	BIO* client;
	char buf[256];

	server = BIO_new_accept( "9999" );
	if ( BIO_do_accept(server) <= 0 ) {
		ERR_print_errors_fp(stderr);
		return 0;
	}

	while(1)
	{
		// ��� �������� ���� ������ ��ٸ���.
		printf("������ ��ٸ�\n");
		if (BIO_do_accept(server) <= 0)
		{
			ERR_print_errors_fp(stderr);
			exit(0);
		}
			// ���� �����̷�� ���ٸ� 
			// �ش� ������ ������ �´�.
		client = BIO_pop(server);
		memset(buf, 0x00, 256);

			// �а� ����.
		if (BIO_read(client, buf, 255) <= 0)
		{
			perror("Read Error");
		}
		BIO_write(client, buf, 255);
		BIO_free(client);
	}
	return 0;
}

*/