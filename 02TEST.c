#include<stdio.h>
#include<time.h>
#include<stdlib.h>
int** init_mat()
{
	int** matrice;
	matrice = calloc(12,sizeof(int*));
	for(int i = 0; i < 12; i ++)
		matrice[i] = calloc(12,sizeof(int));
	return matrice;
}
void nav_gen_OR(int** matrice,int** interzis,int len)
{
	int i,j;
	int diry = rand()%10 + 1;
	int dirx = rand()%7 + 1;//(10 - len - 11)
	for( j = dirx; j < dirx + len ;j++ )
	{
		matrice[diry][j] = 1;
		interzis[diry][j] = 1;
		interzis[diry-1][j] = 2;
		interzis[diry+1][j] = 2;

	}
	interzis[diry][dirx-1] = 2;
	interzis[diry][j] = 2;
	interzis[diry+1][j] = 2;
	interzis[diry-1][j] = 2;
	interzis[diry+1][dirx-1] = 2;
	interzis[diry-1][dirx-1] = 2;	
}
void nav_gen_VR(int** matrice,int** interzis,int len)
{
	int i,j;
	int dirx = rand()%10 + 1;
	int diry = rand()%7 + 1;//(10 - len - 11)
	for( i = diry; i < diry + len ;i++ )
	{		
		matrice[i][dirx] = 1;
		interzis[i][dirx] = 1;
		interzis[i][dirx-1] = 2;
		interzis[i][dirx+1] = 2;
	}
	interzis[diry-1][dirx] = 2;
	interzis[i][dirx] = 2;
	interzis[i][dirx+1] = 2;
	interzis[i][dirx-1] = 2;
	interzis[diry-1][dirx+1] = 2;
	interzis[diry-1][dirx-1] = 2;	
}
int main()
{
	int** matrice;
	int** interzis = init_mat();
	int i,j;
	srand(time(0)); 
	matrice = init_mat();
	int dir_rand = rand()%2;
	int len = 4;//lungimea navei
	while(index_nave)//struct de nave, fiecare cu lungimea ei,fiecare cate sunt
	{	
		if(dir_rand == 1)//orizontal
		{
			nav_gen_OR(matrice,interzis,len);
		}
		else//vertical
		{
			nav_gen_VR(matrice,interzis,len);
		}
	}
	for(i = 1; i <= 10; i++)
	{
		for(j = 1 ;j <= 10; j ++)
			printf("%d",matrice[i][j]);

		printf("\n");
	}
	printf("\n");
	for(i = 1; i <= 10; i++)
	{
		for(j = 1 ;j <= 10; j ++)
			printf("%d",interzis[i][j]);

		printf("\n");
	}
}