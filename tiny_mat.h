/*
 * tiny_mat.h
 *
 *  Created on: Mar 16, 2022
 *      Author: tsuna
 */

#ifndef INC_TINY_MAT_H_
#define INC_TINY_MAT_H_

#include <stddef.h>
#include <stdint.h>
#include <memory.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TINY_MAT_MAX_COL (7)
#define TINY_MAT_MAX_ROW (7)

#define TINY_MAT_DEBUG_ON


typedef struct
{
	uint8_t col;
	uint8_t row;
	float mat[TINY_MAT_MAX_COL * TINY_MAT_MAX_ROW];
} TinyMat_t;


void TinyMat_setOne(TinyMat_t* obj, size_t col, size_t row, float value);
float TinyMat_getOne(const TinyMat_t* obj, size_t col, size_t row);

void TinyMat_show(const TinyMat_t* obj);


void TinyMat_create(TinyMat_t* obj, uint8_t col, uint8_t row);

void TinyMat_createArray(TinyMat_t* obj, uint8_t col, uint8_t row, float* array);

void TinyMat_eye(TinyMat_t* obj, uint8_t col, uint8_t row, float c);

void TinyMat_iden(TinyMat_t* obj, uint8_t n);


void TinyMat_mult(TinyMat_t* res, const TinyMat_t* A, const TinyMat_t* B);

void TinyMat_multTransA(TinyMat_t* res, const TinyMat_t* A, const TinyMat_t* B);

void TinyMat_multTransB(TinyMat_t* res, const TinyMat_t* A, const TinyMat_t* B);

void TinyMat_multScalor(TinyMat_t* res, const TinyMat_t* A, float c);


void TinyMat_add(TinyMat_t* res, const TinyMat_t* A, const TinyMat_t* B);


void TinyMat_sub(TinyMat_t* res, const TinyMat_t* A, const TinyMat_t* B);


void TinyMat_inv(TinyMat_t* res, const TinyMat_t* A);

#ifdef __cplusplus
}
#endif

#endif /* INC_TINY_MAT_H_ */
