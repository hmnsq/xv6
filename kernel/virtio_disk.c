//
// driver for qemu's virtio disk device.
// uses qemu's mmio interface to virtio.
//
// qemu ... -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
//

#अन्तर्भाव "types.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "sleeplock.h"
#अन्तर्भाव "fs.h"
#अन्तर्भाव "buf.h"
#अन्तर्भाव "virtio.h"

// the address of virtio mmio register r.
#परिभाषय R(r) ((चञ्चल uint32 *)(VIRTIO0 + (r)))

स्थिर संरचना disk {
  // a set (not a ring) of DMA descriptors, with which the
  // driver tells the device where to read and write individual
  // disk operations. there are NUM descriptors.
  // most commands consist of a "chain" (a linked list) of a couple of
  // these descriptors.
  संरचना virtq_desc *desc;

  // a ring in which the driver writes descriptor numbers
  // that the driver would like the device to process.  it only
  // includes the head descriptor of each chain. the ring has
  // NUM elements.
  संरचना virtq_avail *avail;

  // a ring in which the device writes descriptor numbers that
  // the device has finished processing (just the head of each chain).
  // there are NUM used ring entries.
  संरचना virtq_प्रयुक्तम् *प्रयुक्तम्_2;

  // our own book-keeping.
  वर्ण मुक्त[अङ्कः_2];  // is a descriptor free?
  uint16 प्रयुक्तम्_idx; // we've looked this far in used[2..NUM].

  // track info about in-flight operations,
  // for use when completion interrupt arrives.
  // indexed by first descriptor index of chain.
  संरचना {
    संरचना buf *b;
    वर्ण स्थितिः;
  } info[अङ्कः_2];

  // disk command headers.
  // one-for-one with descriptors, for convenience.
  संरचना virtio_blk_req ops[अङ्कः_2];

  संरचना spinlock vdisk_तालकम्;

} disk;

शून्य
virtio_disk_init(शून्य)
{
  uint32 स्थितिः = 0;

  initlock(&disk.vdisk_तालकम्, "virtio_disk");

  यदि (*R(VIRTIO_MMIO_MAGIC_मानम्) != 0x74726976 ||
      *R(VIRTIO_MMIO_संस्करणम्) != 2 || *R(VIRTIO_MMIO_DEVICE_ID) != 2 ||
      *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551) {
    panic("could not find virtio disk");
  }

  // reset device
  *R(VIRTIO_MMIO_स्थितिः) = स्थितिः;

  // set ACKNOWLEDGE status bit
  स्थितिः |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
  *R(VIRTIO_MMIO_स्थितिः) = स्थितिः;

  // set DRIVER status bit
  स्थितिः |= VIRTIO_CONFIG_S_DRIVER;
  *R(VIRTIO_MMIO_स्थितिः) = स्थितिः;

  // negotiate features
  uint64 features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
  features &= ~(1 << VIRTIO_BLK_F_RO);
  features &= ~(1 << VIRTIO_BLK_F_SCSI);
  features &= ~(1 << VIRTIO_BLK_F_FLUSH);
  features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
  features &= ~(1 << VIRTIO_BLK_F_MQ);
  features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
  features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
  features &= ~(1 << VIRTIO_RING_F_परोक्षम्_DESC);
  *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

  // tell device that feature negotiation is complete.
  स्थितिः |= VIRTIO_CONFIG_S_FEATURES_OK;
  *R(VIRTIO_MMIO_स्थितिः) = स्थितिः;

  // re-read status to ensure FEATURES_OK is set.
  स्थितिः = *R(VIRTIO_MMIO_स्थितिः);
  यदि (!(स्थितिः & VIRTIO_CONFIG_S_FEATURES_OK))
    panic("virtio disk FEATURES_OK unset");

  // initialize queue 0.
  *R(VIRTIO_MMIO_QUEUE_SEL) = 0;

  // ensure queue 0 is not in use.
  यदि (*R(VIRTIO_MMIO_QUEUE_READY))
    panic("virtio disk should not be ready");

  // check maximum queue size.
  uint32 max = *R(VIRTIO_MMIO_QUEUE_अङ्कः_MAX);
  यदि (max == 0)
    panic("virtio disk has no queue 0");
  यदि (max < अङ्कः_2)
    panic("virtio disk max queue too short");

  // allocate and zero queue memory.
  disk.desc = kalloc();
  disk.avail = kalloc();
  disk.प्रयुक्तम्_2 = kalloc();
  यदि (!disk.desc || !disk.avail || !disk.प्रयुक्तम्_2)
    panic("virtio disk kalloc");
  स्मृतिं_पूरय(disk.desc, 0, PGSIZE);
  स्मृतिं_पूरय(disk.avail, 0, PGSIZE);
  स्मृतिं_पूरय(disk.प्रयुक्तम्_2, 0, PGSIZE);

  // set queue size.
  *R(VIRTIO_MMIO_QUEUE_अङ्कः) = अङ्कः_2;

  // write physical addresses.
  *R(VIRTIO_MMIO_QUEUE_DESC_नीचः) = (uint64)disk.desc;
  *R(VIRTIO_MMIO_QUEUE_DESC_उच्चः) = (uint64)disk.desc >> 32;
  *R(VIRTIO_MMIO_DRIVER_DESC_नीचः) = (uint64)disk.avail;
  *R(VIRTIO_MMIO_DRIVER_DESC_उच्चः) = (uint64)disk.avail >> 32;
  *R(VIRTIO_MMIO_DEVICE_DESC_नीचः) = (uint64)disk.प्रयुक्तम्_2;
  *R(VIRTIO_MMIO_DEVICE_DESC_उच्चः) = (uint64)disk.प्रयुक्तम्_2 >> 32;

  // queue is ready.
  *R(VIRTIO_MMIO_QUEUE_READY) = 0x1;

  // all NUM descriptors start out unused.
  प्रति (पूर्णाङ्क i = 0; i < अङ्कः_2; i++)
    disk.मुक्त[i] = 1;

  // tell device we're completely ready.
  स्थितिः |= VIRTIO_CONFIG_S_DRIVER_OK;
  *R(VIRTIO_MMIO_स्थितिः) = स्थितिः;

  // plic.c and trap.c arrange for interrupts from VIRTIO0_IRQ.
}

