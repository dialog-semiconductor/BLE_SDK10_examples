







GROUP(libgcc.a libc.a libm.a libnosys.a)
ENTRY(Reset_Handler)
SECTIONS
{
        .init_text :
        {
                KEEP(*(.isr_vector))
                . = 0x200;
                __Vectors_End = .;
                __Vectors_Size = __Vectors_End - __isr_vector;
                *(text_reset*)
        } > RAM
        .text :
        {
                *(EXCLUDE_FILE(*libnosys.a:sbrk.o
                               *libgcc.a:_aeabi_uldivmod.o
                               *libgcc.a:_muldi3.o
                               *libgcc.a:_dvmd_tls.o
                               *libgcc.a:bpabi.o
                               *libgcc.a:_udivdi3.o
                               *libgcc.a:_clzdi2.o
                               *libgcc.a:_clzsi2.o) .text*)
                . = ALIGN(4);
                __start_adapter_init_section = .;
                KEEP(*(adapter_init_section))
                __stop_adapter_init_section = .;
                __start_bus_init_section = .;
                KEEP(*(bus_init_section))
                __stop_bus_init_section = .;
                __start_device_init_section = .;
                KEEP(*(device_init_section))
                __stop_device_init_section = .;
                KEEP(*(.init))
                KEEP(*(.fini))
                *crtbegin.o(.ctors)
                *crtbegin?.o(.ctors)
                *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
                *(SORT(.ctors.*))
                *(.ctors)
                *crtbegin.o(.dtors)
                *crtbegin?.o(.dtors)
                *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
                *(SORT(.dtors.*))
                *(.dtors)
                . = ALIGN(4);
                PROVIDE_HIDDEN (__preinit_array_start = .);
                KEEP(*(.preinit_array))
                PROVIDE_HIDDEN (__preinit_array_end = .);
                . = ALIGN(4);
                PROVIDE_HIDDEN (__init_array_start = .);
                KEEP(*(SORT(.init_array.*)))
                KEEP(*(.init_array))
                PROVIDE_HIDDEN (__init_array_end = .);
                . = ALIGN(4);
                PROVIDE_HIDDEN (__fini_array_start = .);
                KEEP(*(SORT(.fini_array.*)))
                KEEP(*(.fini_array))
                PROVIDE_HIDDEN (__fini_array_end = .);
                *(.rodata*)
                KEEP(*(.eh_frame*))
        } > RAM
        .ARM.extab :
        {
                *(.ARM.extab* .gnu.linkonce.armextab.*)
        } > RAM
        __exidx_start = .;
        .ARM.exidx :
        {
                *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        } > RAM
        __exidx_end = .;
        .copy.table :
        {
                . = ALIGN(4);
                __copy_table_start__ = .;
                __copy_table_end__ = .;
        } > RAM
        .zero.table :
        {
                . = ALIGN(4);
                __zero_table_start__ = .;
                LONG (__bss_start__)
                LONG (__bss_end__ - __bss_start__)
                LONG (__retention_ram_zi_start__)
                LONG (__retention_ram_zi_end__ - __retention_ram_zi_start__)
                __zero_table_end__ = .;
        } > RAM
        __etext = .;
        .retention_ram_uninit (NOLOAD) :
        {
                __retention_ram_uninit_start__ = .;
                KEEP(*(nmi_info))
                KEEP(*(hard_fault_info))
                KEEP(*(retention_mem_uninit))
                ASSERT( . <= __retention_ram_uninit_start__ + (128),
                        "retention_ram_uninit section overflowed! Increase dg_configRETAINED_UNINIT_SECTION_SIZE.");
                . = __retention_ram_uninit_start__ + (128);
                __retention_ram_uninit_end__ = .;
        } > RAM
        .retention_ram_init :
        {
                __retention_ram_init_start__ = .;
                . = ALIGN(4);
                *libnosys.a:sbrk.o (.text*)
                *libgcc.a:_aeabi_uldivmod.o (.text*)
                *libgcc.a:_muldi3.o (.text*)
                *libgcc.a:_dvmd_tls.o (.text*)
                *libgcc.a:bpabi.o (.text*)
                *libgcc.a:_udivdi3.o (.text*)
                *libgcc.a:_clzdi2.o (.text*)
                *libgcc.a:_clzsi2.o (.text*)
                *(privileged_data_init)
                *(.retention)
                *(vtable)
                *(retention_mem_init)
                *(retention_mem_const)
                *libg_nano.a:* (.data*)
                *libnosys.a:* (.data*)
                *libgcc.a:* (.data*)
                *libble_stack_da1469x.a:* (.data*)
                *crtbegin.o (.data*)
                KEEP(*(.jcr*))
                . = ALIGN(4);
                __retention_ram_init_end__ = .;
        } > RAM
        .retention_ram_zi (NOLOAD) :
        {
                __retention_ram_zi_start__ = .;
                *(privileged_data_zi)
                *(retention_mem_zi)
                *libg_nano.a:* (.bss*)
                *libnosys.a:* (.bss*)
                *libgcc.a:* (.bss*)
                *libble_stack_da1469x.a:* (.bss*)
                *crtbegin.o (.bss*)
                *(os_heap)
                __HeapBase = .;
                __end__ = .;
                end = __end__;
                KEEP(*(.heap*))
                __HeapLimit = .;
                __retention_ram_zi_end__ = .;
        } > RAM
        .stack_section (NOLOAD) :
        {
                . = . + 1;
                . = ALIGN(8);
                __StackLimit = .;
                KEEP(*(.stack*))
                . = ALIGN(8);
                __StackTop = .;
                PROVIDE(__stack = __StackTop);
        } > RAM
        .snc_section :
        {
                __snc_section_start__ = .;
                . = ALIGN(4);
                KEEP(*(.snc_region*))
                . = ALIGN(4);
                __snc_section_end__ = .;
        } > RAM
        __non_retention_ram_start__ = .;
        .non_retention_ram_init :
        {
                __non_retention_ram_init_start__ = .;
                . = ALIGN(4);
                *(EXCLUDE_FILE(*libg_nano.a:* *libnosys.a:* *libgcc.a:* *libble_stack_da1469x.a:* *crtbegin.o) .data*)
                . = ALIGN(4);
                __non_retention_ram_init_end__ = .;
        } > RAM
        .bss :
        {
                . = ALIGN(4);
                __bss_start__ = .;
                *(EXCLUDE_FILE(*libg_nano.a:* *libnosys.a:* *libgcc.a:* *libble_stack_da1469x.a:* *crtbegin.o) .bss*)
                *(COMMON)
                . = ALIGN(4);
                __bss_end__ = .;
        } > RAM
        __non_retention_ram_end__ = .;
        __unused_ram_start__ = . + 1;
}
