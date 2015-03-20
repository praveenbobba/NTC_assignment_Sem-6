#include<stdio.h>
#include"des.h"

#define DES_ENCRYPT 1
#define DES_DECRYPT 0
#define DES_KEY_SIZE 8
   int ret;
   unsigned char key[DES_KEY_SIZE];
   unsigned char input[100],output[100],input2[300],output2[100],user[100];
   des_context context;


void orenc()
{
     ret = des_setkey_enc (&context,key);
   ret = des_crypt_ecb(&context , input,output);
}

void ordec()
{

    ret = des_setkey_dec (&context,key);
   ret = des_crypt_ecb(&context ,input,output);
}

void enc()
{
  //output2[0] = '\0';
  int p=0,q,size,j=0,k=0,i=0;
  size = strlen(input2);
 /* if(size<=16)
  {	
	
	strcpy(input,input2);
	orenc();
	output[16] = '/';
	strcpy(output2,output);
  }*/

  //else
  //{
    	
    	if(size % 8 !=0 )
		j = 1;
    	for(p=0;p<(size/8)+j  && (input2[8*p] !='*');p++)
    	{
		for(q=(8*p),i=0; q< (8 * (p+1) ) && (q<=size) ;q++,i++)
		{
			input[i] = input2[q];
			//printf("%c %c\n",input[i],input2[q]);
		}
		//input[i]='\0';
		orenc();
		for(k=0;k<8;k++)
		{
			output2[(8*p)+k] = output[k];
		}
		//strcpy(input,output);
		//ordec();
		//printf("%s",output);
        	
    	}
	output2[8*p] = '*' ;

  }



//}

void dec()
{
	//output2[0]='\0' ;
  	int p=0,q,size,j=0,k,i=0;
    	for(p=0;input2[8*p] !='*' ;p++)
    	{
		for(q=(8*p),i=0; q< (8 * (p+1) ) ;++q,++i)
		{
			input[i] = input2[q];
			//printf("%c %c\n",input[i],input2[q]);
		}
		ordec();
		//printf("%s\n",output);
		for(k=0;k<8;k++)
		{
			output2[(8*p)+k] = output[k];
		}
        	
    	}
	//output2[(8*p)]='\0';	
}

int main()
{
   int i;
   printf("enter the data\n");
   //getchar();
   scanf("%s",input2);
   strcpy(key,"pjnredco");
   des_init(&context) ;
   des_key_set_parity(key);
   ret =  des_key_check_key_parity(key);
   ret = des_key_check_weak(key);

    enc();
   for(i=0;output2[i] != '*';i++)
	user[i] = output2[i];
   user[i] = output2[i];

   strcpy(key,"bhideock");

   for(i=0;user[i] != '*';i++)
	input2[i] = user[i];
   input2[i] = user[i];
    enc();
   for(i=0;output2[i] != '*';i++)
	user[i] = output2[i];
   user[i] = output2[i]; 


   for(i=0;user[i] != '*';i++)
	input2[i] = user[i];
   input2[i] = user[i];

    dec();
   for(i=0;output2[i] != '*';i++)
	user[i] = output2[i];
   user[i] = output2[i];
 strcpy(key,"pjnredco");

   for(i=0;user[i] != '*';i++)
	input2[i] = user[i];
   input2[i] = user[i];

    dec();
  strcpy(user,output2);

   printf("%s\n",user);
   return 0;
}
