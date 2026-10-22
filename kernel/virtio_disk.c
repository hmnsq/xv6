//
// driver for qemu's virtio disk device.
// uses qemu's mmio interface to virtio.
//
// qemu ... -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
//

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"
#ಅನ್ತರ್ಭಾವ "fs.h"
#ಅನ್ತರ್ಭಾವ "buf.h"
#ಅನ್ತರ್ಭಾವ "virtio.h"

// the address of virtio mmio register r.
#define R(r) ((ಚಞ್ಚಲ uint32 *)(VIRTIO0 + (r)))

ಸ್ಥಿರ ಸಂರಚನಾ disk {
  // a set (not a ring) of DMA descriptors, with which the
  // driver tells the device where to read and write individual
  // disk operations. there are NUM descriptors.
  // most commands consist of a "chain" (a linked list) of a couple of
  // these descriptors.
  ಸಂರಚನಾ virtq_desc *desc;

  // a ring in which the driver writes descriptor numbers
  // that the driver would like the device to process.  it only
  // includes the head descriptor of each chain. the ring has
  // NUM elements.
  ಸಂರಚನಾ virtq_avail *avail;

  // a ring in which the device writes descriptor numbers that
  // the device has finished processing (just the head of each chain).
  // there are NUM used ring entries.
  ಸಂರಚನಾ virtq_used *used;

  // our own book-keeping.
  ವರ್ಣ ಮುಕ್ತ[ಅಂಕೆ_2];  // is a descriptor free?
  uint16 used_idx; // we've looked this far in used[2..NUM].

  // track info about in-flight operations,
  // for use when completion interrupt arrives.
  // indexed by first descriptor index of chain.
  ಸಂರಚನಾ {
    ಸಂರಚನಾ buf *b;
    ವರ್ಣ status;
  } info[ಅಂಕೆ_2];

  // disk command headers.
  // one-for-one with descriptors, for convenience.
  ಸಂರಚನಾ virtio_blk_req ops[ಅಂಕೆ_2];

  ಸಂರಚನಾ spinlock vdisk_lock;

} disk;

ಶೂನ್ಯ
virtio_disk_init(ಶೂನ್ಯ)
{
  uint32 status = 0;

  initlock(&disk.vdisk_lock, "virtio_disk");

  ಯದಿ (*R(VIRTIO_MMIO_MAGIC_ಮೌಲ್ಯ) != 0x74726976 ||
      *R(VIRTIO_MMIO_VERSION) != 2 || *R(VIRTIO_MMIO_DEVICE_ID) != 2 ||
      *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551) {
    panic("could not find virtio disk");
  }

  // reset device
  *R(VIRTIO_MMIO_STATUS) = status;

  // set ACKNOWLEDGE status bit
  status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
  *R(VIRTIO_MMIO_STATUS) = status;

  // set DRIVER status bit
  status |= VIRTIO_CONFIG_S_DRIVER;
  *R(VIRTIO_MMIO_STATUS) = status;

  // negotiate features
  uint64 features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
  features &= ~(1 << VIRTIO_BLK_F_RO);
  features &= ~(1 << VIRTIO_BLK_F_SCSI);
  features &= ~(1 << VIRTIO_BLK_F_FLUSH);
  features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
  features &= ~(1 << VIRTIO_BLK_F_MQ);
  features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
  features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
  features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
  *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

  // tell device that feature negotiation is complete.
  status |= VIRTIO_CONFIG_S_FEATURES_OK;
  *R(VIRTIO_MMIO_STATUS) = status;

  // re-read status to ensure FEATURES_OK is set.
  status = *R(VIRTIO_MMIO_STATUS);
  ಯದಿ (!(status & VIRTIO_CONFIG_S_FEATURES_OK))
    panic("virtio disk FEATURES_OK unset");

  // initialize queue 0.
  *R(VIRTIO_MMIO_QUEUE_SEL) = 0;

  // ensure queue 0 is not in use.
  ಯದಿ (*R(VIRTIO_MMIO_QUEUE_READY))
    panic("virtio disk should not be ready");

  // check maximum queue size.
  uint32 max = *R(VIRTIO_MMIO_QUEUE_ಅಂಕೆ_MAX);
  ಯದಿ (max == 0)
    panic("virtio disk has no queue 0");
  ಯದಿ (max < ಅಂಕೆ_2)
    panic("virtio disk max queue too short");

  // allocate and zero queue memory.
  disk.desc = kalloc();
  disk.avail = kalloc();
  disk.used = kalloc();
  ಯದಿ (!disk.desc || !disk.avail || !disk.used)
    panic("virtio disk kalloc");
  memset(disk.desc, 0, PGSIZE);
  memset(disk.avail, 0, PGSIZE);
  memset(disk.used, 0, PGSIZE);

  // set queue size.
  *R(VIRTIO_MMIO_QUEUE_ಅಂಕೆ) = ಅಂಕೆ_2;

  // write physical addresses.
  *R(VIRTIO_MMIO_QUEUE_DESC_ಕೆಳಗಿನ) = (uint64)disk.desc;
  *R(VIRTIO_MMIO_QUEUE_DESC_ಮೇಲಿನ) = (uint64)disk.desc >> 32;
  *R(VIRTIO_MMIO_DRIVER_DESC_ಕೆಳಗಿನ) = (uint64)disk.avail;
  *R(VIRTIO_MMIO_DRIVER_DESC_ಮೇಲಿನ) = (uint64)disk.avail >> 32;
  *R(VIRTIO_MMIO_DEVICE_DESC_ಕೆಳಗಿನ) = (uint64)disk.used;
  *R(VIRTIO_MMIO_DEVICE_DESC_ಮೇಲಿನ) = (uint64)disk.used >> 32;

  // queue is ready.
  *R(VIRTIO_MMIO_QUEUE_READY) = 0x1;

  // all NUM descriptors start out unused.
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_2; i++)
    disk.ಮುಕ್ತ[i] = 1;

  // tell device we're completely ready.
  status |= VIRTIO_CONFIG_S_DRIVER_OK;
  *R(VIRTIO_MMIO_STATUS) = status;

  // plic.c and trap.c arrange for interrupts from VIRTIO0_IRQ.
}

