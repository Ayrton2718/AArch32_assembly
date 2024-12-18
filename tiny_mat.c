/*
 * tiny_mat.c
 *
 *  Created on: 2022/03/17
 *      Author: sena2
 */

#include "tiny_mat.h"

#include <stdio.h>
#include <time.h>

#define __BR  "\n\t"

#ifdef TINY_MAT_DEBUG_ON
void TinyMat_checker(int flg)
{
    // printf("checker\n");
	if(flg == 0)
	{
		while(1) {}
	}
}

void TinyMat_safeSize(TinyMat_t* obj)
{
    // printf("safeSize\n");
	if(TINY_MAT_MAX_COL < obj->col || TINY_MAT_MAX_ROW < obj->row)
	{
		while(1) {}
	}
}

#define TINY_MAT_CHECKER(flg) 	TinyMat_checker(flg)
#define TINY_MAT_SAFE(obj)		TinyMat_safeSize(obj)

#else /*TINY_MAT_DEBUG_ON*/
#define TINY_MAT_MATCH_COL(obj, col)
#define TINY_MAT_MATCH_ROW(obj, row)
#define TINY_MAT_SAFE(obj)
#endif /*TINY_MAT_DEBUG_ON*/

#define TINY_MAT_INDEX(col, row) (col * TINY_MAT_MAX_ROW + row)


void TinyMat_setOne(TinyMat_t* obj, size_t col, size_t row, float value)
{
	TINY_MAT_CHECKER(row <= obj->row);
	TINY_MAT_CHECKER(col <= obj->col);

	obj->mat[TINY_MAT_INDEX(col, row)] = value;
}

float TinyMat_getOne(const TinyMat_t* obj, size_t col, size_t row)
{
	TINY_MAT_CHECKER(row <= obj->row);
	TINY_MAT_CHECKER(col <= obj->col);

	return obj->mat[TINY_MAT_INDEX(col, row)];
}

void TinyMat_show(const TinyMat_t* obj)
{
    for(size_t col = 0; col < obj->col; col++)
    {
        printf(" | ");
        for(size_t row = 0; row < obj->row; row++)
        {
            printf("%8.4f ", obj->mat[TINY_MAT_INDEX(col, row)]);
        }
        printf("|\n");
    }

    printf("\n");
}


void TinyMat_create(TinyMat_t* obj, uint8_t col, uint8_t row)
{
	obj->col = col;
	obj->row = row;

	memset(obj->mat, 0x00, sizeof(obj->mat));

	TINY_MAT_SAFE(obj);
}

void TinyMat_createArray(TinyMat_t* obj, uint8_t col, uint8_t row, float* array)
{
	obj->col = col;
	obj->row = row;

	for(size_t _col = 0; _col < obj->col; _col++)
	{
		memcpy(&obj->mat[_col * TINY_MAT_MAX_ROW], &array[_col * obj->row], sizeof(float) * obj->row);
	}

	TINY_MAT_SAFE(obj);
}

void TinyMat_eye(TinyMat_t* obj, uint8_t col, uint8_t row, float c)
{
	obj->col = col;
	obj->row = row;

	for(size_t _col = 0; _col < col; _col++)
	{
		for(size_t _row = 0; _row < row; _row++)
		{
			if(_col == _row)
			{
				obj->mat[TINY_MAT_INDEX(_col, _row)] = c;
			}else{
				obj->mat[TINY_MAT_INDEX(_col, _row)] = 0;
			}
		}
	}

	TINY_MAT_SAFE(obj);
}

void TinyMat_iden(TinyMat_t* obj, uint8_t n)
{
	TinyMat_eye(obj, n, n, 1);
}


