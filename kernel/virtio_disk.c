//
// driver for qemu's virtio disk device.
// uses qemu's mmio interface to virtio.
//
// qemu ... -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
//

#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "spinlock.h"
#شامل "sleeplock.h"
#شامل "fs.h"
#شامل "buf.h"
#شامل "virtio.h"

// the address of virtio mmio register r.
#define R(r) ((فرار uint32 *)(VIRTIO0 + (r)))

ایستا ساختار disk {
  // a set (not a ring) of DMA descriptors, with which the
  // driver tells the device where to read and write individual
  // disk operations. there are NUM descriptors.
  // most commands consist of a "chain" (a linked list) of a couple of
  // these descriptors.
  ساختار virtq_desc *desc;

  // a ring in which the driver writes descriptor numbers
  // that the driver would like the device to process.  it only
  // includes the head descriptor of each chain. the ring has
  // NUM elements.
  ساختار virtq_avail *avail;

  // a ring in which the device writes descriptor numbers that
  // the device has finished processing (just the head of each chain).
  // there are NUM used ring entries.
  ساختار virtq_used *used;

  // our own book-keeping.
  نویسه آزاد[رقم_2];  // is a descriptor free?
  uint16 used_idx; // we've looked this far in used[2..NUM].

  // track info about in-flight operations,
  // for use when completion interrupt arrives.
  // indexed by first descriptor index of chain.
  ساختار {
    ساختار buf *b;
    نویسه status;
  } info[رقم_2];

  // disk command headers.
  // one-for-one with descriptors, for convenience.
  ساختار virtio_blk_req ops[رقم_2];

  ساختار spinlock vdisk_lock;

} disk;

