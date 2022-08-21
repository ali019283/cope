#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <libgen.h>
#include <stdio.h>
#include <archive.h>
#include <archive_entry.h>
char *packs[240];
char pac[120];
int copy_data(struct archive *ar, struct archive *aw){
	int r;
	const void *buff;
	size_t size;
	la_int64_t offset;

	for (;;) {
		r = archive_read_data_block(ar, &buff, &size, &offset);
		if (r == ARCHIVE_EOF)
			return (ARCHIVE_OK);
		if (r < ARCHIVE_OK)
			return (r);
		r = archive_write_data_block(aw, buff, size, offset);
		if (r < ARCHIVE_OK) {
			fprintf(stderr, "%s\n", archive_error_string(aw));
			return (r);
		}
	}
}
static void extract(const char *filename){
	struct archive *a;
	struct archive *ext;
	struct archive_entry *entry;
	int flags;
	int r;
	flags = ARCHIVE_EXTRACT_TIME;
	flags |= ARCHIVE_EXTRACT_PERM;
	flags |= ARCHIVE_EXTRACT_ACL;
	flags |= ARCHIVE_EXTRACT_FFLAGS;
	a = archive_read_new();
	archive_read_support_format_all(a);
	archive_read_support_compression_all(a);
	ext = archive_write_disk_new();
	archive_write_disk_set_options(ext, flags);
	archive_write_disk_set_standard_lookup(ext);
	if ((r = archive_read_open_filename(a, filename, 10240)))
		exit(1);
	for (;;) {
		r = archive_read_next_header(a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r < ARCHIVE_OK)
			fprintf(stderr, "%s\n", archive_error_string(a));
		if (r < ARCHIVE_WARN)
			exit(1);
		r = archive_write_header(ext, entry);
		if (r < ARCHIVE_OK)
			fprintf(stderr, "%s\n", archive_error_string(ext));
		else if (archive_entry_size(entry) > 0) {
			r = copy_data(a, ext);
			if (r < ARCHIVE_OK)
				fprintf(stderr, "%s\n", archive_error_string(ext));
			if (r < ARCHIVE_WARN)
				exit(1);
		}
		r = archive_write_finish_entry(ext);
		if (r < ARCHIVE_OK)
			fprintf(stderr, "%s\n", archive_error_string(ext));
		if (r < ARCHIVE_WARN)
			exit(1);
	}
	archive_read_close(a);
	archive_read_free(a);
	archive_write_close(ext);
	archive_write_free(ext);
}
int download_write_data(void *pointer, size_t size, size_t nmemb, FILE *stream){
	size_t written = fwrite(pointer, size, nmemb, stream);
	return written;
}