// find a free descriptor, mark it non-free, return its index.
स्थिर पूर्णाङ्क
alloc_desc()
{
  प्रति (पूर्णाङ्क i = 0; i < अङ्कः_2; i++) {
    यदि (disk.मुक्त[i]) {
      disk.मुक्त[i] = 0;
      प्रत्यागम i;
    }
  }
  प्रत्यागम -1;
}

// mark a descriptor as free.
स्थिर शून्य
free_desc(पूर्णाङ्क i)
{
  यदि (i >= अङ्कः_2)
    panic("free_desc 1");
  यदि (disk.मुक्त[i])
    panic("free_desc 2");
  disk.desc[i].addr = 0;
  disk.desc[i].len = 0;
  disk.desc[i].flags = 0;
  disk.desc[i].अग्र = 0;
  disk.मुक्त[i] = 1;
  wakeup(&disk.मुक्त[0]);
}

// free a chain of descriptors.
स्थिर शून्य
free_chain(पूर्णाङ्क i)
{
  यावत् (1) {
    पूर्णाङ्क flag = disk.desc[i].flags;
    पूर्णाङ्क nxt = disk.desc[i].अग्र;
    free_desc(i);
    यदि (flag & VRING_DESC_F_अग्र)
      i = nxt;
    अन्यथा
      भङ्ग;
  }
}

// allocate three descriptors (they need not be contiguous).
// disk transfers always use three descriptors.
स्थिर पूर्णाङ्क
alloc3_desc(पूर्णाङ्क *idx)
{
  प्रति (पूर्णाङ्क i = 0; i < 3; i++) {
    idx[i] = alloc_desc();
    यदि (idx[i] < 0) {
      प्रति (पूर्णाङ्क j = 0; j < i; j++)
        free_desc(idx[j]);
      प्रत्यागम -1;
    }
  }
  प्रत्यागम 0;
}

