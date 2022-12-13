main()
{
    enableInterrupts();
    syscall(0, "Attempting to enable interrupts...\r\n", 0, 0);
    syscall(0,"tstpr2 is working!\r\n", 0, 0);
    syscall(0, "Now terminating...\r\n", 0, 0);
    syscall(5);
    while(1);
}
