#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int binaire(int x){
  int a=1;
  int b=0;
  while(x>0){
int c=x%16;
    x=x/16;
    a=a*10;

    x=x/16;
    b=b+c*a;
    a=a*10;}
  
  return b;
}

int main(int argc, char *argv[])

{

    FILE* fichier = NULL;
    int width;
    int height;
    fichier = fopen("map_1", "r");
    int i;
    int j;
    if (fichier != NULL)
    {
        fscanf(fichier, "%d x %d", &width, &height);
	int score[width][height];
	for(i=0; i<height; i++){
	  printf("\n");
	  for(j=0; j<width; j++){
	    fscanf(fichier, "%d ", &score[j][i]);
	    score[j][i]=binaire(score[j][i]);
	    printf("%d \t", score[j][i]);}}
        fclose(fichier);

    }

 

    return 0;

}