void TinyMat_mult(register TinyMat_t* res, register const TinyMat_t* A, register const TinyMat_t* B)
{
	TINY_MAT_CHECKER(B->col == A->row);
	
	res->col = A->col;
	res->row = B->row;


	// for(size_t _col = 0; _col < res->col; _col++)
	// {
	// 	for(size_t _row = 0; _row < res->row; _row++)
	// 	{
	// 		res->mat[TINY_MAT_INDEX(_col, _row)] = 0;
	// 		for(size_t a_row = 0; a_row < A->row; a_row++)
	// 		{
	// 			res->mat[TINY_MAT_INDEX(_col, _row)] += A->mat[TINY_MAT_INDEX(_col, a_row)] * B->mat[TINY_MAT_INDEX(a_row, _row)];
	// 		}
	// 	}
	// }
	__asm__ (
		"mov r0, #0"__BR
		"mult_loop0:"__BR
		"	mov r1, #0"__BR
		"	mult_loop1:"__BR
		"		mov r3, #0"__BR
		"		mul r4, r0, %[Rmax_row]"__BR
		"		add r4, r1"__BR
		"		add r4, %[Rres], r4, lsl #2"__BR
		"		str r3, [r4]"__BR
		"		vldr.f32 s0, [r4]"__BR

		"		mov r2, #0"__BR
		"		mult_loop2:"__BR
		"			mul r3, r0, %[Rmax_row]"__BR
		"			add r3, r2"__BR
		"			add r3, %[Ra], r3, lsl #2"__BR
		"			vldr.f32 s1, [r3]"__BR

		"			mul r3, r2, %[Rmax_row]"__BR
		"			add r3, r1"__BR
		"			add r3, %[Rb], r3, lsl #2"__BR
		"			vldr.f32 s2, [r3]"__BR

		"			vmla.f32 s0, s1, s2"__BR

		"			add r2, #1"__BR
		"			cmp r2, %[Ra_row]"__BR
		"			bne mult_loop2"__BR
		"		vstr.f32 s0, [r4]"__BR

		"		add r1, #1"__BR
		"		cmp r1, %[R_row]"__BR
		"		bne mult_loop1"__BR
		"	add r0, #1"__BR
		"	cmp r0, %[R_col]"__BR
		"	bne mult_loop0"__BR
		:
		: [Rres] "r" (res->mat), [Ra] "r" (A->mat), [Rb] "r" (B->mat), [R_col] "r" (res->col), [R_row] "r" (res->row), [Ra_row] "r" (res->row), [Rmax_row] "r" (TINY_MAT_MAX_ROW)
		: "r0", "r1", "r2", "r3", "r4", "s0", "s1", "s2"
	);

	TINY_MAT_SAFE(res);
}

void TinyMat_multTransA(TinyMat_t* res, const TinyMat_t* A, const TinyMat_t* B)
{
	TINY_MAT_CHECKER(B->col == A->col);

	TinyMat_t tmp;

	tmp.col = A->row;
	tmp.row = B->row;

	for(size_t _col = 0; _col < tmp.col; _col++)
	{
		for(size_t _row = 0; _row < tmp.row; _row++)
		{
			tmp.mat[TINY_MAT_INDEX(_col, _row)] = 0;
			for(size_t a_col = 0; a_col < A->col; a_col++)
			{
				tmp.mat[TINY_MAT_INDEX(_col, _row)] += A->mat[TINY_MAT_INDEX(a_col, _col)] * B->mat[TINY_MAT_INDEX(a_col, _row)];
			}
		}
	}

	TINY_MAT_SAFE(&tmp);
	memcpy(res, &tmp, sizeof(TinyMat_t));
}

void TinyMat_multTransB(TinyMat_t* res, const TinyMat_t* A, const TinyMat_t* B)
{
	TINY_MAT_CHECKER(B->row == A->row);

	TinyMat_t tmp;

	tmp.col = A->col;
	tmp.row = B->col;

	for(size_t _col = 0; _col < tmp.col; _col++)
	{
		for(size_t _row = 0; _row < tmp.row; _row++)
		{
			tmp.mat[TINY_MAT_INDEX(_col, _row)] = 0;
			for(size_t a_row = 0; a_row < A->row; a_row++)
			{
				tmp.mat[TINY_MAT_INDEX(_col, _row)] += A->mat[TINY_MAT_INDEX(_col, a_row)] * B->mat[TINY_MAT_INDEX(_row, a_row)];
			}
		}
	}

	TINY_MAT_SAFE(&tmp);

	memcpy(res, &tmp, sizeof(TinyMat_t));
}