// find a free descriptor, mark it non-free, return its index.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
alloc_desc()
{
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_2; i++) {
    ಯದಿ (disk.ಮುಕ್ತ[i]) {
      disk.ಮುಕ್ತ[i] = 0;
      ಪ್ರತ್ಯಾಗಮ i;
    }
  }
  ಪ್ರತ್ಯಾಗಮ -1;
}

// mark a descriptor as free.
ಸ್ಥಿರ ಶೂನ್ಯ
free_desc(ಪೂರ್ಣಾಙ್ಕ i)
{
  ಯದಿ (i >= ಅಂಕೆ_2)
    panic("free_desc 1");
  ಯದಿ (disk.ಮುಕ್ತ[i])
    panic("free_desc 2");
  disk.desc[i].addr = 0;
  disk.desc[i].len = 0;
  disk.desc[i].flags = 0;
  disk.desc[i].next = 0;
  disk.ಮುಕ್ತ[i] = 1;
  wakeup(&disk.ಮುಕ್ತ[0]);
}

// free a chain of descriptors.
ಸ್ಥಿರ ಶೂನ್ಯ
free_chain(ಪೂರ್ಣಾಙ್ಕ i)
{
  ಯಾವತ್ (1) {
    ಪೂರ್ಣಾಙ್ಕ flag = disk.desc[i].flags;
    ಪೂರ್ಣಾಙ್ಕ nxt = disk.desc[i].next;
    free_desc(i);
    ಯದಿ (flag & VRING_DESC_F_NEXT)
      i = nxt;
    ಅನ್ಯಥಾ
      ಭಙ್ಗ;
  }
}

