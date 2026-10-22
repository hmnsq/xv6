//
// driver for qemu's virtio disk device.
// uses qemu's mmio interface to virtio.
//
// qemu ... -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
//

#ضمن "types.h"
#ضمن "riscv.h"
#ضمن "defs.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "spinlock.h"
#ضمن "sleeplock.h"
#ضمن "fs.h"
#ضمن "buf.h"
#ضمن "virtio.h"

// the address of virtio mmio register r.
#تعریف_کړه R(r) ((متطاير uint32 *)(VIRTIO0 + (r)))

ساكن بنية disk {
  // a set (not a ring) of DMA descriptors, with which the
  // driver tells the device where to read and write individual
  // disk operations. there are NUM descriptors.
  // most commands consist of a "chain" (a linked list) of a couple of
  // these descriptors.
  بنية virtq_desc *desc;

  // a ring in which the driver writes descriptor numbers
  // that the driver would like the device to process.  it only
  // includes the head descriptor of each chain. the ring has
  // NUM elements.
  بنية virtq_avail *avail;

  // a ring in which the device writes descriptor numbers that
  // the device has finished processing (just the head of each chain).
  // there are NUM used ring entries.
  بنية virtq_کارول_شوی *کارول_شوی_2;

  // our own book-keeping.
  حرف حرر[عدد_2];  // is a descriptor free?
  uint16 کارول_شوی_idx; // we've looked this far in used[2..NUM].

  // track info about in-flight operations,
  // for use when completion interrupt arrives.
  // indexed by first descriptor index of chain.
  بنية {
    بنية buf *b;
    حرف حالت;
  } info[عدد_2];

  // disk command headers.
  // one-for-one with descriptors, for convenience.
  بنية virtio_blk_req ops[عدد_2];

  بنية spinlock vdisk_قلف;

} disk;

فراغ
virtio_disk_init(فراغ)
{
  uint32 حالت = 0;

  initlock(&disk.vdisk_قلف, "virtio_disk");

  إذا (*R(VIRTIO_MMIO_MAGIC_ارزښت) != 0x74726976 ||
      *R(VIRTIO_MMIO_نسخه) != 2 || *R(VIRTIO_MMIO_DEVICE_ID) != 2 ||
      *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551) {
    panic("could not find virtio disk");
  }

  // reset device
  *R(VIRTIO_MMIO_حالت) = حالت;

  // set ACKNOWLEDGE status bit
  حالت |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
  *R(VIRTIO_MMIO_حالت) = حالت;

  // set DRIVER status bit
  حالت |= VIRTIO_CONFIG_S_DRIVER;
  *R(VIRTIO_MMIO_حالت) = حالت;

  // negotiate features
  uint64 features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
  features &= ~(1 << VIRTIO_BLK_F_RO);
  features &= ~(1 << VIRTIO_BLK_F_SCSI);
  features &= ~(1 << VIRTIO_BLK_F_FLUSH);
  features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
  features &= ~(1 << VIRTIO_BLK_F_MQ);
  features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
  features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
  features &= ~(1 << VIRTIO_RING_F_غیرمستقیم_DESC);
  *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

  // tell device that feature negotiation is complete.
  حالت |= VIRTIO_CONFIG_S_FEATURES_OK;
  *R(VIRTIO_MMIO_حالت) = حالت;

  // re-read status to ensure FEATURES_OK is set.
  حالت = *R(VIRTIO_MMIO_حالت);
  إذا (!(حالت & VIRTIO_CONFIG_S_FEATURES_OK))
    panic("virtio disk FEATURES_OK unset");

  // initialize queue 0.
  *R(VIRTIO_MMIO_کتار_SEL) = 0;

  // ensure queue 0 is not in use.
  إذا (*R(VIRTIO_MMIO_کتار_READY))
    panic("virtio disk should not be ready");

  // check maximum queue size.
  uint32 max = *R(VIRTIO_MMIO_کتار_عدد_MAX);
  إذا (max == 0)
    panic("virtio disk has no queue 0");
  إذا (max < عدد_2)
    panic("virtio disk max queue too short");

  // allocate and zero queue memory.
  disk.desc = kalloc();
  disk.avail = kalloc();
  disk.کارول_شوی_2 = kalloc();
  إذا (!disk.desc || !disk.avail || !disk.کارول_شوی_2)
    panic("virtio disk kalloc");
  memset(disk.desc, 0, PGSIZE);
  memset(disk.avail, 0, PGSIZE);
  memset(disk.کارول_شوی_2, 0, PGSIZE);

  // set queue size.
  *R(VIRTIO_MMIO_کتار_عدد) = عدد_2;

  // write physical addresses.
  *R(VIRTIO_MMIO_کتار_DESC_ټیټ) = (uint64)disk.desc;
  *R(VIRTIO_MMIO_کتار_DESC_لوړ) = (uint64)disk.desc >> 32;
  *R(VIRTIO_MMIO_DRIVER_DESC_ټیټ) = (uint64)disk.avail;
  *R(VIRTIO_MMIO_DRIVER_DESC_لوړ) = (uint64)disk.avail >> 32;
  *R(VIRTIO_MMIO_DEVICE_DESC_ټیټ) = (uint64)disk.کارول_شوی_2;
  *R(VIRTIO_MMIO_DEVICE_DESC_لوړ) = (uint64)disk.کارول_شوی_2 >> 32;

  // queue is ready.
  *R(VIRTIO_MMIO_کتار_READY) = 0x1;

  // all NUM descriptors start out unused.
  لكل (صحيح i = 0; i < عدد_2; i++)
    disk.حرر[i] = 1;

  // tell device we're completely ready.
  حالت |= VIRTIO_CONFIG_S_DRIVER_OK;
  *R(VIRTIO_MMIO_حالت) = حالت;

  // plic.c and trap.c arrange for interrupts from VIRTIO0_IRQ.
}

