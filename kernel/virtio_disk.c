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
#ವ್ಯಾಖ್ಯಾನಿಸು R(r) ((ಅಸ್ಥಿರ uint32 *)(VIRTIO0 + (r)))

ಸ್ಥಿರ ರಚನೆ disk {
  // a set (not a ring) of DMA descriptors, with which the
  // driver tells the device where to read and write individual
  // disk operations. there are NUM descriptors.
  // most commands consist of a "chain" (a linked list) of a couple of
  // these descriptors.
  ರಚನೆ virtq_desc *desc;

  // a ring in which the driver writes descriptor numbers
  // that the driver would like the device to process.  it only
  // includes the head descriptor of each chain. the ring has
  // NUM elements.
  ರಚನೆ virtq_avail *avail;

  // a ring in which the device writes descriptor numbers that
  // the device has finished processing (just the head of each chain).
  // there are NUM used ring entries.
  ರಚನೆ virtq_ಬಳಸಿದ *ಬಳಸಿದ_2;

  // our own book-keeping.
  ಅಕ್ಷರ ಬಿಡುಗಡೆ[ಅಂಕೆ_2];  // is a descriptor free?
  uint16 ಬಳಸಿದ_idx; // we've looked this far in used[2..NUM].

  // track info about in-flight operations,
  // for use when completion interrupt arrives.
  // indexed by first descriptor index of chain.
  ರಚನೆ {
    ರಚನೆ buf *b;
    ಅಕ್ಷರ ಸ್ಥಿತಿ_2;
  } info[ಅಂಕೆ_2];

  // disk command headers.
  // one-for-one with descriptors, for convenience.
  ರಚನೆ virtio_blk_req ops[ಅಂಕೆ_2];

  ರಚನೆ spinlock vdisk_ಬೀಗ;

} disk;

ಶೂನ್ಯ
virtio_disk_init(ಶೂನ್ಯ)
{
  uint32 ಸ್ಥಿತಿ_2 = 0;

  initlock(&disk.vdisk_ಬೀಗ, "virtio_disk");

  ಆದರೆ (*R(VIRTIO_MMIO_MAGIC_ಮೌಲ್ಯ) != 0x74726976 ||
      *R(VIRTIO_MMIO_ಆವೃತ್ತಿ) != 2 || *R(VIRTIO_MMIO_DEVICE_ID) != 2 ||
      *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551) {
    panic("could not find virtio disk");
  }

  // reset device
  *R(VIRTIO_MMIO_ಸ್ಥಿತಿ) = ಸ್ಥಿತಿ_2;

  // set ACKNOWLEDGE status bit
  ಸ್ಥಿತಿ_2 |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
  *R(VIRTIO_MMIO_ಸ್ಥಿತಿ) = ಸ್ಥಿತಿ_2;

  // set DRIVER status bit
  ಸ್ಥಿತಿ_2 |= VIRTIO_CONFIG_S_DRIVER;
  *R(VIRTIO_MMIO_ಸ್ಥಿತಿ) = ಸ್ಥಿತಿ_2;

  // negotiate features
  uint64 features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
  features &= ~(1 << VIRTIO_BLK_F_RO);
  features &= ~(1 << VIRTIO_BLK_F_SCSI);
  features &= ~(1 << VIRTIO_BLK_F_FLUSH);
  features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
  features &= ~(1 << VIRTIO_BLK_F_MQ);
  features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
  features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
  features &= ~(1 << VIRTIO_RING_F_ಪರೋಕ್ಷ_DESC);
  *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

  // tell device that feature negotiation is complete.
  ಸ್ಥಿತಿ_2 |= VIRTIO_CONFIG_S_FEATURES_OK;
  *R(VIRTIO_MMIO_ಸ್ಥಿತಿ) = ಸ್ಥಿತಿ_2;

  // re-read status to ensure FEATURES_OK is set.
  ಸ್ಥಿತಿ_2 = *R(VIRTIO_MMIO_ಸ್ಥಿತಿ);
  ಆದರೆ (!(ಸ್ಥಿತಿ_2 & VIRTIO_CONFIG_S_FEATURES_OK))
    panic("virtio disk FEATURES_OK unset");

  // initialize queue 0.
  *R(VIRTIO_MMIO_ಸರತಿ_SEL) = 0;

  // ensure queue 0 is not in use.
  ಆದರೆ (*R(VIRTIO_MMIO_ಸರತಿ_READY))
    panic("virtio disk should not be ready");

  // check maximum queue size.
  uint32 max = *R(VIRTIO_MMIO_ಸರತಿ_ಅಂಕೆ_MAX);
  ಆದರೆ (max == 0)
    panic("virtio disk has no queue 0");
  ಆದರೆ (max < ಅಂಕೆ_2)
    panic("virtio disk max queue too short");

  // allocate and zero queue memory.
  disk.desc = kalloc();
  disk.avail = kalloc();
  disk.ಬಳಸಿದ_2 = kalloc();
  ಆದರೆ (!disk.desc || !disk.avail || !disk.ಬಳಸಿದ_2)
    panic("virtio disk kalloc");
  memset(disk.desc, 0, PGSIZE);
  memset(disk.avail, 0, PGSIZE);
  memset(disk.ಬಳಸಿದ_2, 0, PGSIZE);

  // set queue size.
  *R(VIRTIO_MMIO_ಸರತಿ_ಅಂಕೆ) = ಅಂಕೆ_2;

  // write physical addresses.
  *R(VIRTIO_MMIO_ಸರತಿ_DESC_ಕೆಳಗಿನ) = (uint64)disk.desc;
  *R(VIRTIO_MMIO_ಸರತಿ_DESC_ಮೇಲಿನ) = (uint64)disk.desc >> 32;
  *R(VIRTIO_MMIO_DRIVER_DESC_ಕೆಳಗಿನ) = (uint64)disk.avail;
  *R(VIRTIO_MMIO_DRIVER_DESC_ಮೇಲಿನ) = (uint64)disk.avail >> 32;
  *R(VIRTIO_MMIO_DEVICE_DESC_ಕೆಳಗಿನ) = (uint64)disk.ಬಳಸಿದ_2;
  *R(VIRTIO_MMIO_DEVICE_DESC_ಮೇಲಿನ) = (uint64)disk.ಬಳಸಿದ_2 >> 32;

  // queue is ready.
  *R(VIRTIO_MMIO_ಸರತಿ_READY) = 0x1;

  // all NUM descriptors start out unused.
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_2; i++)
    disk.ಬಿಡುಗಡೆ[i] = 1;

  // tell device we're completely ready.
  ಸ್ಥಿತಿ_2 |= VIRTIO_CONFIG_S_DRIVER_OK;
  *R(VIRTIO_MMIO_ಸ್ಥಿತಿ) = ಸ್ಥಿತಿ_2;

  // plic.c and trap.c arrange for interrupts from VIRTIO0_IRQ.
}

