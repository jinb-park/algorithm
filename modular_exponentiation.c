#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long long ULL;
ULL multiply_cnt = 0;

/* a^b mod c */

/**
 * modular_ex_0
 *
 * (a*a*a*a..... (b)) mod c
 */
ULL modular_ex_0(ULL a, ULL b, ULL c)
{
	ULL r, i;

	r = 1;
	for(i=0; i<b; i++) {
		r *= a;
		multiply_cnt++;
	}
	r %= c;
	return r;
}

/**
 * modular_ex_1
 *
 * a^3 mod c ==> (a*a*a) mod c ==> ((a mod c)*(a mod c)*(a mod c)) mod c
 */
ULL modular_ex_1(ULL a, ULL b, ULL c)
{
	ULL r, i;

	r = 1;
	for(i=0; i<b; i++) {
		r = (r * (a % c)) % c;
		multiply_cnt++;
	}
	return r;
}

/**
 * modular_ex_divide_and_conquer
 *
 */
ULL modular_ex_divide_and_conquer(ULL a, ULL b, ULL c)
{
	ULL r1, r2;

	multiply_cnt++;

	/* base */
	if(b == 1)
		return a % c;

	if(b & 1 == 1) {
		/* divide */
		r1 = a % c;
		r2 = modular_ex_divide_and_conquer(a, b-1, c);

		/* merge */
		return (r1 * r2) % c;
	}

	/* divide */
	r1 = modular_ex_divide_and_conquer(a, b/2, c);

	/* merge */
	return (r1 * r1) % c;
}

ULL get_k_bit(ULL b)
{
	ULL r;
	int i = sizeof(ULL);

	for(; i>=0; i--) {
		r = 0;
		r |= (1 << i);

		if(r <= b)
			break;
	}

	return i + 1;
}

/**
 * modular_ex_binary
 *
 * this algorithm is based on below document.
 * - ftp://ftp.rsasecurity.com/pub/pdfs/tr201.pdf
 *   -- 2.3. The Binary Method
 *
 * The principle of this method is same to divide and conquer.
 * Only difference is that dac use recursvie, binary method doesn't use recursive.
 */
ULL modular_ex_binary(ULL a, ULL b, ULL c)
{
	ULL k = get_k_bit(b);	/* b == k bits */
	ULL r, i;

	/* 1 */
	if( (b & (1 << k-1)) )
		r = a;
	else
		r = 1;

	/* 2 */
	i = k-2;
	while(1) {
		multiply_cnt++;

		r = (r * r) % c;		/* 2a */
		if( (b & (1 << i)) ) {	/* 2b */
			multiply_cnt++;
			r = (r * a) % c;
		}

		if(i == 0)
			break;
		i--;
	}

	/* 3 */
	return r;
}

/**
 * modular_ex_m_ary (== fixed window exponentiation)
 *
 * this algorithm is based on below document.
 * - ftp://ftp.rsasecurity.com/pub/pdfs/tr201.pdf
 *   -- 2.4. The m-ary Method
 */
ULL m_arr[32] = {0,};

static inline unsigned int get_idx(ULL b, int s, int r)
{
	ULL idx = 0;

	idx = (b << (64 - s - 1));
	idx = (idx >> (64 - r));
	return (unsigned int)idx;
}

ULL modular_ex_m_ary(ULL a, ULL b, ULL c, ULL m)
{
	ULL r = get_k_bit(m) - 1;
	ULL k = get_k_bit(b);
	ULL res, mul;
	unsigned int idx;
	int i, j, s;

	/* precompute */
	m_arr[0] = 1;
	for(i=1; i<m; i++)
		m_arr[i] = (m_arr[i-1] * a);

	/* padding is needed?? */
	s = k-1;
	if( (k % r) != 0 )
		s += (k % r);

	/* 1st */
	idx = get_idx(b, s, r);
	res = m_arr[idx] % c;
	multiply_cnt++;

	/* remains */
	i = s - r;
	while(1) {
		res = (res * res * res * res) % c;
		multiply_cnt++;

		idx = get_idx(b, i, r);
		if(idx > 0) {
			res = (res * m_arr[idx]) % c;
			multiply_cnt++;
		}

		i -= r;
		if(i <= 0)
			break;
	}

	return res;
}

int main()
{
	ULL a = 2, b = 60, c = 200, r;

	printf("a : %lld, b : %lld, c : %lld\n", a, b, c);

	/* ex0 */
	multiply_cnt = 0;
	r = modular_ex_0(a, b, c);
	printf("modular_ex_0 : %lld, multiply_cnt : %lld\n", r, multiply_cnt);

	/* ex1 */
	multiply_cnt = 0;
	r = modular_ex_1(a, b, c);
	printf("modular_ex_1 : %lld, multiply_cnt : %lld\n", r, multiply_cnt);

	/* divide and conquer */
	multiply_cnt = 0;
	r = modular_ex_divide_and_conquer(a, b, c);
	printf("modular_ex_dac : %lld, multiply_cnt : %lld\n", r, multiply_cnt);

	/* binary method used for RSA */
	multiply_cnt = 0;
	r = modular_ex_binary(a, b, c);
	printf("modular_ex_binary : %lld, multiply_cnt : %lld\n", r, multiply_cnt);

	/* m-ary method used for RSA */
	multiply_cnt = 0;
	r = modular_ex_m_ary(a, b, c, 4);
	printf("modular_ex_m_ary : %lld, multiply_cnt : %lld\n", r, multiply_cnt);

	return 0;
}