// allocate three descriptors (they need not be contiguous).
// disk transfers always use three descriptors.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
alloc3_desc(ಪೂರ್ಣಾಙ್ಕ *idx)
{
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 3; i++) {
    idx[i] = alloc_desc();
    ಯದಿ (idx[i] < 0) {
      ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < i; j++)
        free_desc(idx[j]);
      ಪ್ರತ್ಯಾಗಮ -1;
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ
virtio_disk_rw(ಸಂರಚನಾ buf *b, ಪೂರ್ಣಾಙ್ಕ ಲಿಖ)
{
  uint64 sector = b->blockno * (BSIZE / 512);

  acquire(&disk.vdisk_lock);

  // the spec's Section 5.2 says that legacy block operations use
  // three descriptors: one for type/reserved/sector, one for the
  // data, one for a 1-byte status result.

  // allocate the three descriptors.
  ಪೂರ್ಣಾಙ್ಕ idx[3];
  ಯಾವತ್ (1) {
    ಯದಿ (alloc3_desc(idx) == 0) {
      ಭಙ್ಗ;
    }
    sleep_prepare(&disk.ಮುಕ್ತ[0]);
    release(&disk.vdisk_lock);
    sleep();
    acquire(&disk.vdisk_lock);
  }

  // format the three descriptors.
  // qemu's virtio-blk.c reads them.

  ಸಂರಚನಾ virtio_blk_req *buf0 = &disk.ops[idx[0]];

  ಯದಿ (ಲಿಖ)
    buf0->type = VIRTIO_BLK_T_OUT; // write the disk
  ಅನ್ಯಥಾ
    buf0->type = VIRTIO_BLK_T_IN; // read the disk
  buf0->reserved = 0;
  buf0->sector = sector;

  disk.desc[idx[0]].addr = (uint64)buf0;
  disk.desc[idx[0]].len = ಪರಿಮಾಣ(ಸಂರಚನಾ virtio_blk_req);
  disk.desc[idx[0]].flags = VRING_DESC_F_NEXT;
  disk.desc[idx[0]].next = idx[1];

  disk.desc[idx[1]].addr = (uint64)b->data;
  disk.desc[idx[1]].len = BSIZE;
  ಯದಿ (ಲಿಖ)
    disk.desc[idx[1]].flags = 0; // device reads b->data
  ಅನ್ಯಥಾ
    disk.desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
  disk.desc[idx[1]].flags |= VRING_DESC_F_NEXT;
  disk.desc[idx[1]].next = idx[2];

  disk.info[idx[0]].status = 0xff; // device writes 0 on success
  disk.desc[idx[2]].addr = (uint64)&disk.info[idx[0]].status;
  disk.desc[idx[2]].len = 1;
  disk.desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
  disk.desc[idx[2]].next = 0;

  // record struct buf for virtio_disk_intr().
  b->disk = 1;
  disk.info[idx[0]].b = b;

  // tell the device the first index in our chain of descriptors.
  disk.avail->ring[disk.avail->idx % ಅಂಕೆ_2] = idx[0];

  io_fence();

  // tell the device another avail ring entry is available.
  disk.avail->idx += 1; // not % NUM ...

  io_fence();

  *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0; // value is queue number

  // Wait for virtio_disk_intr() to say request has finished.
  ಯಾವತ್ (b->disk == 1) {
    sleep_prepare(b);
    release(&disk.vdisk_lock);
    sleep();
    acquire(&disk.vdisk_lock);
  }

  disk.info[idx[0]].b = 0;
  free_chain(idx[0]);

  release(&disk.vdisk_lock);
}

ಶೂನ್ಯ
virtio_disk_intr()
{
  acquire(&disk.vdisk_lock);

  // the device won't raise another interrupt until we tell it
  // we've seen this interrupt, which the following line does.
  // this may race with the device writing new entries to
  // the "used" ring, in which case we may process the new
  // completion entries in this interrupt, and have nothing to do
  // in the next interrupt, which is harmless.
  *R(VIRTIO_MMIO_INTERRUPT_ACK) = *R(VIRTIO_MMIO_INTERRUPT_STATUS) & 0x3;

  io_fence();

  // the device increments disk.used->idx when it
  // adds an entry to the used ring.

  ಯಾವತ್ (disk.used_idx != disk.used->idx) {
    io_fence();
    ಪೂರ್ಣಾಙ್ಕ id = disk.used->ring[disk.used_idx % ಅಂಕೆ_2].id;

    ಯದಿ (disk.info[id].status != 0)
      panic("virtio_disk_intr status");

    ಸಂರಚನಾ buf *b = disk.info[id].b;
    b->disk = 0; // disk is done with buf
    wakeup(b);

    disk.used_idx += 1;
  }

  release(&disk.vdisk_lock);
}
