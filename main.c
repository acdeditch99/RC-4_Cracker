//Standard
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

//Unix
#include <string.h>
#include <signal.h>

//OpenMP
//#include <omp.h>

typedef struct 	Gen_Params
		{
			FILE * f_out;
			uint32_t min, max; //both min and max are inclusive
			uint8_t buf_S[256];
			uint8_t buf_K[14];
			char    buf_a[14];
		} 
		Gen_Params_t;

typedef enum 	Gen_Err 
		{
			No_Err,
			KSA_Err,
			PRGA_Err,
			File_Err,
			Print_Err
		} 
		Gen_Err_t;

static const uint8_t cipher_txt[13] = { 0xc7, 0x5a, 0xab, 0xd3,
			     	0x3b, 0x29, 0xfd, 0x21,
			     	0xbb, 0x84, 0xc3, 0x5e, '\0' };
/*
static uint8_t buffer_S[256];

static uint8_t buffer_K[14];	//cannot exceed 12 

static char    buffer_ans[14];
*/
Gen_Err_t gen_potential_K(Gen_Params_t * const params);

Gen_Err_t gen_potential_K_ksa(Gen_Params_t * const params);

Gen_Err_t gen_potential_K_prga(Gen_Params_t * const params);

int main(int argc, char * argv[])
{
	remove("output.txt");	
	FILE * fp = fopen("output.txt","a+");
#ifdef _OPENMP
	uint8_t p_cnt = 0;///do something with this
	char * intermediate_file_out_str[15] = "output_x.txt\0";
	FILE * * fp_intermediate_arr = malloc(sizeof(FILE * *)((size_t)p_cnt+1));
	
	for(uint32_t fp_arr_idx; fp_arr_idx < (p_cnt-1); fp_arr_idx++, intermediate_file_out_str[7]++)
	{
		fp_intermediate_arr[fp_arr_idx] = fopen(intermediate_file_out_str, "a+");
		
	}
	fp_intermediate_arr[p_cnt-1] = NULL;
#else
	FILE * fp = fopen("output.txt","a+");
#endif 
	printf("Running\n");

#ifdef _OPENMP
	for()
#else
	Gen_Params_t * param_i = malloc(sizeof(Gen_Params_t);
	param_i->f_out = fp;
	param_i->min = 0;
	param_i->max = UINT32_MAX;
#endif
	memset((param_i->buf_S), 0, 256);
	memset((param_i->buf_K), 0, 14);
	memset((param_i->buf_a), 0, 14);
	gen_potential_K(param_i);
#ifdef _OPENMP
	for(;fp_intermediate_arr ; fclose(*(fp_intermediate_arr++)), remove(*(fp_intermediate_arr++));
#endif
	printf("Done\n");
	fclose(fp);
	return 0;
}

Gen_Err_t gen_potential_k(Gen_Params_t * const params)
{
	Gen_Err_t err = No_Err;
	for(uint32_t key = key_min; (key < key_max); key++)
	{ 
		params->buf_a[13] = '\0';
		if(err = gen_potential_K_ksa(params))  return err;
		if(err = gen_potential_K_prga(params)) return err;
		if(err = file_write_stage(NULL, NULL)) return err; //don't leave this like this
	}
	return err;
}

Gen_Err_t gen_potential_K_ksa(Gen_Params_t * const params)
{
	char * const buf_S_loc = buf_S;
	const uint32_t key_loc = key;
	uint8_t * buffer_S = params->buf_S;	
	Gen_Err_t err = No_Err;
	//KSA
	//gen this S
	for(uint32_t i_ksa = 0; i_ksa < 256; i_ksa++)
	{
		buf_S_loc[i_ksa] = i_ksa;
	} 
	for(uint32_t i_ksa, j_ksa = 0; i_ksa < 256; i_ksa++)
	{
		j_ksa = ((j_ksa + buf_S_loc[i_ksa] + (((char *)&(key_loc))[((i_ksa)%4)])))%256;
		buf_S_loc[i_ksa] ^= buf_S_loc[j_ksa];
		buf_S_loc[j_ksa] ^= buf_S_loc[i_ksa];
		buf_S_loc[i_ksa] ^= buf_S_loc[j_ksa]; //swapped
	}
	return err;
	
}

Gen_Err_t gen_potential_K_prga(Gen_Params_t * const params)
{
	char * const buf_S_loc = params->buf_S;
	char * const buf_K_loc = params->buf_K; 
	char * const buf_ans_loc = params->buf_a;
	Gen_Err_t err = No_Err;
	//PRGA
	//gen this K
	for(uint32_t k_elem, i_prga, j_prga, idx_tmp = 0; k_elem < 13; k_elem++)
	{
		i_prga = (i_prga + 1)%256;
		j_prga = (j_prga + (uint32_t)buffer_S[i_prga])%256;
		buf_S_loc[i_prga] ^= buf_S_loc[j_prga];
		buf_S_loc[j_prga] ^= buf_S_loc[i_prga];
		buf_S_loc[i_prga] ^= buf_S_loc[j_prga]; //swapped	
		buf_S_loc[i_prga] ^= buf_S_loc[j_prga];
		idx_tmp = (buf_S_loc[i_prga] + buf_S_loc[j_prga])%256;
		buf_K_loc[k_elem] ^= buf_S_loc[idx_tmp];
		buf_ans_loc[k_elem] = (char)((cipher_txt[k_elem]) ^ (buf_K_loc[k_elem]));
	}	
	return err;
}

Gen_Err_t file_write_stage(FILE * const f_out_arg, char ** arr_buf_ans)
{
	const char key_resolution_f_str[40] = "\n\nUsing key: %0i, K resolves to: \0";
	const char ans_resolution_f_str[40] = "\nWhile Answer resolves to %s\n\0";
	Gen_Err_t err = No_Err;
	f_out = f_out_arg;
	for(int i = 0; arr_buf_ans[i]; i++)
	{
		fprintf(f_out, key_resolution_f_str, key);
		for(int str_idx = 0; str_idx < 13; str_idx++)
		{
			fprintf(f_out,'%i', buffer_K[str_idx]);
		}
		fprintf(f_out, ans_resolution_f_str, buffer_ans);
		if((strstr(buffer_ans, "KEY"))||(strstr(buffer_ans, "key")||(strstr(buffer_ans, "Key"))))
		{
			printf("Key: %0i,\t Possible Result: %s\n", key, buffer_ans);
		}
	}
	return err;

}
