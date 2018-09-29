#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>

#define USE_OPENMP true

#if(USE_OPENMP)
typedef struct 	Gen_params
		{
			FILE* f_out;
			uint32_t min, max;
		} 
		Gen_params_t;
#endif

static uint8_t cipher_txt[13] = { 0xc7, 0x5a, 0xab, 0xd3,
			     	0x3b, 0x29, 0xfd, 0x21,
			     	0xbb, 0x84, 0xc3, 0x5e, '\0' };
static uint8_t buffer_S[256];

static uint8_t buffer_K[14];	//cannot exceed 12 

static char    buffer_ans[14];

void gen_potential_K(FILE* f_out);

int main(int argc, char * argv[])
{
	remove("output.txt");	
	FILE * fp = fopen("output.txt","a+");
	printf("Running\n");
	gen_potential_K(fp);
	printf("Done\n");
	fclose(fp);
	return 0;
}

#if(USE_OPENMP)
void gen_potential_k(Gen_params_t* params)
{
	params->
#else
 gen_potential_K(FILE* f_out)
{
#endif	
	for(uint32_t key = 0; (key < UINT32_MAX); key++)
	{ 
		memset(buffer_S,0,256);
		memset(buffer_K,0,12);
		memset(buffer_ans,0,13);
		buffer_ans[13] = '\0';
		//KSA
		//gen this S
		for(uint32_t i_ksa = 0; i_ksa < 256; i_ksa++)
		{
			buffer_S[i_ksa] = i_ksa;
		} 
		for(uint32_t i_ksa, j_ksa = 0; i_ksa < 256; i_ksa++)
		{
			j_ksa = ((j_ksa + buffer_S[i_ksa] + (((char *)&key)[((i_ksa)%4)])))%256;
			buffer_S[i_ksa] ^= buffer_S[j_ksa];
			buffer_S[j_ksa] ^= buffer_S[i_ksa];
			buffer_S[i_ksa] ^= buffer_S[j_ksa]; //swapped
/*#if DEBUG
			fflush(stdout);
			printf("i_ksa: %u, j_ksa: %u, buffer_S[i_ksa]: %u, buffer_S[j_ksa]: %u", 
					i_ksa, j_ksa, buffer_S[i_ksa], buffer_S[j_ksa]);
#endif */
		}
		//PRGA
		//gen this K
		for(uint32_t k_elem, i_prga, j_prga, idx_tmp = 0; k_elem < 13; k_elem++)
		{
			i_prga = (i_prga + 1)%256;
			j_prga = (j_prga + (uint32_t)buffer_S[i_prga])%256;
			//printf("i_prga: %i, j_prga: %i", i_prga, j_prga );
			buffer_S[i_prga] ^= buffer_S[j_prga];
			buffer_S[j_prga] ^= buffer_S[i_prga];
			buffer_S[i_prga] ^= buffer_S[j_prga]; //swapped	
			//printf("i_prga: %i, j_prga: %i", (int)i_prga, (int)j_prga );
			buffer_S[i_prga] ^= buffer_S[j_prga];
			idx_tmp = (buffer_S[i_prga] + buffer_S[j_prga])%256;
			buffer_K[k_elem] ^= buffer_S[idx_tmp];
			buffer_ans[k_elem] = (char)((cipher_txt[k_elem]) ^ (buffer_K[k_elem]));
		}
		//buffer_ans[12] = '\0';
		//if((strstr(buffer_ans, "key"))||(strstr(buffer_ans, "KEY")))
		//{
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
