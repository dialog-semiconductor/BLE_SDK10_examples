







        MEMORY
        {
                ROM (rx) : ORIGIN = (0x0), LENGTH = ((370 * 1024))
                RAM (rw) : ORIGIN = (0x20000000 + 0x200), LENGTH = ((512 * 1024) - 0x200)
        }
