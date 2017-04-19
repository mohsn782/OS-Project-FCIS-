#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)

//=================================================================================//
//============================ REQUIRED FUNCTION ==================================//
//=================================================================================//

struct Address_Info
{
	uint32 size;
	uint32 VAddess;
};

struct Address_Info AddInfo[100];
int ctr=-1;
uint32 *firstFreeVAInKHeap  = (uint32*)KERNEL_HEAP_START;

void* kmalloc(unsigned int size)
{
	size = ROUNDUP(size, PAGE_SIZE);
	int i,pages=size/PAGE_SIZE;
	if(size <= KERNEL_HEAP_MAX - (uint32)firstFreeVAInKHeap)
	{
		uint32 *va = firstFreeVAInKHeap;
		ctr++;
		AddInfo[ctr].size = pages;
	    AddInfo[ctr].VAddess = (uint32)va;
		for(i=0;i<pages;i++)
		{
			struct Frame_Info * ptr_frame_info;
			uint32 *ptr = (uint32 *)(firstFreeVAInKHeap);
			allocate_frame(&ptr_frame_info);
			map_frame(ptr_page_directory, ptr_frame_info, (void*)ptr, PERM_WRITEABLE);
			firstFreeVAInKHeap = (uint32*) ((int)firstFreeVAInKHeap  + PAGE_SIZE);
		}
		return (void*)va;
	}
	else
		return NULL;
}

void kfree(void* virtual_address)
{
	int i,sz;
	for(i=0;i<=ctr;i++)
	{
		if(AddInfo[i].VAddess==(uint32)virtual_address)
		{
			sz = AddInfo[i].size;
			break;
		}
	}
	for (i=0;i<sz;i++)
	{
		unmap_frame(ptr_page_directory, (void*)virtual_address);
		virtual_address += PAGE_SIZE;
	}
	tlbflush();
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	int va = KERNEL_HEAP_START;
	uint32 *ptr_page_table;
	for (;va< KERNEL_HEAP_MAX; va += PAGE_SIZE)
	{
		if (get_frame_info(ptr_page_directory, (void *)va, &ptr_page_table) ==
				to_frame_info(physical_address))
			return va;
	}
	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	uint32 ppt,va = virtual_address;
	uint32 *ptr_page_table = NULL;
	get_page_table(ptr_page_directory, (void*)virtual_address, &ptr_page_table);
	if(ptr_page_table!=NULL)
	{
		uint32 used = ptr_page_table[PTX(virtual_address)] & PERM_PRESENT;
		if(used==1)
		{
			ppt=((ptr_page_table[PTX(virtual_address)])>>12)*4096;
			va = va & (uint32)0x00000FFF;
			return ppt+va;
		}
		return 0;
	}
	return 0;
}

//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2017 - BONUS2] Kernel Heap Realloc
	// Write your code here, remove the panic and write your code
	return NULL;
	panic("krealloc() is not implemented yet...!!");

}
