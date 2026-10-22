//
// driver for qemu's virtio disk device.
// uses qemu's mmio interface to virtio.
//
// qemu ... -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
//

#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sleeplock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "buf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "virtio.h"

// the address of virtio mmio register r.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ R(r) ((ਅਸਥਿਰ uint32 *)(VIRTIO0 + (r)))

ਸਥਿਰ ਬਣਤਰ disk {
  // a set (not a ring) of DMA descriptors, with which the
  // driver tells the device where to read and write individual
  // disk operations. there are NUM descriptors.
  // most commands consist of a "chain" (a linked list) of a couple of
  // these descriptors.
  ਬਣਤਰ virtq_desc *desc;

  // a ring in which the driver writes descriptor numbers
  // that the driver would like the device to process.  it only
  // includes the head descriptor of each chain. the ring has
  // NUM elements.
  ਬਣਤਰ virtq_avail *avail;

  // a ring in which the device writes descriptor numbers that
  // the device has finished processing (just the head of each chain).
  // there are NUM used ring entries.
  ਬਣਤਰ virtq_ਵਰਤਿਆ *ਵਰਤਿਆ_2;

  // our own book-keeping.
  ਅੱਖਰ ਮੁਕਤ[ਅੰਕ_2];  // is a descriptor free?
  uint16 ਵਰਤਿਆ_idx; // we've looked this far in used[2..NUM].

  // track info about in-flight operations,
  // for use when completion interrupt arrives.
  // indexed by first descriptor index of chain.
  ਬਣਤਰ {
    ਬਣਤਰ buf *b;
    ਅੱਖਰ ਸਥਿਤੀ;
  } info[ਅੰਕ_2];

  // disk command headers.
  // one-for-one with descriptors, for convenience.
  ਬਣਤਰ virtio_blk_req ops[ਅੰਕ_2];

  ਬਣਤਰ spinlock vdisk_ਤਾਲਾ;

} disk;

ਖਾਲੀ
virtio_disk_init(ਖਾਲੀ)
{
  uint32 ਸਥਿਤੀ = 0;

  initlock(&disk.vdisk_ਤਾਲਾ, "virtio_disk");

  ਜੇ (*R(VIRTIO_MMIO_MAGIC_ਮੁੱਲ) != 0x74726976 ||
      *R(VIRTIO_MMIO_ਸੰਸਕਰਣ) != 2 || *R(VIRTIO_MMIO_DEVICE_ID) != 2 ||
      *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551) {
    panic("could not find virtio disk");
  }

  // reset device
  *R(VIRTIO_MMIO_ਸਥਿਤੀ) = ਸਥਿਤੀ;

  // set ACKNOWLEDGE status bit
  ਸਥਿਤੀ |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
  *R(VIRTIO_MMIO_ਸਥਿਤੀ) = ਸਥਿਤੀ;

  // set DRIVER status bit
  ਸਥਿਤੀ |= VIRTIO_CONFIG_S_DRIVER;
  *R(VIRTIO_MMIO_ਸਥਿਤੀ) = ਸਥਿਤੀ;

  // negotiate features
  uint64 features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
  features &= ~(1 << VIRTIO_BLK_F_RO);
  features &= ~(1 << VIRTIO_BLK_F_SCSI);
  features &= ~(1 << VIRTIO_BLK_F_FLUSH);
  features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
  features &= ~(1 << VIRTIO_BLK_F_MQ);
  features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
  features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
  features &= ~(1 << VIRTIO_RING_F_ਅਸਿੱਧਾ_DESC);
  *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

  // tell device that feature negotiation is complete.
  ਸਥਿਤੀ |= VIRTIO_CONFIG_S_FEATURES_OK;
  *R(VIRTIO_MMIO_ਸਥਿਤੀ) = ਸਥਿਤੀ;

  // re-read status to ensure FEATURES_OK is set.
  ਸਥਿਤੀ = *R(VIRTIO_MMIO_ਸਥਿਤੀ);
  ਜੇ (!(ਸਥਿਤੀ & VIRTIO_CONFIG_S_FEATURES_OK))
    panic("virtio disk FEATURES_OK unset");

  // initialize queue 0.
  *R(VIRTIO_MMIO_ਕਤਾਰ_SEL) = 0;

  // ensure queue 0 is not in use.
  ਜੇ (*R(VIRTIO_MMIO_ਕਤਾਰ_READY))
    panic("virtio disk should not be ready");

  // check maximum queue size.
  uint32 max = *R(VIRTIO_MMIO_ਕਤਾਰ_ਅੰਕ_MAX);
  ਜੇ (max == 0)
    panic("virtio disk has no queue 0");
  ਜੇ (max < ਅੰਕ_2)
    panic("virtio disk max queue too short");

  // allocate and zero queue memory.
  disk.desc = kalloc();
  disk.avail = kalloc();
  disk.ਵਰਤਿਆ_2 = kalloc();
  ਜੇ (!disk.desc || !disk.avail || !disk.ਵਰਤਿਆ_2)
    panic("virtio disk kalloc");
  memset(disk.desc, 0, PGSIZE);
  memset(disk.avail, 0, PGSIZE);
  memset(disk.ਵਰਤਿਆ_2, 0, PGSIZE);

  // set queue size.
  *R(VIRTIO_MMIO_ਕਤਾਰ_ਅੰਕ) = ਅੰਕ_2;

  // write physical addresses.
  *R(VIRTIO_MMIO_ਕਤਾਰ_DESC_ਨੀਵਾਂ) = (uint64)disk.desc;
  *R(VIRTIO_MMIO_ਕਤਾਰ_DESC_ਉੱਚਾ) = (uint64)disk.desc >> 32;
  *R(VIRTIO_MMIO_DRIVER_DESC_ਨੀਵਾਂ) = (uint64)disk.avail;
  *R(VIRTIO_MMIO_DRIVER_DESC_ਉੱਚਾ) = (uint64)disk.avail >> 32;
  *R(VIRTIO_MMIO_DEVICE_DESC_ਨੀਵਾਂ) = (uint64)disk.ਵਰਤਿਆ_2;
  *R(VIRTIO_MMIO_DEVICE_DESC_ਉੱਚਾ) = (uint64)disk.ਵਰਤਿਆ_2 >> 32;

  // queue is ready.
  *R(VIRTIO_MMIO_ਕਤਾਰ_READY) = 0x1;

  // all NUM descriptors start out unused.
  ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_2; i++)
    disk.ਮੁਕਤ[i] = 1;

  // tell device we're completely ready.
  ਸਥਿਤੀ |= VIRTIO_CONFIG_S_DRIVER_OK;
  *R(VIRTIO_MMIO_ਸਥਿਤੀ) = ਸਥਿਤੀ;

  // plic.c and trap.c arrange for interrupts from VIRTIO0_IRQ.
}

