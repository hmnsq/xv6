//
// driver for qemu's virtio disk device.
// uses qemu's mmio interface to virtio.
//
// qemu ... -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
//

#అన్తర్భావ "types.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "memlayout.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "sleeplock.h"
#అన్తర్భావ "fs.h"
#అన్తర్భావ "buf.h"
#అన్తర్భావ "virtio.h"

// the address of virtio mmio register r.
#నిర్వచించు R(r) ((అస్థిర uint32 *)(VIRTIO0 + (r)))

స్థిర నిర్మాణం disk {
  // a set (not a ring) of DMA descriptors, with which the
  // driver tells the device where to read and write individual
  // disk operations. there are NUM descriptors.
  // most commands consist of a "chain" (a linked list) of a couple of
  // these descriptors.
  నిర్మాణం virtq_desc *desc;

  // a ring in which the driver writes descriptor numbers
  // that the driver would like the device to process.  it only
  // includes the head descriptor of each chain. the ring has
  // NUM elements.
  నిర్మాణం virtq_avail *avail;

  // a ring in which the device writes descriptor numbers that
  // the device has finished processing (just the head of each chain).
  // there are NUM used ring entries.
  నిర్మాణం virtq_వాడిన *వాడిన_2;

  // our own book-keeping.
  అక్షరం విడుదల[అంకె_2];  // is a descriptor free?
  uint16 వాడిన_idx; // we've looked this far in used[2..NUM].

  // track info about in-flight operations,
  // for use when completion interrupt arrives.
  // indexed by first descriptor index of chain.
  నిర్మాణం {
    నిర్మాణం buf *b;
    అక్షరం స్థితి_2;
  } info[అంకె_2];

  // disk command headers.
  // one-for-one with descriptors, for convenience.
  నిర్మాణం virtio_blk_req ops[అంకె_2];

  నిర్మాణం spinlock vdisk_తాళం;

} disk;

శూన్య
virtio_disk_init(శూన్య)
{
  uint32 స్థితి_2 = 0;

  initlock(&disk.vdisk_తాళం, "virtio_disk");

  ఒకవేళ (*R(VIRTIO_MMIO_MAGIC_విలువ) != 0x74726976 ||
      *R(VIRTIO_MMIO_సంస్కరణ) != 2 || *R(VIRTIO_MMIO_DEVICE_ID) != 2 ||
      *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551) {
    panic("could not find virtio disk");
  }

  // reset device
  *R(VIRTIO_MMIO_స్థితి) = స్థితి_2;

  // set ACKNOWLEDGE status bit
  స్థితి_2 |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
  *R(VIRTIO_MMIO_స్థితి) = స్థితి_2;

  // set DRIVER status bit
  స్థితి_2 |= VIRTIO_CONFIG_S_DRIVER;
  *R(VIRTIO_MMIO_స్థితి) = స్థితి_2;

  // negotiate features
  uint64 features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
  features &= ~(1 << VIRTIO_BLK_F_RO);
  features &= ~(1 << VIRTIO_BLK_F_SCSI);
  features &= ~(1 << VIRTIO_BLK_F_FLUSH);
  features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
  features &= ~(1 << VIRTIO_BLK_F_MQ);
  features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
  features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
  features &= ~(1 << VIRTIO_RING_F_పరోక్ష_DESC);
  *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

  // tell device that feature negotiation is complete.
  స్థితి_2 |= VIRTIO_CONFIG_S_FEATURES_OK;
  *R(VIRTIO_MMIO_స్థితి) = స్థితి_2;

  // re-read status to ensure FEATURES_OK is set.
  స్థితి_2 = *R(VIRTIO_MMIO_స్థితి);
  ఒకవేళ (!(స్థితి_2 & VIRTIO_CONFIG_S_FEATURES_OK))
    panic("virtio disk FEATURES_OK unset");

  // initialize queue 0.
  *R(VIRTIO_MMIO_వరుస_SEL) = 0;

  // ensure queue 0 is not in use.
  ఒకవేళ (*R(VIRTIO_MMIO_వరుస_READY))
    panic("virtio disk should not be ready");

  // check maximum queue size.
  uint32 max = *R(VIRTIO_MMIO_వరుస_అంకె_MAX);
  ఒకవేళ (max == 0)
    panic("virtio disk has no queue 0");
  ఒకవేళ (max < అంకె_2)
    panic("virtio disk max queue too short");

  // allocate and zero queue memory.
  disk.desc = kalloc();
  disk.avail = kalloc();
  disk.వాడిన_2 = kalloc();
  ఒకవేళ (!disk.desc || !disk.avail || !disk.వాడిన_2)
    panic("virtio disk kalloc");
  memset(disk.desc, 0, PGSIZE);
  memset(disk.avail, 0, PGSIZE);
  memset(disk.వాడిన_2, 0, PGSIZE);

  // set queue size.
  *R(VIRTIO_MMIO_వరుస_అంకె) = అంకె_2;

  // write physical addresses.
  *R(VIRTIO_MMIO_వరుస_DESC_దిగువ) = (uint64)disk.desc;
  *R(VIRTIO_MMIO_వరుస_DESC_ఎగువ) = (uint64)disk.desc >> 32;
  *R(VIRTIO_MMIO_DRIVER_DESC_దిగువ) = (uint64)disk.avail;
  *R(VIRTIO_MMIO_DRIVER_DESC_ఎగువ) = (uint64)disk.avail >> 32;
  *R(VIRTIO_MMIO_DEVICE_DESC_దిగువ) = (uint64)disk.వాడిన_2;
  *R(VIRTIO_MMIO_DEVICE_DESC_ఎగువ) = (uint64)disk.వాడిన_2 >> 32;

  // queue is ready.
  *R(VIRTIO_MMIO_వరుస_READY) = 0x1;

  // all NUM descriptors start out unused.
  కొరకు (పూర్ణాంకం i = 0; i < అంకె_2; i++)
    disk.విడుదల[i] = 1;

  // tell device we're completely ready.
  స్థితి_2 |= VIRTIO_CONFIG_S_DRIVER_OK;
  *R(VIRTIO_MMIO_స్థితి) = స్థితి_2;

  // plic.c and trap.c arrange for interrupts from VIRTIO0_IRQ.
}

