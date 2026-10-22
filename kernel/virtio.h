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
#تعریف_کن VIRTIO_MMIO_MAGIC_مقدار		0x000 // 0x74726976
#تعریف_کن VIRTIO_MMIO_نسخه		0x004 // version; should be 2
#تعریف_کن VIRTIO_MMIO_DEVICE_ID		0x008 // device type; 1 is net, 2 is disk
#تعریف_کن VIRTIO_MMIO_VENDOR_ID		0x00c // 0x554d4551
#تعریف_کن VIRTIO_MMIO_DEVICE_FEATURES	0x010
#تعریف_کن VIRTIO_MMIO_DRIVER_FEATURES	0x020
#تعریف_کن VIRTIO_MMIO_صف_SEL		0x030 // select queue, write-only
#تعریف_کن VIRTIO_MMIO_صف_رقم_MAX	0x034 // max size of current queue, read-only
#تعریف_کن VIRTIO_MMIO_صف_رقم		0x038 // size of current queue, write-only
#تعریف_کن VIRTIO_MMIO_صف_READY		0x044 // ready bit
#تعریف_کن VIRTIO_MMIO_صف_NOTIFY	0x050 // write-only
#تعریف_کن VIRTIO_MMIO_وقفه_وضعیت	0x060 // read-only
#تعریف_کن VIRTIO_MMIO_وقفه_ACK	0x064 // write-only
#تعریف_کن VIRTIO_MMIO_وضعیت		0x070 // read/write
#تعریف_کن VIRTIO_MMIO_صف_DESC_کم	0x080 // physical address for descriptor table, write-only
#تعریف_کن VIRTIO_MMIO_صف_DESC_بالا	0x084
#تعریف_کن VIRTIO_MMIO_DRIVER_DESC_کم	0x090 // physical address for available ring, write-only
#تعریف_کن VIRTIO_MMIO_DRIVER_DESC_بالا	0x094
#تعریف_کن VIRTIO_MMIO_DEVICE_DESC_کم	0x0a0 // physical address for used ring, write-only
#تعریف_کن VIRTIO_MMIO_DEVICE_DESC_بالا	0x0a4
// clang-format on

// status register bits, from qemu virtio_config.h
#تعریف_کن VIRTIO_CONFIG_S_ACKNOWLEDGE 1
#تعریف_کن VIRTIO_CONFIG_S_DRIVER      2
#تعریف_کن VIRTIO_CONFIG_S_DRIVER_OK   4
#تعریف_کن VIRTIO_CONFIG_S_FEATURES_OK 8

// device feature bits
#تعریف_کن VIRTIO_BLK_F_RO             5  /* Disk is read-only */
#تعریف_کن VIRTIO_BLK_F_SCSI           7  /* Supports scsi command passthru */
#تعریف_کن VIRTIO_BLK_F_FLUSH          9  /* Cache flush command supported */
#تعریف_کن VIRTIO_BLK_F_CONFIG_WCE     11 /* Writeback mode available in config */
#تعریف_کن VIRTIO_BLK_F_MQ             12 /* support more than one vq */
#تعریف_کن VIRTIO_F_ANY_LAYOUT         27
#تعریف_کن VIRTIO_RING_F_غیرمستقیم_DESC 28
#تعریف_کن VIRTIO_RING_F_EVENT_IDX     29

// this many virtio descriptors.
// must be a power of two.
#تعریف_کن رقم_2 8

// a single descriptor, from the spec.
بنية virtq_desc {
  uint64 addr;
  uint32 len;
  uint16 flags;
  uint16 بعدی;
};
#تعریف_کن VRING_DESC_F_بعدی  1 // chained with another descriptor
#تعریف_کن VRING_DESC_F_WRITE 2 // device writes (vs read)

// the (entire) avail ring, from the spec.
بنية virtq_avail {
  uint16 flags;     // always zero
  uint16 idx;       // driver will write ring[idx] next
  uint16 ring[رقم_2]; // descriptor numbers of chain heads
  uint16 استفادهنشده_2;
};

// one entry in the "used" ring, with which the
// device tells the driver about completed requests.
بنية virtq_استفاده_شده_elem {
  uint32 id; // index of start of completed descriptor chain
  uint32 len;
};

بنية virtq_استفاده_شده {
  uint16 flags; // always zero
  uint16 idx;   // device increments when it adds a ring[] entry
  بنية virtq_استفاده_شده_elem ring[رقم_2];
};

// these are specific to virtio block devices, e.g. disks,
// described in Section 5.2 of the spec.

#تعریف_کن VIRTIO_BLK_T_درون  0 // read the disk
#تعریف_کن VIRTIO_BLK_T_بیرون 1 // write the disk

// the format of the first descriptor in a disk request.
// to be followed by two more descriptors containing
// the block, and a one-byte status.
بنية virtio_blk_req {
  uint32 type; // VIRTIO_BLK_T_IN or ..._OUT
  uint32 reserved;
  uint64 sector;
};
