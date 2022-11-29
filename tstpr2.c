main()
{
    syscall(0,"tstpr2 is working!\r\n");
    syscall(0, "Now terminating...\r\n", 0, 0);
    syscall(5);
    while(1);
}