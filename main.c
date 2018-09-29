//Standard
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

//Unix
#include <string.h>
#include <signal.h>

//OpenMP
#include <omp.h>

typedef struct 	Gen_Params
		{
			FILE * f_out;
			uint32_t min, max; //both min and max are inclusive
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
static uint8_t buffer_S[256];

static uint8_t buffer_K[14];	//cannot exceed 12 

static char    buffer_ans[14];

Gen_Err_t gen_potential_K(Gen_Params_t * params);

Gen_Err_t gen_potential_K_ksa(char * buf_S, const uint32_t key);

Gen_Err_t gen_potential_K_prga(char * buf_S, char * buf_K);

int main(int argc, char * argv[])
{
	remove("output.txt");	
	FILE * fp = fopen("output.txt","a+");
#ifdef _OPENMP
	uint8_t p_cnt = 0;///do something with this
	char * intermediate_file_out_str[15] = "output_x.txt\0";
	FILE * * fp_intermediate_arr = malloc(sizeof(FILE * *)((size_t)p_cnt+1))
	for(uint32_t fp_arr_idx; fp_arr_idx < (p_cnt-1); fp_arr_idx++)
	{
		intermediate_file_out_str[7]++;
		fp_intermediate_arr[fp_arr_idx] = fopen(intermediate_file_out_str, "a+");
		
	}
	fp_intermediate_arr[p_cnt-1] = NULL;
#endif 
	printf("Running\n");
	gen_potential_K(param);
#ifdef _OPENMP
	for(; fp_intermediate_arr != NULL ; fclose(*(fp_intermediate_arr++)));
#endif
	printf("Done\n");
	fclose(fp);
	return 0;
}

Gen_Err_t gen_potential_k(Gen_Params_t * const params)
{
	Gen_Err_t first_err = No_Err;
	const uint32_t key_max = params->max;
	const uint32_t key_min = params->min;
	for(uint32_t key = key_min; (key < key_max); key++)
	{ 
		memset(buffer_S,0,256);
		memset(buffer_K,0,12);
		memset(buffer_ans,0,13);
		buffer_ans[13] = '\0';
		gen_potential_K_ksa(buffer_S, (const uint32_t)key);
		fflush(f_out);
		fprintf(f_out, "\nUsing key: %0i, K resolves to: ", key);
		for(int str_idx = 0; str_idx < 13; str_idx++)
		{
			fprintf(f_out,"%i", buffer_K[str_idx]);
		}
		fprintf(f_out, "\tWhile Answer resolves to: %s\n", buffer_ans);
		if((strstr(buffer_ans, "KEY"))||(strstr(buffer_ans, "key")||(strstr(buffer_ans, "Key"))))
		{
			printf("Key: %0i,\t Possible Result: %s\n", key, buffer_ans);
		}
	}
}

Gen_Err_t gen_potential_K_ksa(char * const buf_S, const uint32_t key)
{
	char * const buf_S_loc = buf_S;
	const uint32_t key_loc = key;
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

Gen_Err_t gen_potential_K_prga(char * const buf_S, char * const buf_K, char * const buf_ans)
{
	char * const buf_S_loc = buf_S;
	char * const buf_K_loc = buf_K; 
	char * const buf_ans_loc = buf_ans;
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