int download(char *url, char *destination){
	FILE *output;
	CURLcode result;
	CURL *curl = curl_easy_init();
	if (curl) {
		output = fopen(destination, "wb");
		if (output == NULL)
			return 0;
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, output);
		printf("\x1b[32m>>>\x1b[36m Downloading %s...\x1b[0m\n", destination);
		result = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(output);
		return 1;
	}
	return 0;
}
int is(int i, char s[]);
int fpc(char *b, int kl);
int inst(char *b);
int cifi(FILE *ok, char *s[]);
int main(int argc, char *argv[]){
	if(geteuid() != 0)
        {
            printf("\x1b[33m>>>\x1b[31m This program needs root privilages, exiting\x1b[0m\n");
            exit(0);
        }
    char opt = argc > 1 ? argv[1][0] : ' ';
	char *temp_dir = "/root/.cache/pk/";
	chdir(temp_dir);
	switch (opt) {
        case 'd':
	      printf("\x1b[32m>>>\x1b[36m Packages that will be installed:\x1b[0m \n");
	      for (int a = 2; a < argc; a++){
                      fpc(argv[a], 1);
                      if(inst(argv[a])==0){
                              char kk[120];
                              snprintf(kk, "%s", argv[a]);
                              if(cifi(fopen("/var/db/rp/world", "r"), kk) == 0){
                                      FILE *ok=fopen("/var/db/rp/world", "a");
                                      fprintf(ok, "%s\n", argv[a]); fclose(ok);
                              }
                      }
	      }
	      return 0;
	case 'u':
              printf("\x1b[32m>>> \x1b[36m Packages that will be removed:\x1b[0m \n");
              for(int a = 2; a < argc; a++){
                      printf("\x1b[32m>>> \x1b[0m %s \n", argv[a]);
              }
              for (int a = 2; a < argc ; a++){		
                      char *package;
                      package=argv[a];
		      char rm[120];
		      sprintf(rm, "/var/db/rp/%s/files", package);
		      FILE *fptr = fopen(rm, "r");
                      char s[120];
		      while(fgets(s, 120, fptr)){
                              if(s[strlen(s)-1] == '\n'){
                                      s[strlen(s)-1]='\0';
                              }remove(s);
                      }char jk[120];
                      char sjk[120];
                      int i=0;
                      FILE *kkk=fopen("/var/db/rp/world", "r");
                      int l=cifi(fopen("/var/db/rp/world", "r"), argv[a]);
                      while(fgets(sjk, 120, kkk) != NULL){
                              i++;
                              if(l==i){
                                      continue;
                              }
                              strcat(jk, sjk);
                      }fclose(kkk);
                      kkk=fopen("/var/db/rp/world", "w");
                      fprintf(kkk, jk);
                      fclose(kkk);
                      
	      }
		return 0;
	}
}
int is(int i, char s[]){
	packs[i]=s;
        fpc(packs[i], 0);
	inst(packs[i]);
	return 0;
}
int cifi(FILE *ok, char *s[]){
        char k[120];
        int j=0;
        while(fgets(k, 120, ok) !=NULL){
                j++;
                if(k[strlen(k)-1]=='\n'){
                       k[strlen(k)-1]='\0';
                }
                if(strcmp(s, k) == 0){
                        return j;
                }
        }
        fclose(ok);
        return 0;
}
int fpc(char *b, int kl){
        strcat(pac, b);
	puts(b); printf("%s", pac);
	char depend[120];
	sprintf(depend, "/var/db/rp/%s/depends", b);
	char s[120];
	printf("\x1b[32m>>>\x1b[0m %s\n", b);
	FILE *dep = fopen(depend, "r");
	if(dep==NULL){
		printf("\x1b[32m>>>\x1b[35m Package '%s' doesnt have any dependency folder, skiping dependency check\x1b[0m\n", b);
		return 1;
	}
        FILE *ok=fopen("/var/db/rp/world", "r");
	for (int i = strlen(packs); fgets(s, 120, dep); i++){
                s[strlen(s)-1]='\0';
		char kk[120]="";
		if(kl==0){
			for(int i = 0; i<=strlen(pac)-strlen(s); i++){
				strncpy(kk, pac+i, strlen(s));
				printf("%s  %s\n", kk, s); 
				if(strcmp(kk ,s)==0){
					printf("\x1b[33m>>>\x1b[31m Circular dependency detected while installing %s, exiting cycle\x1b[0m\n", s);
					return 0;
				}
			}
		}
                if(cifi(ok,s)==0){
                        is(i, s);
                }else{
                        printf("\x1b[32m>>>\x1b[36m Dependency %s is already installed, skiping\x1b[0m\n", s);
                        return 0;
                }
	}
	fclose(dep);
        ok=fopen("/var/db/rp/world", "a");
        fprintf(ok, "%s\n", s); fclose(ok);
	return 0;
}
int inst(char *b){
	char buf[60];
	char str[120];
	char *package;
	char build[120];
	sprintf(build, "/var/db/rp/%s/build", b);
	char source[120];
	sprintf(source, "/var/db/rp/%s/source", b);
	char ins_pkg[120];
	sprintf(ins_pkg, "/var/db/rp/installed/%s", b);
	char rm[120];
        sprintf(rm, "/var/db/rp/%s/files", b);
        FILE *fptr = fopen(source, "r");
	if(fptr==NULL){
		printf("\x1b[33m>>>\x1b[31m Can't find package '%s', skiping\x1b[0m\n", b);
		return 1;
	}
        fgets(str, 120, fptr);
        char base[120];
	strcpy(base, basename(str));
	if(base[strlen(base)-1] == '\n'){
                base[strlen(base)-1]='\0'; 
                str[strlen(str)-1]='\0';
        }
	char packbas[120];strcpy(packbas, base);
        download(str, base);
	printf("\x1b[32m>>>\x1b[36m Extracting %s...\x1b[0m\n", packbas);
        extract(packbas);
        FILE *ok = fopen(rm, "r");
        while(fgets(str, 120, ok)!=NULL){
                if(str[strlen(str)-1]=='\n') 
                        str[strlen(str)-1]='\0';
        }fclose(ok);
        char dir[120]="/root/.cache/pk/";
        strcat(dir, str);
        printf("\x1b[32m>>>\x1b[36m Changing directory to %s\x1b[0m\n", dir);
        chdir(dir);
	while(fgets(str, 120, fptr)!=NULL){
                strcpy(base, basename(str)); 
                      if(base[strlen(base)-1] == '\n'){
                              base[strlen(base)-1]='\0'; 
                              str[strlen(str)-1]='\0';
                      } 
                download(str, base);
        }fclose(fptr);
	system(build);
        chdir("/root/.cache/pk");
	mkdir(ins_pkg, 0777);
	return 0;
}
