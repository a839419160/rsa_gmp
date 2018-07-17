#include <cstdio>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <gmp.h>

using namespace std;

#define KEY_LENGTH 2048
#define BASE 16 

struct key_pair
{
    char * n;
    char * d;
    int e;
};

mpz_t * gen_pq();
key_pair * gen_key_pair();
char * encrypt(const char * plain_text, const char * key_n, int key_e);
char * decrypt(const char * cipher_text, const char * key_n, const char * key_d);

int main()
{      
    key_pair * p = gen_key_pair();

    cout << "n = " << p->n << endl << endl;
    cout << "d = " << p->d << endl << endl;
    cout << "e = " << p->e << endl << endl;

    char buf[KEY_LENGTH + 10];
    cout << "limit :" << KEY_LENGTH << endl << endl;
    cin >> buf;

    char * cipher_text = encrypt(buf, p->n, p->e);
    cout << "ciphertext：" << cipher_text << endl << endl;
    char * plain_text = decrypt(cipher_text, p->n, p->d);
    cout << "Plaintext ：" << plain_text << endl << endl;

/*    if(strcmp(buf, plain_text) != 0)
        cout<<"success"<<endl;
    else
        cout<<"failed"<<endl;
*/
    return 0;
}


mpz_t * gen_pq()
{                                     
    gmp_randstate_t grt;                
    gmp_randinit_default(grt);  
    gmp_randseed_ui(grt, time(NULL));   

    mpz_t _p, _q;
    mpz_init(_p);
    mpz_init(_q);

    mpz_urandomb(_p, grt, KEY_LENGTH / 2);       
    mpz_urandomb(_q, grt, KEY_LENGTH / 2);

    mpz_t * result = new mpz_t[2];
    mpz_init(result[0]);
    mpz_init(result[1]);

    mpz_nextprime(result[0], _p);
    mpz_nextprime(result[1], _q);

    mpz_clear(_p);
    mpz_clear(_q);

    return result;  
}


key_pair * gen_key_pair()
{
    mpz_t * _pq= gen_pq();

    mpz_t key_n, key_e, key_r;
    mpz_init(key_n);
    mpz_init(key_r);
    mpz_init_set_ui(key_e, 65537);

    mpz_mul(key_n, _pq[0], _pq[1]);
    mpz_sub_ui(_pq[0], _pq[0], 1);
    mpz_sub_ui(_pq[1], _pq[1], 1);
    mpz_mul(key_r, _pq[0], _pq[1]);

    mpz_t key_d;    
    mpz_init(key_d);
    mpz_invert(key_d, key_e, key_r);

    key_pair * result = new key_pair;

    char * buf_n = new char[KEY_LENGTH + 10];
    char * buf_d = new char[KEY_LENGTH + 10];

    mpz_get_str(buf_n, BASE, key_n);
    result->n = buf_n;
    mpz_get_str(buf_d, BASE, key_d);
    result->d = buf_d;
    result->e = 65537;

    mpz_clear(_pq[0]);
    mpz_clear(_pq[1]);
    mpz_clear(key_n);
    mpz_clear(key_d);
    mpz_clear(key_e);
    mpz_clear(key_r);
    delete []_pq;

    return result;
}

char * encrypt(const char * plain_text, const char * key_n, int key_e)  
{
    mpz_t M, C, n;
    mpz_init_set_str(M, plain_text, BASE); 
    mpz_init_set_str(n, key_n, BASE);
    mpz_init_set_ui(C, 0);

    mpz_powm_ui(C, M, key_e, n);

    char * result = new char[KEY_LENGTH + 10];
    mpz_get_str(result, BASE, C);

    return result;
}

char * decrypt(const char * cipher_text, const char * key_n, const char * key_d)  
{
    mpz_t M, C, n, d;
    mpz_init_set_str(C, cipher_text, BASE); 
    mpz_init_set_str(n, key_n, BASE);
    mpz_init_set_str(d, key_d, BASE);
    mpz_init(M);

    mpz_powm(M, C, d, n);

    char * result = new char[KEY_LENGTH + 10];
    mpz_get_str(result, BASE, M);

    return result;
}


