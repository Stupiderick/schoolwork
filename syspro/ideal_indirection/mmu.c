/**
* Ideal Indirection Lab
* CS 241 - Fall 2018
*/

/**
 * partnered with Yuyun Li & Yufeng Luo
 */

#include "mmu.h"
#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

mmu *mmu_create() {
    mmu *my_mmu = calloc(1, sizeof(mmu));
    my_mmu->tlb = tlb_create();
    return my_mmu;
}

void mmu_read_from_virtual_address(mmu *this, addr32 virtual_address,
                                   size_t pid, void *buffer, size_t num_bytes) {
    assert(this);
    assert(pid < MAX_PROCESS_ID);
    assert(num_bytes + (virtual_address % PAGE_SIZE) <= PAGE_SIZE);
    // TODO implement me
    // buffer = malloc(num_bytes);

    vm_segmentations *mySeg = this->segmentations[pid];
    int goodSeg = address_in_segmentations(mySeg, virtual_address);

    // void *stuff = get_system_pointer_from_address(fullAddress);

    if (!goodSeg /*|| myPte->read_write == 0*/) {
        mmu_raise_segmentation_fault(this);
        return;
    } else {
        vm_segmentation *myOtherSeg = find_segment(this->segmentations[pid], virtual_address);
        if (!(myOtherSeg->permissions & READ)) {
            mmu_raise_segmentation_fault(this);
            return;
        }
    }

    if (this->curr_pid != 0) {
        if (this->curr_pid != pid) {
            tlb_flush(&this->tlb);
            // fprintf(stderr, "yess\n");
        }
    }
    // fprintf(stderr, "curr pid is: %zu\n", this->curr_pid);
    this->curr_pid = pid;

    addr32 base_vd = virtual_address >> 12;
    page_table_entry *myPte = tlb_get_pte(&this->tlb, base_vd);

    // when it is not in the tlb
    if (myPte == NULL) {
        mmu_tlb_miss(this);
        addr32 pd_idx = virtual_address >> 22;
        page_directory *myPd = this->page_directories[pid];
        page_directory_entry *myPde = &myPd->entries[pd_idx];
        if (!myPde->present) {
            mmu_raise_page_fault(this);
            addr32 myFrame = ask_kernel_for_frame(NULL);
            myPde->present = 1;
            myPde->read_write = 1;
            myPde->user_supervisor = 1;
            myPde->base_addr = myFrame >> 12;
        }

        addr32 pt_idx = virtual_address << 10 >> 22;
        page_table *myPt = get_system_pointer_from_pde(myPde);
        myPte = &myPt->entries[pt_idx];

        if (!myPte->present) {
            mmu_raise_page_fault(this);
            addr32 myPF = ask_kernel_for_frame(myPte);
            myPte->present = 1;
            myPte->read_write = 1;
            myPte->user_supervisor = 1;
            myPte->dirty = 0;
            myPte->base_addr = myPF >> 12;
            read_page_from_disk(myPte);
        }
        myPte->accessed = 1;
        tlb_add_pte(&this->tlb, base_vd, myPte);

    // when it is in the tlb...
    } else {
        if (!myPte->present) {
            mmu_raise_page_fault(this);
            addr32 myPF = ask_kernel_for_frame(myPte);
            myPte->present = 1;
            myPte->read_write = 1;
            myPte->user_supervisor = 1;
            myPte->dirty = 0;
            myPte->base_addr = myPF >> 12;
            read_page_from_disk(myPte);

        }
        myPte->accessed = 1;
    }

    addr32 offset = virtual_address << 20 >> 20;
    addr32 base = myPte->base_addr << 12;
    addr32 realAddress = base | offset;
    memcpy(buffer, get_system_pointer_from_address(realAddress), num_bytes);
}