// find a free descriptor, mark it non-free, return its index.
ਸਥਿਰ ਪੂਰਨ
alloc_desc()
{
  ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_2; i++) {
    ਜੇ (disk.ਮੁਕਤ[i]) {
      disk.ਮੁਕਤ[i] = 0;
      ਵਾਪਸ i;
    }
  }
  ਵਾਪਸ -1;
}

// mark a descriptor as free.
ਸਥਿਰ ਖਾਲੀ
free_desc(ਪੂਰਨ i)
{
  ਜੇ (i >= ਅੰਕ_2)
    panic("free_desc 1");
  ਜੇ (disk.ਮੁਕਤ[i])
    panic("free_desc 2");
  disk.desc[i].addr = 0;
  disk.desc[i].len = 0;
  disk.desc[i].flags = 0;
  disk.desc[i].ਅਗਲਾ = 0;
  disk.ਮੁਕਤ[i] = 1;
  wakeup(&disk.ਮੁਕਤ[0]);
}

// free a chain of descriptors.
ਸਥਿਰ ਖਾਲੀ
free_chain(ਪੂਰਨ i)
{
  ਜਦੋਂ (1) {
    ਪੂਰਨ flag = disk.desc[i].flags;
    ਪੂਰਨ nxt = disk.desc[i].ਅਗਲਾ;
    free_desc(i);
    ਜੇ (flag & VRING_DESC_F_ਅਗਲਾ)
      i = nxt;
    ਨਹੀਂਤਾਂ
      ਰੋਕੋ;
  }
}

// allocate three descriptors (they need not be contiguous).
// disk transfers always use three descriptors.
ਸਥਿਰ ਪੂਰਨ
alloc3_desc(ਪੂਰਨ *idx)
{
  ਲਈ (ਪੂਰਨ i = 0; i < 3; i++) {
    idx[i] = alloc_desc();
    ਜੇ (idx[i] < 0) {
      ਲਈ (ਪੂਰਨ j = 0; j < i; j++)
        free_desc(idx[j]);
      ਵਾਪਸ -1;
    }
  }
  ਵਾਪਸ 0;
}

