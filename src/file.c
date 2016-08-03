
//based off VitaShell code by  TheFloW
#include "file.h"
int ReadFile(char *file, void *buf, int size) {
	SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
	if (fd < 0)
	return fd;

	int read = sceIoRead(fd, buf, size);

	sceIoClose(fd);
	return read;
}

int WriteFile(char *file, void *buf, int size) {
	SceUID fd = sceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fd < 0)
	return fd;

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}
void saveScore(int score) {
	char buf[10];
	sprintf(buf, "%d", score);
	int size = sizeof(score); 
	int ret = WriteFile("ux0:/data/psp2048/highscore.txt", buf, size); 

	if(ret<0) {
		sceIoMkdir("ux0:/data/psp2048/", 0777);
		int ret = WriteFile("ux0:/data/psp2048/highscore.txt", buf, size); 
	}
	
}
int loadScore() {
	char buf[10];
	int output;
	int ret = ReadFile("ux0:/data/psp2048/highscore.txt",buf,10);
	if(ret<0) {
		output=0; 
	} else  {
		sscanf(buf, "%d", &output);
	}
	return output;
}