void mmu_write_to_virtual_address(mmu *this, addr32 virtual_address, size_t pid,
                                  const void *buffer, size_t num_bytes) {
    assert(this);
    assert(pid < MAX_PROCESS_ID);
    assert(num_bytes + (virtual_address % PAGE_SIZE) <= PAGE_SIZE);
    // TODO implement me

    vm_segmentations *mySeg = this->segmentations[pid];
    int goodSeg = address_in_segmentations(mySeg, virtual_address);

    // void *stuff = get_system_pointer_from_address(fullAddress);


    if (!goodSeg /*|| myPte->read_write == 0*/) {
        mmu_raise_segmentation_fault(this);
        return;
    } else {
        vm_segmentation *myOtherSeg = find_segment(this->segmentations[pid], virtual_address);
        if (!(myOtherSeg->permissions & WRITE)) {
            mmu_raise_segmentation_fault(this);
            return;
        }
    }

    if (this->curr_pid != 0) {
        if (this->curr_pid != pid) {
            tlb_flush(&this->tlb);
            // fprintf(stderr, "yess\n");
        }
    }
    // fprintf(stderr, "curr pid is: %zu\n", this->curr_pid);
    this->curr_pid = pid;

    addr32 base_vd = virtual_address >> 12;
    page_table_entry *myPte = tlb_get_pte(&this->tlb, base_vd);

    // when it is not in the tlb
    if (myPte == NULL) {
        mmu_tlb_miss(this);
        addr32 pd_idx = virtual_address >> 22;
        page_directory *myPd = this->page_directories[pid];
        page_directory_entry *myPde = &myPd->entries[pd_idx];
        if (!myPde->present) {
            mmu_raise_page_fault(this);
            addr32 myFrame = ask_kernel_for_frame(NULL);
            myPde->present = 1;
            myPde->read_write = 1;
            myPde->user_supervisor = 1;
            myPde->base_addr = myFrame >> 12;
        }

        addr32 pt_idx = virtual_address << 10 >> 22;
        page_table *myPt = get_system_pointer_from_pde(myPde);
        myPte = &myPt->entries[pt_idx];

        if (!myPte->present) {
            mmu_raise_page_fault(this);
            addr32 myPF = ask_kernel_for_frame(myPte);
            myPte->present = 1;
            myPte->read_write = 1;
            myPte->user_supervisor = 1;
            myPte->dirty = 1;
            myPte->base_addr = myPF >> 12;
            read_page_from_disk(myPte);
        }
        myPte->accessed = 1;
        tlb_add_pte(&this->tlb, base_vd, myPte);

    // when it is in the tlb...
    } else {
        if (!myPte->present) {
            mmu_raise_page_fault(this);
            addr32 myPF = ask_kernel_for_frame(myPte);
            myPte->present = 1;
            myPte->read_write = 1;
            myPte->user_supervisor = 1;
            myPte->dirty = 1;
            myPte->base_addr = myPF >> 12;
            read_page_from_disk(myPte);
        }
        myPte->accessed = 1;
    }

    addr32 offset = virtual_address << 20 >> 20;
    addr32 base = myPte->base_addr << 12;
    addr32 realAddress = base | offset;
    memcpy(get_system_pointer_from_address(realAddress), buffer, num_bytes);
}

void mmu_tlb_miss(mmu *this) {
    this->num_tlb_misses++;
}

void mmu_raise_page_fault(mmu *this) {
    this->num_page_faults++;
}

void mmu_raise_segmentation_fault(mmu *this) {
    this->num_segmentation_faults++;
}

