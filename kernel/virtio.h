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
#నిర్వచించు VIRTIO_MMIO_MAGIC_విలువ		0x000 // 0x74726976
#నిర్వచించు VIRTIO_MMIO_సంస్కరణ		0x004 // version; should be 2
#నిర్వచించు VIRTIO_MMIO_DEVICE_ID		0x008 // device type; 1 is net, 2 is disk
#నిర్వచించు VIRTIO_MMIO_VENDOR_ID		0x00c // 0x554d4551
#నిర్వచించు VIRTIO_MMIO_DEVICE_FEATURES	0x010
#నిర్వచించు VIRTIO_MMIO_DRIVER_FEATURES	0x020
#నిర్వచించు VIRTIO_MMIO_వరుస_SEL		0x030 // select queue, write-only
#నిర్వచించు VIRTIO_MMIO_వరుస_అంకె_MAX	0x034 // max size of current queue, read-only
#నిర్వచించు VIRTIO_MMIO_వరుస_అంకె		0x038 // size of current queue, write-only
#నిర్వచించు VIRTIO_MMIO_వరుస_READY		0x044 // ready bit
#నిర్వచించు VIRTIO_MMIO_వరుస_NOTIFY	0x050 // write-only
#నిర్వచించు VIRTIO_MMIO_అంతరాయం_స్థితి	0x060 // read-only
#నిర్వచించు VIRTIO_MMIO_అంతరాయం_ACK	0x064 // write-only
#నిర్వచించు VIRTIO_MMIO_స్థితి		0x070 // read/write
#నిర్వచించు VIRTIO_MMIO_వరుస_DESC_దిగువ	0x080 // physical address for descriptor table, write-only
#నిర్వచించు VIRTIO_MMIO_వరుస_DESC_ఎగువ	0x084
#నిర్వచించు VIRTIO_MMIO_DRIVER_DESC_దిగువ	0x090 // physical address for available ring, write-only
#నిర్వచించు VIRTIO_MMIO_DRIVER_DESC_ఎగువ	0x094
#నిర్వచించు VIRTIO_MMIO_DEVICE_DESC_దిగువ	0x0a0 // physical address for used ring, write-only
#నిర్వచించు VIRTIO_MMIO_DEVICE_DESC_ఎగువ	0x0a4
// clang-format on

// status register bits, from qemu virtio_config.h
#నిర్వచించు VIRTIO_CONFIG_S_ACKNOWLEDGE 1
#నిర్వచించు VIRTIO_CONFIG_S_DRIVER      2
#నిర్వచించు VIRTIO_CONFIG_S_DRIVER_OK   4
#నిర్వచించు VIRTIO_CONFIG_S_FEATURES_OK 8

// device feature bits
#నిర్వచించు VIRTIO_BLK_F_RO             5  /* Disk is read-only */
#నిర్వచించు VIRTIO_BLK_F_SCSI           7  /* Supports scsi command passthru */
#నిర్వచించు VIRTIO_BLK_F_FLUSH          9  /* Cache flush command supported */
#నిర్వచించు VIRTIO_BLK_F_CONFIG_WCE     11 /* Writeback mode available in config */
#నిర్వచించు VIRTIO_BLK_F_MQ             12 /* support more than one vq */
#నిర్వచించు VIRTIO_F_ANY_LAYOUT         27
#నిర్వచించు VIRTIO_RING_F_పరోక్ష_DESC 28
#నిర్వచించు VIRTIO_RING_F_EVENT_IDX     29

// this many virtio descriptors.
// must be a power of two.
#నిర్వచించు అంకె_2 8

// a single descriptor, from the spec.
నిర్మాణం virtq_desc {
  uint64 addr;
  uint32 len;
  uint16 flags;
  uint16 తదుపరి;
};
#నిర్వచించు VRING_DESC_F_తదుపరి  1 // chained with another descriptor
#నిర్వచించు VRING_DESC_F_WRITE 2 // device writes (vs read)

// the (entire) avail ring, from the spec.
నిర్మాణం virtq_avail {
  uint16 flags;     // always zero
  uint16 idx;       // driver will write ring[idx] next
  uint16 ring[అంకె_2]; // descriptor numbers of chain heads
  uint16 వాడని_2;
};

// one entry in the "used" ring, with which the
// device tells the driver about completed requests.
నిర్మాణం virtq_వాడిన_elem {
  uint32 id; // index of start of completed descriptor chain
  uint32 len;
};

నిర్మాణం virtq_వాడిన {
  uint16 flags; // always zero
  uint16 idx;   // device increments when it adds a ring[] entry
  నిర్మాణం virtq_వాడిన_elem ring[అంకె_2];
};

// these are specific to virtio block devices, e.g. disks,
// described in Section 5.2 of the spec.

#నిర్వచించు VIRTIO_BLK_T_లోపల  0 // read the disk
#నిర్వచించు VIRTIO_BLK_T_బహిర్గతం 1 // write the disk

// the format of the first descriptor in a disk request.
// to be followed by two more descriptors containing
// the block, and a one-byte status.
నిర్మాణం virtio_blk_req {
  uint32 type; // VIRTIO_BLK_T_IN or ..._OUT
  uint32 reserved;
  uint64 sector;
};
