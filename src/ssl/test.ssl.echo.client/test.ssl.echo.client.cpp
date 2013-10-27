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
    int sd;
    struct sockaddr_in sa;

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
	
	sd = socket (AF_INET, SOCK_STREAM, 0);
    CHK_ERR(sd, "socket");

    memset (&sa, '\0', sizeof(sa));
    sa.sin_family      = AF_INET;
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");
    sa.sin_port        = htons (7543);          /* Server Port number */

    err = connect(sd, (struct sockaddr*) &sa, sizeof (sa));
	
	ssl = SSL_new( ctx );
	BIO* bio_recv = BIO_new( BIO_s_mem());
	BIO* bio_send = BIO_new( BIO_s_mem());
	SSL_set_bio( ssl , bio_recv , bio_send );
	SSL_set_connect_state( ssl );

	char* t_buf = new char[1024758];
	int r;
	r = SSL_read( ssl , nullptr , 0 );
	while ( true ) {
		
		printf( "read %d" , r );
		if( SSL_is_init_finished(ssl)){
			printf("Init finished\n");
			break;
		}
		while ( BIO_ctrl_pending( bio_send )){
			int bytes = BIO_read( bio_send , t_buf , 1 );
			if ( bytes > 0 ) {
				send( sd , t_buf , bytes , 0 );
			} else {
				break;
			}
		}
		r = recv( sd , t_buf , 1024768 , 0 );
		BIO_write( bio_recv , t_buf , r );
		r = SSL_read( ssl , t_buf , 1024768 );
	}

	client_cert = SSL_get_peer_certificate (ssl);


    if (client_cert != NULL)
    {
        printf ("Server certificate:\n");

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

	int len = SSL_write( ssl , t_buf , 5 );
	while ( BIO_ctrl_pending( bio_send )){
		int bytes = BIO_read( bio_send , t_buf , 1024768 );
		if ( bytes > 0 ) {
			send( sd , t_buf , bytes , 0 );
		}
	}

	char* recv_buffer = new char[10247680];
	int recv_size = 0;

	while ( true ) {
		r = recv( sd , recv_buffer + recv_size , 4096 , 0 );
		printf( "Socket read %d\n" , r );
		if ( r > 0 ) {
			recv_size += r;
			int wr = BIO_write( bio_recv , recv_buffer , recv_size );
			if ( wr < 0 && !BIO_should_retry( bio_recv )) {
				printf("Error\n");
				return 0;
			} 
			if ( wr <= 0 ) {
				if ( !BIO_should_retry( bio_recv )) {
					printf("Error1\n");
					return 0;
				}else{
					printf( "Cont\n");
				}
			} else {
				if ( wr != recv_size ) {
					memmove( recv_buffer , recv_buffer + wr , recv_size - wr );
					recv_size -= wr;
					printf( "Remain %d\n" , recv_size );
				}else{
					printf( "No Reamin\n");
					recv_size = 0;
				}
				while ( true ) {
					r = SSL_read( ssl , t_buf , 1 );
					printf( "SSLread %d\n" , r );
					if ( r <= 0 )
						break;
				}
				
				
			}
		}
	}
	
	//BIO_write( bio_send , "I Want You" , strlen( "I Want You" ));




















	SSL_CTX_set_default_passwd_cb( ctx , &pem_password );
    // SSL �ڵ彦��ũ �޽�����ȯ ������ �˷��ִ� �ݺ��Լ�
    SSL_CTX_set_info_callback(ctx,ssl_info_callback);


    sd = socket (AF_INET, SOCK_STREAM, 0);
    CHK_ERR(sd, "socket");

    memset (&sa, '\0', sizeof(sa));
    sa.sin_family      = AF_INET;
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");
    sa.sin_port        = htons (1111);          /* Server Port number */

    err = connect(sd, (struct sockaddr*) &sa, sizeof (sa));
    CHK_ERR(err, "bind");

    ssl = SSL_new (ctx);
    CHK_NULL(ssl);

    // connect the SSL object with a file descriptor
    // ����� ���ϰ� SSL���� ����
    SSL_set_fd (ssl, sd);
    // ���� �߿��� �Լ�, Ŭ���̾�Ʈ���� �ʱ� �������, �� �ڵ彦��ũ ������ ����
    printf ("SSL_connect start =========================\n");
	err = SSL_connect(ssl);
    CHK_SSL(err);
    printf ("SSL_connect end =========================\n");

    // Get the cipher - opt
    // ���� Ŭ���̾�Ʈ�� ���ǵ� ��ȣȭ �Ķ���������� ����
    printf ("SSL connection using %s\n", SSL_get_cipher (ssl));
    printf ("SSL connection using %s\n", SSL_CIPHER_get_name(SSL_get_current_cipher(ssl)));

    // Get client's certificate (note: beware of dynamic allocation) - opt
    client_cert = SSL_get_peer_certificate (ssl);


    if (client_cert != NULL)
    {
        printf ("Server certificate:\n");

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
    

	err = SSL_write (ssl, "I want you.", strlen("I want you."));
	CHK_SSL(err);

    err = SSL_read (ssl, buf, sizeof(buf) - 1);
    CHK_SSL(err);

    buf[err] = '\0';
    // ���� �����͸� ȭ�鿡 ǥ��
    printf ("Got %d chars:'%s'\n", err, buf);

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