تهی
virtio_disk_init(تهی)
{
  uint32 status = 0;

  initlock(&disk.vdisk_lock, "virtio_disk");

  اگر (*R(VIRTIO_MMIO_MAGIC_مقدار) != 0x74726976 ||
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
  اگر (!(status & VIRTIO_CONFIG_S_FEATURES_OK))
    panic("virtio disk FEATURES_OK unset");

  // initialize queue 0.
  *R(VIRTIO_MMIO_QUEUE_SEL) = 0;

  // ensure queue 0 is not in use.
  اگر (*R(VIRTIO_MMIO_QUEUE_READY))
    panic("virtio disk should not be ready");

  // check maximum queue size.
  uint32 max = *R(VIRTIO_MMIO_QUEUE_رقم_MAX);
  اگر (max == 0)
    panic("virtio disk has no queue 0");
  اگر (max < رقم_2)
    panic("virtio disk max queue too short");

  // allocate and zero queue memory.
  disk.desc = kalloc();
  disk.avail = kalloc();
  disk.used = kalloc();
  اگر (!disk.desc || !disk.avail || !disk.used)
    panic("virtio disk kalloc");
  memset(disk.desc, 0, PGSIZE);
  memset(disk.avail, 0, PGSIZE);
  memset(disk.used, 0, PGSIZE);

  // set queue size.
  *R(VIRTIO_MMIO_QUEUE_رقم) = رقم_2;

  // write physical addresses.
  *R(VIRTIO_MMIO_QUEUE_DESC_کم) = (uint64)disk.desc;
  *R(VIRTIO_MMIO_QUEUE_DESC_بالا) = (uint64)disk.desc >> 32;
  *R(VIRTIO_MMIO_DRIVER_DESC_کم) = (uint64)disk.avail;
  *R(VIRTIO_MMIO_DRIVER_DESC_بالا) = (uint64)disk.avail >> 32;
  *R(VIRTIO_MMIO_DEVICE_DESC_کم) = (uint64)disk.used;
  *R(VIRTIO_MMIO_DEVICE_DESC_بالا) = (uint64)disk.used >> 32;

  // queue is ready.
  *R(VIRTIO_MMIO_QUEUE_READY) = 0x1;

  // all NUM descriptors start out unused.
  برای (صحیح i = 0; i < رقم_2; i++)
    disk.آزاد[i] = 1;

  // tell device we're completely ready.
  status |= VIRTIO_CONFIG_S_DRIVER_OK;
  *R(VIRTIO_MMIO_STATUS) = status;

  // plic.c and trap.c arrange for interrupts from VIRTIO0_IRQ.
}

// find a free descriptor, mark it non-free, return its index.
ایستا صحیح
alloc_desc()
{
  برای (صحیح i = 0; i < رقم_2; i++) {
    اگر (disk.آزاد[i]) {
      disk.آزاد[i] = 0;
      بازگشت i;
    }
  }
  بازگشت -1;
}

// mark a descriptor as free.
ایستا تهی
free_desc(صحیح i)
{
  اگر (i >= رقم_2)
    panic("free_desc 1");
  اگر (disk.آزاد[i])
    panic("free_desc 2");
  disk.desc[i].addr = 0;
  disk.desc[i].len = 0;
  disk.desc[i].flags = 0;
  disk.desc[i].next = 0;
  disk.آزاد[i] = 1;
  wakeup(&disk.آزاد[0]);
}

// free a chain of descriptors.
ایستا تهی
free_chain(صحیح i)
{
  وقتی (1) {
    صحیح flag = disk.desc[i].flags;
    صحیح nxt = disk.desc[i].next;
    free_desc(i);
    اگر (flag & VRING_DESC_F_NEXT)
      i = nxt;
    وگرنه
      بشکن;
  }
}

// allocate three descriptors (they need not be contiguous).
// disk transfers always use three descriptors.
ایستا صحیح
alloc3_desc(صحیح *idx)
{
  برای (صحیح i = 0; i < 3; i++) {
    idx[i] = alloc_desc();
    اگر (idx[i] < 0) {
      برای (صحیح j = 0; j < i; j++)
        free_desc(idx[j]);
      بازگشت -1;
    }
  }
  بازگشت 0;
}

تهی
virtio_disk_rw(ساختار buf *b, صحیح اكتب)
{
  uint64 sector = b->blockno * (BSIZE / 512);

  acquire(&disk.vdisk_lock);

  // the spec's Section 5.2 says that legacy block operations use
  // three descriptors: one for type/reserved/sector, one for the
  // data, one for a 1-byte status result.

  // allocate the three descriptors.
  صحیح idx[3];
  وقتی (1) {
    اگر (alloc3_desc(idx) == 0) {
      بشکن;
    }
    sleep_prepare(&disk.آزاد[0]);
    release(&disk.vdisk_lock);
    sleep();
    acquire(&disk.vdisk_lock);
  }

  // format the three descriptors.
  // qemu's virtio-blk.c reads them.

  ساختار virtio_blk_req *buf0 = &disk.ops[idx[0]];

  اگر (اكتب)
    buf0->type = VIRTIO_BLK_T_OUT; // write the disk
  وگرنه
    buf0->type = VIRTIO_BLK_T_IN; // read the disk
  buf0->reserved = 0;
  buf0->sector = sector;

  disk.desc[idx[0]].addr = (uint64)buf0;
  disk.desc[idx[0]].len = اندازه(ساختار virtio_blk_req);
  disk.desc[idx[0]].flags = VRING_DESC_F_NEXT;
  disk.desc[idx[0]].next = idx[1];

  disk.desc[idx[1]].addr = (uint64)b->data;
  disk.desc[idx[1]].len = BSIZE;
  اگر (اكتب)
    disk.desc[idx[1]].flags = 0; // device reads b->data
  وگرنه
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
  disk.avail->ring[disk.avail->idx % رقم_2] = idx[0];

  io_fence();

  // tell the device another avail ring entry is available.
  disk.avail->idx += 1; // not % NUM ...

  io_fence();

  *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0; // value is queue number

  // Wait for virtio_disk_intr() to say request has finished.
  وقتی (b->disk == 1) {
    sleep_prepare(b);
    release(&disk.vdisk_lock);
    sleep();
    acquire(&disk.vdisk_lock);
  }

  disk.info[idx[0]].b = 0;
  free_chain(idx[0]);

  release(&disk.vdisk_lock);
}

تهی
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

  وقتی (disk.used_idx != disk.used->idx) {
    io_fence();
    صحیح id = disk.used->ring[disk.used_idx % رقم_2].id;

    اگر (disk.info[id].status != 0)
      panic("virtio_disk_intr status");

    ساختار buf *b = disk.info[id].b;
    b->disk = 0; // disk is done with buf
    wakeup(b);

    disk.used_idx += 1;
  }

  release(&disk.vdisk_lock);
}
