#include <stdio.h>
#include <extract.h>

static int copy_data(struct archive* ar, struct archive* aw) {
  int r;
  const void* buf;
  size_t size;
  la_int64_t offset;

  for (;;) {
    r = archive_read_data_block(ar, &buf, &size, &offset);
    if (r == ARCHIVE_EOF) return ARCHIVE_OK;
    if (r < ARCHIVE_OK) return r;
    r = archive_write_data_block(aw, buf, size, offset);
    if (r < ARCHIVE_OK) {
      log(error, "%s\n", archive_error_string(aw));
      return r;
    }
  }
}

int extract(const char* in) {
  struct archive* a;
  struct archive* ext;
  struct archive_entry* entry;
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

  if ((r = archive_read_open_filename(a, in, 10240)))
    goto err;
  for (;;) {
    r = archive_read_next_header(a, &entry);
    if (r == ARCHIVE_EOF)
      break;
    if (r < ARCHIVE_OK)
      log(error, "%s\n", archive_error_string(a));
    if (r < ARCHIVE_WARN)
      goto err;
    char out[FILENAME_MAX];
    snprintf(out, sizeof(out), "%s/%s", temp(DEFAULT_DIR_NAME), archive_entry_pathname(entry));
    archive_entry_set_pathname(entry, out);
    r = archive_write_header(ext, entry);
    if (r < ARCHIVE_OK)
      log(error, "%s\n", archive_error_string(ext));
    else if (archive_entry_size(entry) > 0) {
      r = copy_data(a, ext);
      if (r < ARCHIVE_OK)
        log(error, "%s\n", archive_error_string(ext));
      if (r < ARCHIVE_WARN)
        goto err;
    }
    r = archive_write_finish_entry(ext);
    if (r < ARCHIVE_OK)
      log(error, "%s\n", archive_error_string(ext));
    if (r < ARCHIVE_WARN)
      goto err;
  }

  archive_read_close(a);
  archive_read_free(a);
  archive_write_close(ext);
  archive_write_free(ext);
  return 1;

err:
  log(error, "something wrong with extract archive; exit code is -1;\n");
  return -1;

}