void TinyMat_multScalor(TinyMat_t* res, const TinyMat_t* A, float c)
{
	res->col = A->col;
	res->row = A->row;

	__asm__ (
		"vldr.f32 s1, [%[Rc]]"__BR
		"mov r0, #0"__BR
		"multi_scalor_loop1:"__BR
		"	mov r1, #0"__BR
		"	multi_scalor_loop2:"__BR
		"		mul r2, r0, %[Rmax_row]"__BR
		"		add r2, r1"__BR
		"		lsl r2, #2"__BR
	
		"		add r3, %[Ra], r2"__BR
		"		vldr.f32 s0, [r3]"__BR
		"		vmul.f32 s0, s0, s1"__BR

		"		add r3, %[Rres], r2"__BR
		"		vstr.f32 s0, [r3]"__BR

		"		add r1, #1"__BR
		"		cmp r1, %[Rrow]"__BR
		"		bne multi_scalor_loop2"__BR
		"	add r0, #1"__BR
		"	cmp r0, %[Rcol]"__BR
		"	bne multi_scalor_loop1"__BR
		:
		: [Rres] "r" (res->mat), [Ra] "r" (A->mat), [Rc] "r" (&c), [Rcol] "r" (A->col), [Rrow] "r" (A->row), [Rmax_row] "r" (TINY_MAT_MAX_ROW)
		: "r0", "r1", "r2", "r3", "s0", "s1"
	);
}


void TinyMat_add(TinyMat_t* res, const TinyMat_t* A, const TinyMat_t* B)
{
	TINY_MAT_CHECKER(B->col == A->col);
	TINY_MAT_CHECKER(B->row == A->row);

	res->col = A->col;
	res->row = A->row;

	__asm__ (
		"mov r0, #0"__BR
		"add_loop1:"__BR
		"	mov r1, #0"__BR
		"	add_loop2:"__BR
		"		mul r2, r0, %[Rmax_row]"__BR
		"		add r2, r1"__BR
		"		lsl r2, #2"__BR
	
		"		add r3, %[Ra], r2"__BR
		"		vldr.f32 s0, [r3]"__BR

		"		add r3, %[Rb], r2"__BR
		"		vldr.f32 s1, [r3]"__BR

		"		vadd.f32 s0, s0, s1"__BR

		"		add r3, %[Rres], r2"__BR
		"		vstr.f32 s0, [r3]"__BR

		"		add r1, #1"__BR
		"		cmp r1, %[Rrow]"__BR
		"		bne add_loop2"__BR
		"	add r0, #1"__BR
		"	cmp r0, %[Rcol]"__BR
		"	bne add_loop1"__BR
		:
		: [Rres] "r" (res->mat), [Ra] "r" (A->mat), [Rb] "r" (&B->mat), [Rcol] "r" (A->col), [Rrow] "r" (A->row), [Rmax_row] "r" (TINY_MAT_MAX_ROW)
		: "r0", "r1", "r2", "r3", "s0", "s1"
	);

	TINY_MAT_SAFE(res);
}