// find a free descriptor, mark it non-free, return its index.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
alloc_desc()
{
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_2; i++) {
    ಆದರೆ (disk.ಬಿಡುಗಡೆ[i]) {
      disk.ಬಿಡುಗಡೆ[i] = 0;
      ಹಿಂತಿರುಗಿ i;
    }
  }
  ಹಿಂತಿರುಗಿ -1;
}

// mark a descriptor as free.
ಸ್ಥಿರ ಶೂನ್ಯ
free_desc(ಪೂರ್ಣಾಂಕ i)
{
  ಆದರೆ (i >= ಅಂಕೆ_2)
    panic("free_desc 1");
  ಆದರೆ (disk.ಬಿಡುಗಡೆ[i])
    panic("free_desc 2");
  disk.desc[i].addr = 0;
  disk.desc[i].len = 0;
  disk.desc[i].flags = 0;
  disk.desc[i].ಮುಂದಿನ = 0;
  disk.ಬಿಡುಗಡೆ[i] = 1;
  wakeup(&disk.ಬಿಡುಗಡೆ[0]);
}

// free a chain of descriptors.
ಸ್ಥಿರ ಶೂನ್ಯ
free_chain(ಪೂರ್ಣಾಂಕ i)
{
  ಆಗ (1) {
    ಪೂರ್ಣಾಂಕ flag = disk.desc[i].flags;
    ಪೂರ್ಣಾಂಕ nxt = disk.desc[i].ಮುಂದಿನ;
    free_desc(i);
    ಆದರೆ (flag & VRING_DESC_F_ಮುಂದಿನ)
      i = nxt;
    ಇಲ್ಲದಿದ್ದರೆ
      ನಿಲ್ಲಿಸು;
  }
}

