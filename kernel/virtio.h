//
// virtio device definitions.
// for both the mmio interface, and virtio descriptors.
// only tested with qemu.
//
// the virtio spec:
// https://docs.oasis-open.org/virtio/virtio/v1.1/virtio-v1.1.pdf
//

// virtio mmio control registers, mapped starting at 0x10001000.
// from qemu virtio_mmio.h
// clang-format off
#परिभाषय VIRTIO_MMIO_MAGIC_मानम्		0x000 // 0x74726976
#परिभाषय VIRTIO_MMIO_संस्करणम्		0x004 // version; should be 2
#परिभाषय VIRTIO_MMIO_DEVICE_ID		0x008 // device type; 1 is net, 2 is disk
#परिभाषय VIRTIO_MMIO_VENDOR_ID		0x00c // 0x554d4551
#परिभाषय VIRTIO_MMIO_DEVICE_FEATURES	0x010
#परिभाषय VIRTIO_MMIO_DRIVER_FEATURES	0x020
#परिभाषय VIRTIO_MMIO_QUEUE_SEL		0x030 // select queue, write-only
#परिभाषय VIRTIO_MMIO_QUEUE_अङ्कः_MAX	0x034 // max size of current queue, read-only
#परिभाषय VIRTIO_MMIO_QUEUE_अङ्कः		0x038 // size of current queue, write-only
#परिभाषय VIRTIO_MMIO_QUEUE_READY		0x044 // ready bit
#परिभाषय VIRTIO_MMIO_QUEUE_NOTIFY	0x050 // write-only
#परिभाषय VIRTIO_MMIO_व्यवधानम्_स्थितिः	0x060 // read-only
#परिभाषय VIRTIO_MMIO_व्यवधानम्_ACK	0x064 // write-only
#परिभाषय VIRTIO_MMIO_स्थितिः		0x070 // read/write
#परिभाषय VIRTIO_MMIO_QUEUE_DESC_नीचः	0x080 // physical address for descriptor table, write-only
#परिभाषय VIRTIO_MMIO_QUEUE_DESC_उच्चः	0x084
#परिभाषय VIRTIO_MMIO_DRIVER_DESC_नीचः	0x090 // physical address for available ring, write-only
#परिभाषय VIRTIO_MMIO_DRIVER_DESC_उच्चः	0x094
#परिभाषय VIRTIO_MMIO_DEVICE_DESC_नीचः	0x0a0 // physical address for used ring, write-only
#परिभाषय VIRTIO_MMIO_DEVICE_DESC_उच्चः	0x0a4
// clang-format on

// status register bits, from qemu virtio_config.h
#परिभाषय VIRTIO_CONFIG_S_ACKNOWLEDGE 1
#परिभाषय VIRTIO_CONFIG_S_DRIVER      2
#परिभाषय VIRTIO_CONFIG_S_DRIVER_OK   4
#परिभाषय VIRTIO_CONFIG_S_FEATURES_OK 8

// device feature bits
#परिभाषय VIRTIO_BLK_F_RO             5  /* Disk is read-only */
#परिभाषय VIRTIO_BLK_F_SCSI           7  /* Supports scsi command passthru */
#परिभाषय VIRTIO_BLK_F_FLUSH          9  /* Cache flush command supported */
#परिभाषय VIRTIO_BLK_F_CONFIG_WCE     11 /* Writeback mode available in config */
#परिभाषय VIRTIO_BLK_F_MQ             12 /* support more than one vq */
#परिभाषय VIRTIO_F_ANY_LAYOUT         27
#परिभाषय VIRTIO_RING_F_परोक्षम्_DESC 28
#परिभाषय VIRTIO_RING_F_EVENT_IDX     29

// this many virtio descriptors.
// must be a power of two.
#परिभाषय अङ्कः_2 8

// a single descriptor, from the spec.
संरचना virtq_desc {
  uint64 addr;
  uint32 len;
  uint16 flags;
  uint16 अग्र;
};
#परिभाषय VRING_DESC_F_अग्र  1 // chained with another descriptor
#परिभाषय VRING_DESC_F_WRITE 2 // device writes (vs read)

// the (entire) avail ring, from the spec.
संरचना virtq_avail {
  uint16 flags;     // always zero
  uint16 idx;       // driver will write ring[idx] next
  uint16 ring[अङ्कः_2]; // descriptor numbers of chain heads
  uint16 अप्रयुक्तम्_2;
};

// one entry in the "used" ring, with which the
// device tells the driver about completed requests.
संरचना virtq_प्रयुक्तम्_elem {
  uint32 id; // index of start of completed descriptor chain
  uint32 len;
};

संरचना virtq_प्रयुक्तम् {
  uint16 flags; // always zero
  uint16 idx;   // device increments when it adds a ring[] entry
  संरचना virtq_प्रयुक्तम्_elem ring[अङ्कः_2];
};

// these are specific to virtio block devices, e.g. disks,
// described in Section 5.2 of the spec.

#परिभाषय VIRTIO_BLK_T_अन्तर्  0 // read the disk
#परिभाषय VIRTIO_BLK_T_बहिर्गमनम् 1 // write the disk

// the format of the first descriptor in a disk request.
// to be followed by two more descriptors containing
// the block, and a one-byte status.
संरचना virtio_blk_req {
  uint32 type; // VIRTIO_BLK_T_IN or ..._OUT
  uint32 reserved;
  uint64 sector;
};