शून्य
virtio_disk_rw(संरचना buf *b, पूर्णाङ्क लिख)
{
  uint64 sector = b->blockno * (BSIZE / 512);

  acquire(&disk.vdisk_तालकम्);

  // the spec's Section 5.2 says that legacy block operations use
  // three descriptors: one for type/reserved/sector, one for the
  // data, one for a 1-byte status result.

  // allocate the three descriptors.
  पूर्णाङ्क idx[3];
  यावत् (1) {
    यदि (alloc3_desc(idx) == 0) {
      भङ्ग;
    }
    sleep_prepare(&disk.मुक्त[0]);
    release(&disk.vdisk_तालकम्);
    sleep();
    acquire(&disk.vdisk_तालकम्);
  }

  // format the three descriptors.
  // qemu's virtio-blk.c reads them.

  संरचना virtio_blk_req *buf0 = &disk.ops[idx[0]];

  यदि (लिख)
    buf0->type = VIRTIO_BLK_T_बहिर्गमनम्; // write the disk
  अन्यथा
    buf0->type = VIRTIO_BLK_T_अन्तर्; // read the disk
  buf0->reserved = 0;
  buf0->sector = sector;

  disk.desc[idx[0]].addr = (uint64)buf0;
  disk.desc[idx[0]].len = परिमाण(संरचना virtio_blk_req);
  disk.desc[idx[0]].flags = VRING_DESC_F_अग्र;
  disk.desc[idx[0]].अग्र = idx[1];

  disk.desc[idx[1]].addr = (uint64)b->दत्त;
  disk.desc[idx[1]].len = BSIZE;
  यदि (लिख)
    disk.desc[idx[1]].flags = 0; // device reads b->data
  अन्यथा
    disk.desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
  disk.desc[idx[1]].flags |= VRING_DESC_F_अग्र;
  disk.desc[idx[1]].अग्र = idx[2];

  disk.info[idx[0]].स्थितिः = 0xff; // device writes 0 on success
  disk.desc[idx[2]].addr = (uint64)&disk.info[idx[0]].स्थितिः;
  disk.desc[idx[2]].len = 1;
  disk.desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
  disk.desc[idx[2]].अग्र = 0;

  // record struct buf for virtio_disk_intr().
  b->disk = 1;
  disk.info[idx[0]].b = b;

  // tell the device the first index in our chain of descriptors.
  disk.avail->ring[disk.avail->idx % अङ्कः_2] = idx[0];

  io_fence();

  // tell the device another avail ring entry is available.
  disk.avail->idx += 1; // not % NUM ...

  io_fence();

  *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0; // value is queue number

  // Wait for virtio_disk_intr() to say request has finished.
  यावत् (b->disk == 1) {
    sleep_prepare(b);
    release(&disk.vdisk_तालकम्);
    sleep();
    acquire(&disk.vdisk_तालकम्);
  }

  disk.info[idx[0]].b = 0;
  free_chain(idx[0]);

  release(&disk.vdisk_तालकम्);
}

शून्य
virtio_disk_intr()
{
  acquire(&disk.vdisk_तालकम्);

  // the device won't raise another interrupt until we tell it
  // we've seen this interrupt, which the following line does.
  // this may race with the device writing new entries to
  // the "used" ring, in which case we may process the new
  // completion entries in this interrupt, and have nothing to do
  // in the next interrupt, which is harmless.
  *R(VIRTIO_MMIO_व्यवधानम्_ACK) = *R(VIRTIO_MMIO_व्यवधानम्_स्थितिः) & 0x3;

  io_fence();

  // the device increments disk.used->idx when it
  // adds an entry to the used ring.

  यावत् (disk.प्रयुक्तम्_idx != disk.प्रयुक्तम्_2->idx) {
    io_fence();
    पूर्णाङ्क id = disk.प्रयुक्तम्_2->ring[disk.प्रयुक्तम्_idx % अङ्कः_2].id;

    यदि (disk.info[id].स्थितिः != 0)
      panic("virtio_disk_intr status");

    संरचना buf *b = disk.info[id].b;
    b->disk = 0; // disk is done with buf
    wakeup(b);

    disk.प्रयुक्तम्_idx += 1;
  }

  release(&disk.vdisk_तालकम्);
}
