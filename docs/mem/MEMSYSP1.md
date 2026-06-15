## The memory system for Trunk.

So since the start of the project, I have always been Interested In memory management.
I decided that Trunk's memory management system should be the part I focus on the most.
I want to make It as modern, feature-heavy, and secure as possible.
I've split It up Into 3 parts currently, which are temp and will be Improved on / changed.
I did this because I cannot write It all at once, some things need stuff My operating system does not have yet.
For example, there's no point In having 'oom' If we don't have user-space processes.

So the first part Is the core memory management system.
The second part Is added Improvements.
Third Is added Improvements as well.

# The 'hybrid'

Since I am basing Trunk off not only Linux, but also Windows, It's a hybrid.
I've implemented some Windows systems, some Linux systems.

But the memory management system Is a complete mix.

Both Windows and Linux handle memory very well.

But what about both of their systems mixed?

PFN from Windows, memblock from Linux?

Well, that's what Im doing.

I have carefully decided how to mix components and make the best possible system for Trunk!

# Part 1, the core

So for part 1, It will be a lot.
It will be the core memory system, memory won't work without It.
Part 2 and 3 are just Improvements.

Here are the chosen (current, more will be added) files for part 1:

```text
memblock
pfn
buddy
vmm
vma
guard
demand
lookaside
pool
dma
```

But what are these? What does each do?

# Explaining

So all files are concepts from Windows and Linux, merged together! Here Is each files respective job:

```text
memblock - Early boot stage memory allocation, discarded after boot stage.
pfn - Page frame number, maps virtual memory pages to their respective physical spot In RAM.
buddy - Advanced memory algorithm, used to allocate and deallocate physical page frames.
vmm - Virtual memory manager, abstraction over physical memory that lies to programs.
vma - Virtual memory area, tracks a processes memory mappings.
guard - Stack guard pages, protection layer that makes sure reserved memory areas are never touched.
demand - Inspects VMA layout, If verified, fetches a physical frame from buddy allocator.
lookaside - Fast static allocations, using pre-sized allocations that bypass the engine and allocate() directly.
pool - The official pool allocator, contains kmalloc(), and kfree().
dma - Direct memory access, Allows devices to transfer data directly to the system memory.
```

As you can see, each of these files Is either from Windows or Linux
It's a hybrid mix, with buddy system from linux, pool system from Windows, etc.

This stack provides fast, secure, robust, and complex memory management.