// find a free descriptor, mark it non-free, return its index.
ساكن صحيح
alloc_desc()
{
  لكل (صحيح i = 0; i < عدد_2; i++) {
    إذا (disk.حرر[i]) {
      disk.حرر[i] = 0;
      أرجع i;
    }
  }
  أرجع -1;
}

// mark a descriptor as free.
ساكن فراغ
free_desc(صحيح i)
{
  إذا (i >= عدد_2)
    panic("free_desc 1");
  إذا (disk.حرر[i])
    panic("free_desc 2");
  disk.desc[i].addr = 0;
  disk.desc[i].len = 0;
  disk.desc[i].flags = 0;
  disk.desc[i].بل = 0;
  disk.حرر[i] = 1;
  wakeup(&disk.حرر[0]);
}

// free a chain of descriptors.
ساكن فراغ
free_chain(صحيح i)
{
  طالما (1) {
    صحيح flag = disk.desc[i].flags;
    صحيح nxt = disk.desc[i].بل;
    free_desc(i);
    إذا (flag & VRING_DESC_F_بل)
      i = nxt;
    وإلا
      اكسر;
  }
}

// allocate three descriptors (they need not be contiguous).
// disk transfers always use three descriptors.
ساكن صحيح
alloc3_desc(صحيح *idx)
{
  لكل (صحيح i = 0; i < 3; i++) {
    idx[i] = alloc_desc();
    إذا (idx[i] < 0) {
      لكل (صحيح j = 0; j < i; j++)
        free_desc(idx[j]);
      أرجع -1;
    }
  }
  أرجع 0;
}

فراغ
virtio_disk_rw(بنية buf *b, صحيح اكتب)
{
  uint64 sector = b->blockno * (BSIZE / 512);

  acquire(&disk.vdisk_قلف);

  // the spec's Section 5.2 says that legacy block operations use
  // three descriptors: one for type/reserved/sector, one for the
  // data, one for a 1-byte status result.

  // allocate the three descriptors.
  صحيح idx[3];
  طالما (1) {
    إذا (alloc3_desc(idx) == 0) {
      اكسر;
    }
    sleep_prepare(&disk.حرر[0]);
    release(&disk.vdisk_قلف);
    sleep();
    acquire(&disk.vdisk_قلف);
  }

  // format the three descriptors.
  // qemu's virtio-blk.c reads them.

  بنية virtio_blk_req *buf0 = &disk.ops[idx[0]];

  إذا (اكتب)
    buf0->type = VIRTIO_BLK_T_بهر; // write the disk
  وإلا
    buf0->type = VIRTIO_BLK_T_دننه; // read the disk
  buf0->reserved = 0;
  buf0->sector = sector;

  disk.desc[idx[0]].addr = (uint64)buf0;
  disk.desc[idx[0]].len = حجم(بنية virtio_blk_req);
  disk.desc[idx[0]].flags = VRING_DESC_F_بل;
  disk.desc[idx[0]].بل = idx[1];

  disk.desc[idx[1]].addr = (uint64)b->معلومات;
  disk.desc[idx[1]].len = BSIZE;
  إذا (اكتب)
    disk.desc[idx[1]].flags = 0; // device reads b->data
  وإلا
    disk.desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
  disk.desc[idx[1]].flags |= VRING_DESC_F_بل;
  disk.desc[idx[1]].بل = idx[2];

  disk.info[idx[0]].حالت = 0xff; // device writes 0 on success
  disk.desc[idx[2]].addr = (uint64)&disk.info[idx[0]].حالت;
  disk.desc[idx[2]].len = 1;
  disk.desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
  disk.desc[idx[2]].بل = 0;

  // record struct buf for virtio_disk_intr().
  b->disk = 1;
  disk.info[idx[0]].b = b;

  // tell the device the first index in our chain of descriptors.
  disk.avail->ring[disk.avail->idx % عدد_2] = idx[0];

  io_fence();

  // tell the device another avail ring entry is available.
  disk.avail->idx += 1; // not % NUM ...

  io_fence();

  *R(VIRTIO_MMIO_کتار_NOTIFY) = 0; // value is queue number

  // Wait for virtio_disk_intr() to say request has finished.
  طالما (b->disk == 1) {
    sleep_prepare(b);
    release(&disk.vdisk_قلف);
    sleep();
    acquire(&disk.vdisk_قلف);
  }

  disk.info[idx[0]].b = 0;
  free_chain(idx[0]);

  release(&disk.vdisk_قلف);
}

فراغ
virtio_disk_intr()
{
  acquire(&disk.vdisk_قلف);

  // the device won't raise another interrupt until we tell it
  // we've seen this interrupt, which the following line does.
  // this may race with the device writing new entries to
  // the "used" ring, in which case we may process the new
  // completion entries in this interrupt, and have nothing to do
  // in the next interrupt, which is harmless.
  *R(VIRTIO_MMIO_مداخله_ACK) = *R(VIRTIO_MMIO_مداخله_حالت) & 0x3;

  io_fence();

  // the device increments disk.used->idx when it
  // adds an entry to the used ring.

  طالما (disk.کارول_شوی_idx != disk.کارول_شوی_2->idx) {
    io_fence();
    صحيح id = disk.کارول_شوی_2->ring[disk.کارول_شوی_idx % عدد_2].id;

    إذا (disk.info[id].حالت != 0)
      panic("virtio_disk_intr status");

    بنية buf *b = disk.info[id].b;
    b->disk = 0; // disk is done with buf
    wakeup(b);

    disk.کارول_شوی_idx += 1;
  }

  release(&disk.vdisk_قلف);
}
