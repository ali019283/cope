#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include <archive.h>
#include <archive_entry.h>
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
		result = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(output);
		return 1;
	}

	return 0;
}
int main(int argc, char *argv[]){
	char buf[60];
	if(geteuid() != 0)
        {
                puts("This program needs root privileges, exiting.");
                exit(0);
        }
        char opt = argc > 1 ? argv[1][0] : ' ';
	char *temp_dir = "/root/.cache/pk/";
	chdir(temp_dir);
	switch (opt) {
		char *package;
                case 'd':
			printf("\x1b[31m>>> \x1b[33m Packages that will be installed:\x1b[0m \n");
                        for(int a = 2; a < argc; a++){
                                printf("\x1b[31m>>> \x1b[0m %s \n", argv[a]);
                        }
			char *package_or;
			for (int a = 2; a < argc; a++){
                                package_or=(argv[a]);
				char build[120];
				sprintf(build, "/var/db/rp/%s/build", package_or);
				char source[120];
				sprintf(source, "/var/db/rp/%s/source", package_or);
				char ins_pkg[120];
				sprintf(ins_pkg, "/var/db/rp/installed/%s", package_or);
				FILE *fptr = fopen(source, "r");
                                if(fptr==NULL){
                                        printf("\x1b[31m>>>\x1b[33m Can't find package '%s', skiping\x1b[0m\n", argv[a]);
                                        continue;
                                }
				char str[120];
				fgets(str, 120, fptr);
				fclose(fptr);
				char arg[120];
                                snprintf(arg, "%s", argv[a]);
                                strncpy(buf, arg, 60);
				strcat(arg, ".tar");
				package=arg;
				download(str, package);
				extract(package);
				system(build);
				mkdir(ins_pkg, 0777);
			}
			return 0;
		case 'u':
                        printf("\x1b[31m>>> \x1b[33m Packages that will be removed:\x1b[0m \n");
                        for(int a = 2; a < argc; a++){
                                printf("\x1b[31m>>> \x1b[0m %s \n", argv[a]);
                        }
			for (int a = 2; a < argc ; a++){	
				package=argv[a];
				char remove[120];
				sprintf(remove, "/var/db/rp/%s/uninstall", package);
				system(remove);
			
			}
	return 0;
	}
}