// allocate three descriptors (they need not be contiguous).
// disk transfers always use three descriptors.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
alloc3_desc(ಪೂರ್ಣಾಂಕ *idx)
{
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++) {
    idx[i] = alloc_desc();
    ಆದರೆ (idx[i] < 0) {
      ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < i; j++)
        free_desc(idx[j]);
      ಹಿಂತಿರುಗಿ -1;
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ
virtio_disk_rw(ರಚನೆ buf *b, ಪೂರ್ಣಾಂಕ ಲಿಖ)
{
  uint64 sector = b->blockno * (BSIZE / 512);

  acquire(&disk.vdisk_ಬೀಗ);

  // the spec's Section 5.2 says that legacy block operations use
  // three descriptors: one for type/reserved/sector, one for the
  // data, one for a 1-byte status result.

  // allocate the three descriptors.
  ಪೂರ್ಣಾಂಕ idx[3];
  ಆಗ (1) {
    ಆದರೆ (alloc3_desc(idx) == 0) {
      ನಿಲ್ಲಿಸು;
    }
    sleep_prepare(&disk.ಬಿಡುಗಡೆ[0]);
    release(&disk.vdisk_ಬೀಗ);
    sleep();
    acquire(&disk.vdisk_ಬೀಗ);
  }

  // format the three descriptors.
  // qemu's virtio-blk.c reads them.

  ರಚನೆ virtio_blk_req *buf0 = &disk.ops[idx[0]];

  ಆದರೆ (ಲಿಖ)
    buf0->type = VIRTIO_BLK_T_ಹೊರಹೋಗು; // write the disk
  ಇಲ್ಲದಿದ್ದರೆ
    buf0->type = VIRTIO_BLK_T_ಒಳ; // read the disk
  buf0->reserved = 0;
  buf0->sector = sector;

  disk.desc[idx[0]].addr = (uint64)buf0;
  disk.desc[idx[0]].len = ಗಾತ್ರ(ರಚನೆ virtio_blk_req);
  disk.desc[idx[0]].flags = VRING_DESC_F_ಮುಂದಿನ;
  disk.desc[idx[0]].ಮುಂದಿನ = idx[1];

  disk.desc[idx[1]].addr = (uint64)b->ದತ್ತಾಂಶ;
  disk.desc[idx[1]].len = BSIZE;
  ಆದರೆ (ಲಿಖ)
    disk.desc[idx[1]].flags = 0; // device reads b->data
  ಇಲ್ಲದಿದ್ದರೆ
    disk.desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
  disk.desc[idx[1]].flags |= VRING_DESC_F_ಮುಂದಿನ;
  disk.desc[idx[1]].ಮುಂದಿನ = idx[2];

  disk.info[idx[0]].ಸ್ಥಿತಿ_2 = 0xff; // device writes 0 on success
  disk.desc[idx[2]].addr = (uint64)&disk.info[idx[0]].ಸ್ಥಿತಿ_2;
  disk.desc[idx[2]].len = 1;
  disk.desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
  disk.desc[idx[2]].ಮುಂದಿನ = 0;

  // record struct buf for virtio_disk_intr().
  b->disk = 1;
  disk.info[idx[0]].b = b;

  // tell the device the first index in our chain of descriptors.
  disk.avail->ring[disk.avail->idx % ಅಂಕೆ_2] = idx[0];

  io_fence();

  // tell the device another avail ring entry is available.
  disk.avail->idx += 1; // not % NUM ...

  io_fence();

  *R(VIRTIO_MMIO_ಸರತಿ_NOTIFY) = 0; // value is queue number

  // Wait for virtio_disk_intr() to say request has finished.
  ಆಗ (b->disk == 1) {
    sleep_prepare(b);
    release(&disk.vdisk_ಬೀಗ);
    sleep();
    acquire(&disk.vdisk_ಬೀಗ);
  }

  disk.info[idx[0]].b = 0;
  free_chain(idx[0]);

  release(&disk.vdisk_ಬೀಗ);
}

ಶೂನ್ಯ
virtio_disk_intr()
{
  acquire(&disk.vdisk_ಬೀಗ);

  // the device won't raise another interrupt until we tell it
  // we've seen this interrupt, which the following line does.
  // this may race with the device writing new entries to
  // the "used" ring, in which case we may process the new
  // completion entries in this interrupt, and have nothing to do
  // in the next interrupt, which is harmless.
  *R(VIRTIO_MMIO_ಅಡಚಣೆ_ACK) = *R(VIRTIO_MMIO_ಅಡಚಣೆ_ಸ್ಥಿತಿ) & 0x3;

  io_fence();

  // the device increments disk.used->idx when it
  // adds an entry to the used ring.

  ಆಗ (disk.ಬಳಸಿದ_idx != disk.ಬಳಸಿದ_2->idx) {
    io_fence();
    ಪೂರ್ಣಾಂಕ id = disk.ಬಳಸಿದ_2->ring[disk.ಬಳಸಿದ_idx % ಅಂಕೆ_2].id;

    ಆದರೆ (disk.info[id].ಸ್ಥಿತಿ_2 != 0)
      panic("virtio_disk_intr status");

    ರಚನೆ buf *b = disk.info[id].b;
    b->disk = 0; // disk is done with buf
    wakeup(b);

    disk.ಬಳಸಿದ_idx += 1;
  }

  release(&disk.vdisk_ಬೀಗ);
}
