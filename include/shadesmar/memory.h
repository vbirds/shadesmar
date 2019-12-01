//
// Created by squadrick on 27/11/19.
//

#ifndef shadesmar_MEMORY_H
#define shadesmar_MEMORY_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>

#include <cstdint>

namespace shm {

uint8_t *create_memory_segment(const std::string &name, size_t size,
                               bool &new_segment) {
  int fd;
  while (true) {
    new_segment = true;
    fd = shm_open(name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0644);
    if (fd >= 0) {
      fchmod(fd, 0644);
    }
    if (errno == EEXIST) {
      fd = shm_open(name.c_str(), O_RDWR, 0644);
      if (fd < 0 && errno == ENOENT) {
        // the memory segment was deleted in the mean time
        continue;
      }
      new_segment = false;
    }
    break;
  }
  int result = ftruncate(fd, size);
  if (result == EINVAL) {
    return nullptr;
  }
  auto *ptr = static_cast<uint8_t *>(
      mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

  return ptr;
}
} // namespace shm

#endif // shadesmar_MEMORY_H