void mmu_add_process(mmu *this, size_t pid) {
    assert(pid < MAX_PROCESS_ID);
    addr32 page_directory_address = ask_kernel_for_frame(NULL);
    this->page_directories[pid] =
        (page_directory *)get_system_pointer_from_address(
            page_directory_address);
    page_directory *pd = this->page_directories[pid];
    this->segmentations[pid] = calloc(1, sizeof(vm_segmentations));
    vm_segmentations *segmentations = this->segmentations[pid];

    // Note you can see this information in a memory map by using
    // cat /proc/self/maps
    segmentations->segments[STACK] =
        (vm_segmentation){.start = 0xBFFFE000,
                          .end = 0xC07FE000, // 8mb stack
                          .permissions = READ | WRITE,
                          .grows_down = true};

    segmentations->segments[MMAP] =
        (vm_segmentation){.start = 0xC07FE000,
                          .end = 0xC07FE000,
                          // making this writeable to simplify the next lab.
                          // todo make this not writeable by default
                          .permissions = READ | EXEC | WRITE,
                          .grows_down = true};

    segmentations->segments[HEAP] =
        (vm_segmentation){.start = 0x08072000,
                          .end = 0x08072000,
                          .permissions = READ | WRITE,
                          .grows_down = false};

    segmentations->segments[BSS] =
        (vm_segmentation){.start = 0x0805A000,
                          .end = 0x08072000,
                          .permissions = READ | WRITE,
                          .grows_down = false};

    segmentations->segments[DATA] =
        (vm_segmentation){.start = 0x08052000,
                          .end = 0x0805A000,
                          .permissions = READ | WRITE,
                          .grows_down = false};

    segmentations->segments[TEXT] =
        (vm_segmentation){.start = 0x08048000,
                          .end = 0x08052000,
                          .permissions = READ | EXEC,
                          .grows_down = false};

    // creating a few mappings so we have something to play with (made up)
    // this segment is made up for testing purposes
    segmentations->segments[TESTING] =
        (vm_segmentation){.start = PAGE_SIZE,
                          .end = 3 * PAGE_SIZE,
                          .permissions = READ | WRITE,
                          .grows_down = false};
    // first 4 mb is bookkept by the first page directory entry
    page_directory_entry *pde = &(pd->entries[0]);
    // assigning it a page table and some basic permissions
    pde->base_addr = (ask_kernel_for_frame(NULL) >> NUM_OFFSET_BITS);
    pde->present = true;
    pde->read_write = true;
    pde->user_supervisor = true;

    // setting entries 1 and 2 (since each entry points to a 4kb page)
    // of the page table to point to our 8kb of testing memory defined earlier
    for (int i = 1; i < 3; i++) {
        page_table *pt = (page_table *)get_system_pointer_from_pde(pde);
        page_table_entry *pte = &(pt->entries[i]);
        pte->base_addr = (ask_kernel_for_frame(pte) >> NUM_OFFSET_BITS);
        pte->present = true;
        pte->read_write = true;
        pte->user_supervisor = true;
    }
}

void mmu_remove_process(mmu *this, size_t pid) {
    assert(pid < MAX_PROCESS_ID);
    // example of how to BFS through page table tree for those to read code.
    page_directory *pd = this->page_directories[pid];
    if (pd) {
        for (size_t vpn1 = 0; vpn1 < NUM_ENTRIES; vpn1++) {
            page_directory_entry *pde = &(pd->entries[vpn1]);
            if (pde->present) {
                page_table *pt = (page_table *)get_system_pointer_from_pde(pde);
                for (size_t vpn2 = 0; vpn2 < NUM_ENTRIES; vpn2++) {
                    page_table_entry *pte = &(pt->entries[vpn2]);
                    if (pte->present) {
                        void *frame = (void *)get_system_pointer_from_pte(pte);
                        return_frame_to_kernel(frame);
                    }
                    remove_swap_file(pte);
                }
                return_frame_to_kernel(pt);
            }
        }
        return_frame_to_kernel(pd);
    }

    this->page_directories[pid] = NULL;
    free(this->segmentations[pid]);
    this->segmentations[pid] = NULL;

    if (this->curr_pid == pid) {
        tlb_flush(&(this->tlb));
    }
}

void mmu_delete(mmu *this) {
    for (size_t pid = 0; pid < MAX_PROCESS_ID; pid++) {
        mmu_remove_process(this, pid);
    }

    tlb_delete(this->tlb);
    free(this);
    remove_swap_files();
}
