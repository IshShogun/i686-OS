# Learning points

While interesting to build a 32 bit bare bones operating system with a tty driver, grub which utilising the multiboot2 spec. This codebase has a few issues.

## Codebase Issues

### Issue 1: highly outdated

This is the main reason i will not longer be working on this. 

The osdev wiki tutorial was meant to use VGA, multiboot1, and 32bit OS as an example OS. I adapted it to use multiboot2, framebuffer in 32 bit protected mode. However, this is highly outdated.

Firstly, 32bit OS's only has 4gb of addressable virtual memory. Meaning i'll have remap the memory to a DMA zone (0-8mb), lowmem (user space 8mb -> 3GB - 128mb + 8mb), highmem (kernel space 3gb -> 4gb). Modern hardware has far more than 4GB, meaning its deprecated.
This solution isn't used anymore as a limited address space isn't a problem anymore. Instead modern kernels use hhdm. The higher half of the address space is a direct map to physical memory (phys + offset = virt), the kernel uses this address space
pages can be reused with the bottom half of the address space. 

While remapping is still done in x86_64, this specific lowmem/highmem solution is definitely legacy and i would rather implement a grub/mb2/hhdm remap in 64bit if i come to it.

For now as i've played with multiboot2 and studied MentOS i want to play with paging, context switching and interrupts now. So i am moving to a x86_64 limine kernel now. 

### Issue 2: Dumb code, dumb structure

Instead of building a tty driver and simply abstracting arch specific logic that gets realised during linking, the tutorial I followed (bare bones & meaty skeleton osdev wiki) decides to make it arch specific? 

Also why does every function have its own file? The build toolchain is friggin unnessarily confusing. whole this is just a mess. lol/

Also whats the point of a libk? just write a libc, we have to have a bunch a __is_libk__ ifdefs, just weird. i don't like it. we only implement like 0.01% of libc anyway, so just link it? lol. 


# Conclusion

Adapting osdev wiki barebones to multiboot2 and framebuffer tty in a 32bit OS was interesting. But now i have to do the remap with lowmem and highmem, which interests me less at the moment than paging, scheduling and interrupts. So i'm moving to limine x86_64.
