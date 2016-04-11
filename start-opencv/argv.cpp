# include <stdio.h>
int argv(int argc, char** argv){
	int i = 0;
	while (i < argc){
		printf("%s\n", argv[i]);
		i++;
	}
	getchar();
	return 0;
}