void TinyMat_sub(TinyMat_t* res, const TinyMat_t* A, const TinyMat_t* B)
{
	TINY_MAT_CHECKER(B->col == A->col);
	TINY_MAT_CHECKER(B->row == A->row);

	res->col = A->col;
	res->row = A->row;

	__asm__ (
		"mov r0, #0"__BR
		"sub_loop1:"__BR
		"	mov r1, #0"__BR
		"	sub_loop2:"__BR
		"		mul r2, r0, %[Rmax_row]"__BR
		"		add r2, r1"__BR
		"		lsl r2, #2"__BR
	
		"		add r3, %[Ra], r2"__BR
		"		vldr.f32 s0, [r3]"__BR

		"		add r3, %[Rb], r2"__BR
		"		vldr.f32 s1, [r3]"__BR

		"		vsub.f32 s0, s0, s1"__BR

		"		add r3, %[Rres], r2"__BR
		"		vstr.f32 s0, [r3]"__BR

		"		add r1, #1"__BR
		"		cmp r1, %[Rrow]"__BR
		"		bne sub_loop2"__BR
		"	add r0, #1"__BR
		"	cmp r0, %[Rcol]"__BR
		"	bne sub_loop1"__BR
		:
		: [Rres] "r" (res->mat), [Ra] "r" (A->mat), [Rb] "r" (&B->mat), [Rcol] "r" (A->col), [Rrow] "r" (A->row), [Rmax_row] "r" (TINY_MAT_MAX_ROW)
		: "r0", "r1", "r2", "r3", "s0", "s1"
	);

	TINY_MAT_SAFE(res);
}

// vdiv
void TinyMat_inv(TinyMat_t* res, const TinyMat_t* A)
{
	TINY_MAT_CHECKER(A->col == A->row);

    uint8_t order = A->col;

    // TinyMat_t tmp;
    float tmp[TINY_MAT_MAX_COL * 2 * TINY_MAT_MAX_ROW * 2];
	// for (size_t col = 0; col < order; col++)
    // {
    // 	for (size_t row = 0; row < order; row++)
    //     {
    //         tmp[TINY_MAT_INDEX(col, row)] = A->mat[TINY_MAT_INDEX(col, row)];
    //     }
    // }
	__asm__ (
		"mov r0, #0"__BR
		"inv_setTmp_loop1:"__BR
		"	mov r1, #0"__BR
		"	inv_setTmp_loop2:"__BR
		"		mul r2, r0, %[Rmax_row]"__BR
		"		add r2, r1"__BR
		"		lsl r2, #2"__BR
		"		add r3, %[Ra], r2"__BR
		"		vldr.f32 s0, [r3]"__BR

		"		add r3, %[Rres], r2"__BR
		"		vstr.f32 s0, [r3]"__BR
		
		"		add r1, #1"__BR
		"		cmp r1, %[Rrow]"__BR
		"		bne inv_setTmp_loop2"__BR
		"	add r0, #1"__BR
		"	cmp r0, %[Rcol]"__BR
		"	bne inv_setTmp_loop1"__BR
		:
		: [Rres] "r" (tmp), [Ra] "r" (A->mat), [Rcol] "r" (order), [Rrow] "r" (order), [Rmax_row] "r" (TINY_MAT_MAX_ROW)
		: "r0", "r1", "r2", "r3", "s0", "s1"
	);
	

    for (size_t row = 0; row < order; row++)
    {
        for (size_t col = order; col < 2 * order; col++)
        {
            if (row == col - order)
            {
                tmp[TINY_MAT_INDEX(col, row)] = 1.0;
            }
            else
            {
                tmp[TINY_MAT_INDEX(col, row)] = 0.0;
            }
        }
    }

    for(size_t row = 0; row < order; row++)
    {
        float t = tmp[TINY_MAT_INDEX(row, row)];
        for(size_t col = row; col < 2 * order; col++)
        {
            tmp[TINY_MAT_INDEX(col, row)] /= t;
        }

        for(size_t col = 0; col < order; col++)
        {
            if (row != col)
            {
                t = tmp[TINY_MAT_INDEX(row, col)];
                for (size_t k = 0; k < 2 * order; k++)
                {
                    tmp[TINY_MAT_INDEX(k, col)] -= t * tmp[TINY_MAT_INDEX(k, row)];
                }
            }
        }
    }

    res->col = A->col;
    res->row = A->row;
    for (size_t row = 0; row < order; row++)
    {
        size_t colCounter = 0;
        for (size_t col = order; col < 2 * order; col++)
        {
            res->mat[TINY_MAT_INDEX(colCounter++, row)] = tmp[TINY_MAT_INDEX(col, row)];
        }
    }

	TINY_MAT_SAFE(res);
}