// find a free descriptor, mark it non-free, return its index.
స్థిర పూర్ణాంకం
alloc_desc()
{
  కొరకు (పూర్ణాంకం i = 0; i < అంకె_2; i++) {
    ఒకవేళ (disk.విడుదల[i]) {
      disk.విడుదల[i] = 0;
      తిరిగి i;
    }
  }
  తిరిగి -1;
}

// mark a descriptor as free.
స్థిర శూన్య
free_desc(పూర్ణాంకం i)
{
  ఒకవేళ (i >= అంకె_2)
    panic("free_desc 1");
  ఒకవేళ (disk.విడుదల[i])
    panic("free_desc 2");
  disk.desc[i].addr = 0;
  disk.desc[i].len = 0;
  disk.desc[i].flags = 0;
  disk.desc[i].తదుపరి = 0;
  disk.విడుదల[i] = 1;
  wakeup(&disk.విడుదల[0]);
}

// free a chain of descriptors.
స్థిర శూన్య
free_chain(పూర్ణాంకం i)
{
  అయితే (1) {
    పూర్ణాంకం flag = disk.desc[i].flags;
    పూర్ణాంకం nxt = disk.desc[i].తదుపరి;
    free_desc(i);
    ఒకవేళ (flag & VRING_DESC_F_తదుపరి)
      i = nxt;
    లేకపోతే
      ఆపు;
  }
}

// allocate three descriptors (they need not be contiguous).
// disk transfers always use three descriptors.
స్థిర పూర్ణాంకం
alloc3_desc(పూర్ణాంకం *idx)
{
  కొరకు (పూర్ణాంకం i = 0; i < 3; i++) {
    idx[i] = alloc_desc();
    ఒకవేళ (idx[i] < 0) {
      కొరకు (పూర్ణాంకం j = 0; j < i; j++)
        free_desc(idx[j]);
      తిరిగి -1;
    }
  }
  తిరిగి 0;
}

