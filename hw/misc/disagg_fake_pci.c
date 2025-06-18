// Sources for some code: 
// https://www.qemu.org/docs/master/devel/qom.html
// https://blog.davidv.dev/posts/learning-pcie/
// https://github.com/airbus-seclab/qemu_blog/blob/main/pci_slave.md
// Harsha's shmem api in the libvfio-user subproject tran_sock.c file

#include "qemu/osdep.h"

#include "hw/hw.h"
#include "hw/pci/pci_bridge.h"
#include "hw/qdev-properties.h"

/*
 * All those values are set iniside the EDU init class function
 */
#define DISAGG_VENDOR_ID PCI_VENDOR_ID_QEMU
#define DISAGG_DEVICE_ID (0x11e8)
#define DISAGG_REVISION (0x10)
#define DISAGG_CLASS_ID (PCI_CLASS_OTHERS)

#define TYPE_DISAGG_FAKE_PCI "disagg-fake-pci"
OBJECT_DECLARE_SIMPLE_TYPE(DisaggFakePCIDevice, DISAGG_FAKE_PCI)

struct DisaggFakePCIDevice {
    PCIDevice pdev;
    uint64_t bar_size;
    MemoryRegion bar0;
};

// No additions to this class, so reuse parent class
typedef PCIDeviceClass DisaggFakePCIDeviceClass;

static void disagg_fake_pci_device_realize(PCIDevice *pdev, Error **errp)
{
    printf("Disagg_fake_pci_device_realize\n");

    DisaggFakePCIDevice *dev = DISAGG_FAKE_PCI(pdev);
    //PCIDeviceClass* pclass = PCI_DEVICE_GET_CLASS(pdev);

    memory_region_init_io(&dev->bar0, OBJECT(dev), NULL, NULL, "disagg-fake-pci-mmio", dev->bar_size);
    pci_register_bar(pdev, /* BAR nr. */ 0, PCI_BASE_ADDRESS_SPACE_MEMORY, &dev->bar0);
}

static Property disagg_fake_device_properties[] = {
    DEFINE_PROP_UINT64("bar-size", DisaggFakePCIDevice, bar_size, 0),
};

static void disagg_fake_device_instance_init(Object *obj)
{
    printf("disagg_fake_device_instance_init\n");
}

static void disagg_fake_device_class_init(ObjectClass *klass, void *data)
{
    printf("disagg_fake_device_class_init\n");

    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *pdc = PCI_DEVICE_CLASS(klass);

    device_class_set_props(dc, disagg_fake_device_properties);
    dc->desc = "Disagg fake pci device for remote EDU";
    pdc->realize = disagg_fake_pci_device_realize;
    pdc->vendor_id = DISAGG_VENDOR_ID;
    pdc->device_id = DISAGG_DEVICE_ID;
    pdc->revision = DISAGG_REVISION;
    pdc->class_id = DISAGG_CLASS_ID;
}

static const TypeInfo disagg_fake_device_info = {
    .name = TYPE_DISAGG_FAKE_PCI,
    .parent = TYPE_PCI_DEVICE,
    .instance_size = sizeof(DisaggFakePCIDevice),
    .instance_init = disagg_fake_device_instance_init,
    .class_init = disagg_fake_device_class_init,
    .interfaces = (InterfaceInfo[]) {
	{ INTERFACE_CONVENTIONAL_PCI_DEVICE },
	{ },
    },
};

static void disagg_fake_device_register_type(void)
{
    type_register_static(&disagg_fake_device_info);
}

type_init(disagg_fake_device_register_type);
