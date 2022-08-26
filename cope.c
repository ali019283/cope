#define _XOPEN_SOURCE 500
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <libgen.h>
#include <ftw.h>
#include <stdio.h>
#include <archive.h>
#include <archive_entry.h>
char pac[240];
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
static char *extract(const char *filename){
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
	r = archive_read_next_header(a, &entry);
        char *sg=strdup(archive_entry_pathname(entry));
        for (;;) {
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
	        r = archive_read_next_header(a, &entry);
        }
	archive_read_close(a);
	archive_read_free(a);
	archive_write_close(ext);
	archive_write_free(ext);
        return sg;
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
int fpc(char *b, int kl, char *opt);
int cifi(FILE *ok, char *s[]);
int uni(char *st);
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
        case 'b':
        case 'd':
	      printf("\x1b[32m>>>\x1b[36m Packages that will be installed:\x1b[0m \n");
	      for (int a = 2; a < argc; a++){
                      fpc(argv[a], 1, opt);
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
        case 's':
        case 'r':
              printf("\x1b[32m>>>\x1b[36m Packages that will be removed:\x1b[0m \n");
              for (int a = 2; a < argc; a++){
                    if(opt!='s'){
                            fpc(argv[a], 2, opt);
                    }else{
                            printf("\x1b[32m>>>\x1b[0m %s\n", argv[a]);
                    }
                    uni(argv[a]);
              }
	      return 0;
        }
}
int uni(char *st){
        char rm[120];
        sprintf(rm, "/var/db/rp/%s/manifest", st);
        FILE *fptr = fopen(rm, "r");
        char s[120], ff[120][120];
        int o=0;
        while(fgets(s, 120, fptr)!=NULL){
                strcpy(ff[o], s);
                o++;
        }
        while(o>0){
                o--;
                strcpy(s, ff[o]);
                if(s[strlen(s)-1] == '\n'){
                        s[strlen(s)-1]='\0';
                }
                if(access(s, 0) !=0){
                        printf("\x1b[32m>>>\x1b[36m Package '%s' is not installed\x1b[0m\n", st);
                        return 0;
                }
                remove(s);
        }printf("\x1b[32m>>>\x1b[36m Removed %s\n\x1b[0m", st);
        char jk[120]="";
        char sjk[120]="";
        int i=0;
        FILE *kkk=fopen("/var/db/rp/world", "r");
        int l=cifi(fopen("/var/db/rp/world", "r"), st);
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
int fpc(char *b, int kl, char *opt){
        strcat(pac, b);
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
	while (fgets(s, 120, dep)!=NULL){
                s[strlen(s)-1]='\0';
		char kk[120]="";
		if(kl!=1){
			for(int i = 0; i<=strlen(pac)-strlen(s); i++){
				strncpy(kk, pac+i, strlen(s));
				if(strcmp(kk ,s)==0){
					printf("\x1b[33m>>>\x1b[31m Circular dependency detected, exiting cycle\x1b[0m\n", s);
					return 0;
				}
			}
		}
                if(kl==2){
                        fpc(s, 2, opt); 
                        uni(s); 
                        fclose(dep); 
                        return 0;
                }
                if(cifi(ok,s)==0 || opt=='b'){
			fpc(s, 0, opt); 
                        inst(s);
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
	char buf[60], str[120], *package, build[120], source[120], ins_pkg[120], mani[120];
	sprintf(build, "/var/db/rp/%s/build", b);
	sprintf(source, "/var/db/rp/%s/source", b);
	sprintf(ins_pkg, "/var/db/rp/installed/%s", b);
        sprintf(mani, "/var/db/rp/%s/manifest", b);
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
        if(access(base, 0) == 0){
                printf("\x1b[32m>>>\x1b[36m Source file was found in cache directory, skiping download\x1b[0m\n");
        }else{
                download(str, base);
        }
	printf("\x1b[32m>>>\x1b[36m Extracting %s...\x1b[0m\n", packbas);
        char *gh=extract(packbas);
        mkdir("/root/.cache/pk/pkg_dir", 0700);
        setenv("DESTDIR", "/root/.cache/pk/pkg_dir", 1);
        printf("\x1b[32m>>>\x1b[36m Changing directory to %s\x1b[0m\n", gh);
        chdir(gh);
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
        remove(mani); open(mani, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
        FILE *man=fopen(mani, "a");
        int manif(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
                if(typeflag == FTW_D){
                        fprintf(man, "%s/\n", fpath+23);
                        mkdir(fpath+23, 0700);
                        return 0;
                }
                int in=open(fpath, O_RDONLY), ou=creat(fpath+23, 0660);
                off_t by = 0;
                struct stat fi = {0};
                fstat(in, &fi);
                sendfile(ou, in, &by, fi.st_size);
                chmod(fpath+23, fi.st_mode);
                close(ou); close(in);
                fprintf(man, "%s\n", fpath+23); 
                return 0;
        }int rme(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){return remove(fpath);}
        nftw("/root/.cache/pk/pkg_dir",manif,64, 0); nftw("/root/.cache/pk/pkg_dir",rme,64, FTW_DEPTH);
	fclose(man);
        mkdir(ins_pkg, 0777);
	return 0;
}
