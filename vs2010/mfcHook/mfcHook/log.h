#ifndef _LOG_H_
#define _LOG_H_
#define LOG(format, ...) do{\
	FILE *fp = fopen("C:\\log.txt", "a+");\
	fprintf(fp, "[%s:%d]:", __FILE__, __LINE__);\
	fprintf(fp, format, ## __VA_ARGS__);\
	fprintf(fp, "\n");\
	fclose(fp);\
	}while(0)
#define ORI_LOG(format, ...) do{\
	FILE *fp = fopen("C:\\log.txt", "a+");\
	fprintf(fp, format, ## __VA_ARGS__);\
	fclose(fp);\
	}while(0)
#endif