ਖਾਲੀ
virtio_disk_rw(ਬਣਤਰ buf *b, ਪੂਰਨ ਲਿਖ)
{
  uint64 sector = b->blockno * (BSIZE / 512);

  acquire(&disk.vdisk_ਤਾਲਾ);

  // the spec's Section 5.2 says that legacy block operations use
  // three descriptors: one for type/reserved/sector, one for the
  // data, one for a 1-byte status result.

  // allocate the three descriptors.
  ਪੂਰਨ idx[3];
  ਜਦੋਂ (1) {
    ਜੇ (alloc3_desc(idx) == 0) {
      ਰੋਕੋ;
    }
    sleep_prepare(&disk.ਮੁਕਤ[0]);
    release(&disk.vdisk_ਤਾਲਾ);
    sleep();
    acquire(&disk.vdisk_ਤਾਲਾ);
  }

  // format the three descriptors.
  // qemu's virtio-blk.c reads them.

  ਬਣਤਰ virtio_blk_req *buf0 = &disk.ops[idx[0]];

  ਜੇ (ਲਿਖ)
    buf0->type = VIRTIO_BLK_T_ਬਾਹਰਲਾ; // write the disk
  ਨਹੀਂਤਾਂ
    buf0->type = VIRTIO_BLK_T_ਵਿੱਚ; // read the disk
  buf0->reserved = 0;
  buf0->sector = sector;

  disk.desc[idx[0]].addr = (uint64)buf0;
  disk.desc[idx[0]].len = ਆਕਾਰ(ਬਣਤਰ virtio_blk_req);
  disk.desc[idx[0]].flags = VRING_DESC_F_ਅਗਲਾ;
  disk.desc[idx[0]].ਅਗਲਾ = idx[1];

  disk.desc[idx[1]].addr = (uint64)b->ਅੰਕੜੇ;
  disk.desc[idx[1]].len = BSIZE;
  ਜੇ (ਲਿਖ)
    disk.desc[idx[1]].flags = 0; // device reads b->data
  ਨਹੀਂਤਾਂ
    disk.desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
  disk.desc[idx[1]].flags |= VRING_DESC_F_ਅਗਲਾ;
  disk.desc[idx[1]].ਅਗਲਾ = idx[2];

  disk.info[idx[0]].ਸਥਿਤੀ = 0xff; // device writes 0 on success
  disk.desc[idx[2]].addr = (uint64)&disk.info[idx[0]].ਸਥਿਤੀ;
  disk.desc[idx[2]].len = 1;
  disk.desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
  disk.desc[idx[2]].ਅਗਲਾ = 0;

  // record struct buf for virtio_disk_intr().
  b->disk = 1;
  disk.info[idx[0]].b = b;

  // tell the device the first index in our chain of descriptors.
  disk.avail->ring[disk.avail->idx % ਅੰਕ_2] = idx[0];

  io_fence();

  // tell the device another avail ring entry is available.
  disk.avail->idx += 1; // not % NUM ...

  io_fence();

  *R(VIRTIO_MMIO_ਕਤਾਰ_NOTIFY) = 0; // value is queue number

  // Wait for virtio_disk_intr() to say request has finished.
  ਜਦੋਂ (b->disk == 1) {
    sleep_prepare(b);
    release(&disk.vdisk_ਤਾਲਾ);
    sleep();
    acquire(&disk.vdisk_ਤਾਲਾ);
  }

  disk.info[idx[0]].b = 0;
  free_chain(idx[0]);

  release(&disk.vdisk_ਤਾਲਾ);
}

ਖਾਲੀ
virtio_disk_intr()
{
  acquire(&disk.vdisk_ਤਾਲਾ);

  // the device won't raise another interrupt until we tell it
  // we've seen this interrupt, which the following line does.
  // this may race with the device writing new entries to
  // the "used" ring, in which case we may process the new
  // completion entries in this interrupt, and have nothing to do
  // in the next interrupt, which is harmless.
  *R(VIRTIO_MMIO_ਰੁਕਾਵਟ_ACK) = *R(VIRTIO_MMIO_ਰੁਕਾਵਟ_ਸਥਿਤੀ) & 0x3;

  io_fence();

  // the device increments disk.used->idx when it
  // adds an entry to the used ring.

  ਜਦੋਂ (disk.ਵਰਤਿਆ_idx != disk.ਵਰਤਿਆ_2->idx) {
    io_fence();
    ਪੂਰਨ id = disk.ਵਰਤਿਆ_2->ring[disk.ਵਰਤਿਆ_idx % ਅੰਕ_2].id;

    ਜੇ (disk.info[id].ਸਥਿਤੀ != 0)
      panic("virtio_disk_intr status");

    ਬਣਤਰ buf *b = disk.info[id].b;
    b->disk = 0; // disk is done with buf
    wakeup(b);

    disk.ਵਰਤਿਆ_idx += 1;
  }

  release(&disk.vdisk_ਤਾਲਾ);
}
