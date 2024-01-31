#define _XOPEN_SOURCE 500
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/sendfile.h>
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
	archive_read_support_filter_all(a);
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
        char *isgit = &url[strlen(url)-4];
        if(strcmp(isgit, ".git") == 0){
                printf("\x1b[32m>>>\x1b[36m Downloading git source %s...\x1b[0m\n", url);
                char turl[480]="git clone --depth=1 ";
                strcat(turl, url);
                char * const k = strrchr(url, '.');
                *k = '\0'; char *kol = strrchr(url, '/'); 
                int rme(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){return remove(fpath);}
                if(access(kol + 1, 0) == 0)
                        nftw(kol + 1, rme, 64, FTW_DEPTH);
                system(turl);
                return 2;
        }
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
int checkinstall(FILE *world, char *pack){
        char worldl[120];
        int line=0;
        while(fgets(worldl, 120, world) !=NULL){
                line++;
                if(worldl[strlen(worldl)-1]=='\n'){
                       worldl[strlen(worldl)-1]='\0';
                }
                if(strcmp(pack, worldl) == 0){
                        return line;
                }
        }
        fclose(world);
        return 0;
}
int uninstall(char *pack){
        char manifest[120];
        sprintf(manifest, "/var/db/rp/%s/manifest", pack);
        FILE *fptr = fopen(manifest, "r");
        if(fptr==NULL){
		printf("\x1b[33m>>>\x1b[31m Can't find package '%s' to remove, skiping\x1b[0m\n", pack);
		return 1;
	}
        char line[120], reline[120][120];
        int i = 0;
        while(fgets(line, 120, fptr)!=NULL){
                strcpy(reline[i], line);
                i++;
        }
        while(i>0){
                i--;
                strcpy(line, reline[i]);
                if(line[strlen(line)-1] == '\n'){
                        line[strlen(line)-1]='\0';
                }
                if(access(line, 0) !=0){
                        printf("\x1b[32m>>>\x1b[36m Package '%s' is not installed\x1b[0m\n", pack);
                        return 0;
                }
                remove(line);
        }printf("\x1b[32m>>>\x1b[36m Removed %s\n\x1b[0m", pack);
        char tempworld[1024]="";
        char worldl[120]="";
        i = 0;
        FILE *world=fopen("/var/db/rp/world", "r");
        int l=checkinstall(fopen("/var/db/rp/world", "r"), pack);
        while(fgets(worldl, 120, world) != NULL){
                i++;
                if(l==i){
                        continue;
                }
                strcat(tempworld, worldl);
        }fclose(world);
        world=fopen("/var/db/rp/world", "w");
        fprintf(world, "%s", tempworld);
        fclose(world);
        return 0;
}
int install(char *pack){
	char buf[60], str[120], *package, build[120], source[120], manifest[120];
	sprintf(build, "/var/db/rp/%s/build", pack);
	sprintf(source, "/var/db/rp/%s/source", pack);
        sprintf(manifest, "/var/db/rp/%s/manifest", pack);
        FILE *fptr = fopen(source, "r");
	if(fptr==NULL){
		printf("\x1b[33m>>>\x1b[31m Can't find package '%s' to install, skiping\x1b[0m\n", pack);
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
        char *isgit = &packbas[strlen(packbas)-4]; if(strcmp(isgit, ".git")==0){
                packbas[strlen(packbas)-4] = '\0';
        }
        char *extracted=packbas;
        int checkgit;
        if(access(base, 0) == 0){
                printf("\x1b[32m>>>\x1b[36m Source file was found in cache directory, skiping download\x1b[0m\n");
        }else{
                checkgit = download(str, base);
        }
        if(checkgit != 2){
                printf("\x1b[32m>>>\x1b[36m Extracting %s...\x1b[0m\n", packbas);
                extracted=extract(packbas);
        }
        mkdir("/root/.cache/pk/pkg_dir", 0700);
        setenv("DESTDIR", "/root/.cache/pk/pkg_dir", 1);
        printf("\x1b[32m>>>\x1b[36m Changing directory to %s\x1b[0m\n", extracted);
        chdir(extracted);
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
        remove(manifest); open(manifest, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
        FILE *man=fopen(manifest, "a");
        printf("\x1b[32m>>>\x1b[36m Generating manifest...\x1b[0m\n");
        int manif(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
                if(typeflag == FTW_D){
                        fprintf(man, "%s/\n", fpath+23);
                        mkdir(fpath+23, 0755);
                        printf("\x1b[32m>>>\x1b[0m %s/\n", fpath+23);
                        return 0;
                }
                printf("\x1b[32m>>>\x1b[0m %s\n", fpath+23);
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
        nftw("/root/.cache/pk/pkg_dir", manif, 64, 0); nftw("/root/.cache/pk/pkg_dir", rme, 64, FTW_DEPTH);
	fclose(man);
        printf("\x1b[32m>>>\x1b[36m Succesfully installed %s\x1b[0m\n", pack);
	return 0;
}
int dependcheck(char *pack, int uni, char opt){
        strcat(pac, pack);
	char depend[120];
	sprintf(depend, "/var/db/rp/%s/depends", pack);
	char deppack[120];
	printf("\x1b[32m>>>\x1b[0m %s\n", pack);
	FILE *dep = fopen(depend, "r");
	if(dep==NULL){
		printf("\x1b[32m>>>\x1b[35m Package '%s' doesnt have any dependency folder, skiping dependency check\x1b[0m\n", pack);
		return 1;
	}
        FILE *world=fopen("/var/db/rp/world", "r");
	while (fgets(deppack, 120, dep)!=NULL){
                deppack[strlen(deppack)-1]='\0';
		char paclist[120]="";
		if(uni!=1){
			for(int i = 0; i<=strlen(pac)-strlen(deppack); i++){
				strncpy(paclist, pac+i, strlen(deppack));
				if(strcmp(paclist, deppack)==0){
					printf("\x1b[33m>>>\x1b[31m Circular dependency detected, exiting cycle\x1b[0m\n", deppack);
					return 0;
				}
			}
		}
                if(uni==2){
                        dependcheck(deppack, 2, opt); 
                        uninstall(deppack); 
                        fclose(dep); 
                        return 0;
                }else if(opt=='b'){
			dependcheck(deppack, 0, opt); 
                        install(deppack);
                        fclose(dep);
                        return 0;
                }else if(checkinstall(world, deppack)==0){
			dependcheck(deppack, 0, opt); 
                        install(deppack);
                }else{
                        printf("\x1b[32m>>>\x1b[36m Dependency %s is already installed, skiping\x1b[0m\n", deppack);
                        return 0;
                }
	}
	fclose(dep);
        world=fopen("/var/db/rp/world", "a");
        fprintf(world, "%s\n", deppack); fclose(world);
	return 0;
}
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
                                dependcheck(argv[a], 1, opt);
                                if(install(argv[a])==0){
                                        if(checkinstall(fopen("/var/db/rp/world", "r"), argv[a]) == 0){
                                                FILE *world=fopen("/var/db/rp/world", "a");
                                                fprintf(world, "%s\n", argv[a]); fclose(world);
                                        }
                                }
                        }
                        exit(0);
                case 'f':
                        printf("\x1b[32m>>>\x1b[36m Packages that will be installed:\x1b[0m \n");
                        for (int a = 2; a < argc; a++){
                                if(install(argv[a])==0){
                                        if(checkinstall(fopen("/var/db/rp/world", "r"), argv[a]) == 0){
                                                FILE *world=fopen("/var/db/rp/world", "a");
                                                fprintf(world, "%s\n", argv[a]); fclose(world);
                                        }
                                }
                        }
                        exit(0);
                        return 0;
                case 's':
                case 'r':
                        printf("\x1b[32m>>>\x1b[36m Packages that will be removed:\x1b[0m \n");
                        for (int a = 2; a < argc; a++){
                                if(opt!='s'){
                                        dependcheck(argv[a], 2, opt);
                                }else{
                                        printf("\x1b[32m>>>\x1b[0m %s\n", argv[a]);
                                }
                                uninstall(argv[a]);
                        }
                        exit(0);
                case 'u':
                        printf("\x1b[32m>>>\x1b[36m Updating repo\x1b[0m \n");
                        install("rp");
                        exit(0);
                        
                case 'l':
                        char line[64];
                        FILE *worldr=fopen("/var/db/rp/world", "r");
                        while (fgets(line, 64, worldr)){
                                printf("\x1b[32m>>>\x1b[0m %s", line);
                        }
                        exit(0);
        }

}