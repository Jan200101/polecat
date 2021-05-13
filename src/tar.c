#include <stdio.h>
#include <stdlib.h>
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <limits.h>

#include "common.h"
#include "tar.h"

static inline int copy_data(struct archive* ar, struct archive* aw)
{
    int r;
    const void *buff;
    size_t size;
    la_int64_t offset;

    for (;;)
    {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF) return (ARCHIVE_OK);
        if (r < ARCHIVE_OK) return (r);

        // why does read return an int but write not?
        r = (int)archive_write_data_block(aw, buff, size, offset);

        if (r < ARCHIVE_OK)
        {
            printf("%s\n", archive_error_string(aw));
            return (r);
        }
    }
}

void extract(const struct MemoryStruct* tar, const char* outputdir)
{
    char cwd[PATH_MAX];
    void* err = getcwd(cwd, sizeof(cwd));

    if (chdir(outputdir) < 0)
    {
        printf("Cannot change to %s\n", outputdir);
        return;
    }

    struct archive* a;
    struct archive* ext;
    struct archive_entry* entry;
    int flags, r;

    /* Select which attributes we want to restore. */
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

    if (archive_read_open_memory(a, tar->memory, tar->size)) return;

    for (;;)
    {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
        {
            break;
        }

        if (r < ARCHIVE_OK)
        {
            printf("%s\n", archive_error_string(a));
        }

        if (r < ARCHIVE_WARN)
        {
            return;
        }

        r = archive_write_header(ext, entry);
        if (r < ARCHIVE_OK)
        {
            printf("%s\n", archive_error_string(ext));
        }
        else if (archive_entry_size(entry) > 0)
        {
            r = copy_data(a, ext);
            if (r < ARCHIVE_OK)
                printf("%s\n", archive_error_string(ext));
            if (r < ARCHIVE_WARN)
                return;
        }

        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_OK)
            printf("%s\n", archive_error_string(ext));
        if (r < ARCHIVE_WARN)
            return;
    }
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);

    if (err) (void)chdir(cwd);
}