శూన్య
virtio_disk_rw(నిర్మాణం buf *b, పూర్ణాంకం లిఖ)
{
  uint64 sector = b->blockno * (BSIZE / 512);

  acquire(&disk.vdisk_తాళం);

  // the spec's Section 5.2 says that legacy block operations use
  // three descriptors: one for type/reserved/sector, one for the
  // data, one for a 1-byte status result.

  // allocate the three descriptors.
  పూర్ణాంకం idx[3];
  అయితే (1) {
    ఒకవేళ (alloc3_desc(idx) == 0) {
      ఆపు;
    }
    sleep_prepare(&disk.విడుదల[0]);
    release(&disk.vdisk_తాళం);
    sleep();
    acquire(&disk.vdisk_తాళం);
  }

  // format the three descriptors.
  // qemu's virtio-blk.c reads them.

  నిర్మాణం virtio_blk_req *buf0 = &disk.ops[idx[0]];

  ఒకవేళ (లిఖ)
    buf0->type = VIRTIO_BLK_T_బహిర్గతం; // write the disk
  లేకపోతే
    buf0->type = VIRTIO_BLK_T_లోపల; // read the disk
  buf0->reserved = 0;
  buf0->sector = sector;

  disk.desc[idx[0]].addr = (uint64)buf0;
  disk.desc[idx[0]].len = పరిమాణం(నిర్మాణం virtio_blk_req);
  disk.desc[idx[0]].flags = VRING_DESC_F_తదుపరి;
  disk.desc[idx[0]].తదుపరి = idx[1];

  disk.desc[idx[1]].addr = (uint64)b->దత్తాంశం;
  disk.desc[idx[1]].len = BSIZE;
  ఒకవేళ (లిఖ)
    disk.desc[idx[1]].flags = 0; // device reads b->data
  లేకపోతే
    disk.desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
  disk.desc[idx[1]].flags |= VRING_DESC_F_తదుపరి;
  disk.desc[idx[1]].తదుపరి = idx[2];

  disk.info[idx[0]].స్థితి_2 = 0xff; // device writes 0 on success
  disk.desc[idx[2]].addr = (uint64)&disk.info[idx[0]].స్థితి_2;
  disk.desc[idx[2]].len = 1;
  disk.desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
  disk.desc[idx[2]].తదుపరి = 0;

  // record struct buf for virtio_disk_intr().
  b->disk = 1;
  disk.info[idx[0]].b = b;

  // tell the device the first index in our chain of descriptors.
  disk.avail->ring[disk.avail->idx % అంకె_2] = idx[0];

  io_fence();

  // tell the device another avail ring entry is available.
  disk.avail->idx += 1; // not % NUM ...

  io_fence();

  *R(VIRTIO_MMIO_వరుస_NOTIFY) = 0; // value is queue number

  // Wait for virtio_disk_intr() to say request has finished.
  అయితే (b->disk == 1) {
    sleep_prepare(b);
    release(&disk.vdisk_తాళం);
    sleep();
    acquire(&disk.vdisk_తాళం);
  }

  disk.info[idx[0]].b = 0;
  free_chain(idx[0]);

  release(&disk.vdisk_తాళం);
}

శూన్య
virtio_disk_intr()
{
  acquire(&disk.vdisk_తాళం);

  // the device won't raise another interrupt until we tell it
  // we've seen this interrupt, which the following line does.
  // this may race with the device writing new entries to
  // the "used" ring, in which case we may process the new
  // completion entries in this interrupt, and have nothing to do
  // in the next interrupt, which is harmless.
  *R(VIRTIO_MMIO_అంతరాయం_ACK) = *R(VIRTIO_MMIO_అంతరాయం_స్థితి) & 0x3;

  io_fence();

  // the device increments disk.used->idx when it
  // adds an entry to the used ring.

  అయితే (disk.వాడిన_idx != disk.వాడిన_2->idx) {
    io_fence();
    పూర్ణాంకం id = disk.వాడిన_2->ring[disk.వాడిన_idx % అంకె_2].id;

    ఒకవేళ (disk.info[id].స్థితి_2 != 0)
      panic("virtio_disk_intr status");

    నిర్మాణం buf *b = disk.info[id].b;
    b->disk = 0; // disk is done with buf
    wakeup(b);

    disk.వాడిన_idx += 1;
  }

  release(&disk.vdisk_తాళం);
}
