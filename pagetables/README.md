# Input files for virt2phys

pagetable-24a.txt : 24-bit (16MB), 4kB pages, all VPNs=PPNs (so virt_addr==phys_addr)
pagetable-24b.txt : 24-bit (16MB), 4kB pages, random PPNs, no invalid
pagetable-24c.txt : 24-bit (16MB), 4kB pages, random PPNs, some invalid
pagetable-24d.txt : 24-bit (16MB), 64B pages, random PPNs, some invalid
pagetable-8a.txt  : 8-bit (256B), 16B pages, all VPNs=PPNs (so virt_addr==phys_addr)
pagetable-8b.txt  : 8-bit (256B), 16B pages, random PPNs, no invalid
pagetable-8c.txt  : 8-bit (256B), 16B pages, random PPNs, some invalid
example_page_table.txt : 10-bit (1kB), 256B pages, simple example from writeup
