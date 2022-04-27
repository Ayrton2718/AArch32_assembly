/*
 * tiny_mat.c
 *
 *  Created on: 2022/03/17
 *      Author: sena2
 */

#include "tiny_mat.h"

#include <stdio.h>

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


void TinyMat_setOne(TinyMat_t* obj, size_t col, size_t row, float value)
{
	TINY_MAT_CHECKER(row <= obj->row);
	TINY_MAT_CHECKER(col <= obj->col);

	obj->mat[col][row] = value;
}

float TinyMat_getOne(const TinyMat_t* obj, size_t col, size_t row)
{
	TINY_MAT_CHECKER(row <= obj->row);
	TINY_MAT_CHECKER(col <= obj->col);

	return obj->mat[col][row];
}


void TinyMat_show(const TinyMat_t* obj)
{
    for(size_t col = 0; col < obj->col; col++)
    {
        printf(" | ");
        for(size_t row = 0; row < obj->row; row++)
        {
            printf("%8.4f ", obj->mat[col][row]);
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
		memcpy(obj->mat[_col], &array[_col * obj->row], sizeof(float) * obj->row);
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
				obj->mat[_col][_row] = c;
			}else{
				obj->mat[_col][_row] = 0;
			}
		}
	}

	TINY_MAT_SAFE(obj);
}

void TinyMat_iden(TinyMat_t* obj, uint8_t n)
{
	TinyMat_eye(obj, n, n, 1);
}


void TinyMat_mult(TinyMat_t* res, const TinyMat_t* A, const TinyMat_t* B)
{
	TINY_MAT_CHECKER(B->col == A->row);

	TinyMat_t tmp;

	tmp.col = A->col;
	tmp.row = B->row;

	for(size_t _col = 0; _col < tmp.col; _col++)
	{
		for(size_t _row = 0; _row < tmp.row; _row++)
		{
			__asm__ (
				"	vmov.f32 s0, #0"__BR
				"loop:"__BR
				"	vldr.f32 s1, [%[Rap], #0x04]!"__BR
				"	ldr r0 [%%[Rbp], #0]"__BR
				// "	vldr.f32 s2, [%[Rbp], #0x04]!"__BR

				// "vldr.f32 s1, [%[Rap], #0]!"__BR
				// "vldr.f32 s2, [%[Rbp], #0]"__BR
				"vmla.f32 s0, s1, s2"__BR
				"vstr.f32 s0, [%[Rtp], #0]"__BR
				:
				: [Rap] "r" (A->mat[_col]), [Rbp] "r" (B->mat), [Rtp] "r" (&tmp.mat[_col][_row])
				: "r0", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7"
			);

			// tmp.mat[_col][_row] = 0;
			// for(size_t a_row = 0; a_row < A->row; a_row++)
			// {
			// }
		}
	}

	TINY_MAT_SAFE(&tmp);

	memcpy(res, &tmp, sizeof(TinyMat_t));
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
			tmp.mat[_col][_row] = 0;
			for(size_t a_row = 0; a_row < A->col; a_row++)
			{
				tmp.mat[_col][_row] += A->mat[a_row][_col] * B->mat[a_row][_row];
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
			tmp.mat[_col][_row] = 0;
			for(size_t a_row = 0; a_row < A->row; a_row++)
			{
				tmp.mat[_col][_row] += A->mat[_col][a_row] * B->mat[_row][a_row];
			}
		}
	}

	TINY_MAT_SAFE(&tmp);

	memcpy(res, &tmp, sizeof(TinyMat_t));
}


void TinyMat_multScalor(TinyMat_t* res, const TinyMat_t* A, float c)
{
	for(size_t _col = 0; _col < res->col; _col++)
	{
		for(size_t _row = 0; _row < res->row; _row++)
		{
			res->mat[_col][_row] = A->mat[_col][_row] * c;
		}
	}
}


void TinyMat_add(TinyMat_t* res, const TinyMat_t* A, const TinyMat_t* B)
{
	TINY_MAT_CHECKER(B->col == A->col);
	TINY_MAT_CHECKER(B->row == A->row);

	res->col = A->col;
	res->row = A->row;

	for(size_t _col = 0; _col < res->col; _col++)
	{
		for(size_t _row = 0; _row < res->row; _row++)
		{
			res->mat[_col][_row] = A->mat[_col][_row] + B->mat[_col][_row];
		}
	}

	TINY_MAT_SAFE(res);
}


void TinyMat_sub(TinyMat_t* res, const TinyMat_t* A, const TinyMat_t* B)
{
	TINY_MAT_CHECKER(B->col == A->col);
	TINY_MAT_CHECKER(B->row == A->row);

	res->col = A->col;
	res->row = A->row;

	for(size_t _col = 0; _col < res->col; _col++)
	{
		for(size_t _row = 0; _row < res->row; _row++)
		{
			res->mat[_col][_row] = A->mat[_col][_row] - B->mat[_col][_row];
		}
	}

	TINY_MAT_SAFE(res);
}


void TinyMat_inv(TinyMat_t* res, const TinyMat_t* A)
{
	TINY_MAT_CHECKER(A->col == A->row);

    uint8_t order = A->col;

    // TinyMat_t tmp;
    float tmp[TINY_MAT_MAX_COL * 2][TINY_MAT_MAX_ROW * 2];

    for (size_t row = 0; row < order; row++)
    {
        for (size_t col = 0; col < order; col++)
        {
            tmp[col][row] = A->mat[col][row];
        }
    }

    for (size_t row = 0; row < order; row++)
    {
        for (size_t col = order; col < 2 * order; col++)
        {
            if (row == col - order)
            {
                tmp[col][row] = 1.0;
            }
            else
            {
                tmp[col][row] = 0.0;
            }
        }
    }

    for(size_t row = 0; row < order; row++)
    {
        float t = tmp[row][row];
        for(size_t col = row; col < 2 * order; col++)
        {
            tmp[col][row] /= t;
        }

        for(size_t col = 0; col < order; col++)
        {
            if (row != col)
            {
                t = tmp[row][col];
                for (size_t k = 0; k < 2 * order; k++)
                {
                    tmp[k][col] -= t * tmp[k][row];
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
            res->mat[colCounter++][row] = tmp[col][row];
        }
    }

	TINY_MAT_SAFE(res);
}


