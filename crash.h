String getStack(uint32_t start, uint32_t end){
  char stack_self[500] = "Stack trace: ";
  char stack_self2[50];
  for (uint32_t pos = start; pos < end; pos += 0x10) {
      uint32_t* values = (uint32_t*)(pos);

      // rough indicator: stack frames usually have SP saved as the second word
      bool looksLikeStackFrame = (values[2] == pos + 0x10);

      sprintf(stack_self2, "%08x:  %08x %08x %08x %08x %c\n", pos, values[0], values[1], values[2], values[3], (looksLikeStackFrame)?'<':' ');
      strcat(stack_self, stack_self2);
    }
  return stack_self;
}
extern "C" void custom_crash_callback(struct rst_info * rst_info, uint32_t stack, uint32_t stack_end ){
  // write a 0 to all 512 bytes of the EEPROM
  //for (int i = 0; i < 512; i++){
    //EEPROM.write(i, 0);
  //}
   
  register uint32_t sp asm("a1");
  cont_t g_cont __attribute__ ((aligned (16)));
  char result[2000];
  char exception[1350];
  char cont[50];
  char nctx[50];
  char spi[50];
  
  if (rst_info->reason == REASON_EXCEPTION_RST) {
     sprintf(exception,"\nException (%i):\nepc1=0x%08x epc2=0x%08x epc3=0x%08x excvaddr=0x%08x depc=0x%08x\n",
            rst_info->exccause, rst_info->epc1, rst_info->epc2, rst_info->epc3, rst_info->excvaddr, rst_info->depc);
  }
    
  uint32_t cont_stack_start = (uint32_t) &(g_cont.stack);
  uint32_t cont_stack_end = (uint32_t) g_cont.stack_end;
  //uint32_t stack_end;

  uint32_t offset = 0;
  if (rst_info->reason == REASON_SOFT_WDT_RST) {
      offset = 0x1b0;
  }
  else if (rst_info->reason == REASON_EXCEPTION_RST) {
      offset = 0x1a0;
  }
  else if (rst_info->reason == REASON_WDT_RST) {
      offset = 0x10;
  }
  if (sp > cont_stack_start && sp < cont_stack_end) {
      sprintf(nctx, "\nctx: cont \n");
      stack_end = cont_stack_end;
  }
  else {
      sprintf(nctx, "\nctx: sys \n");
      stack_end = 0x3fffffb0;
  }

  sprintf(spi, "sp: %08x end: %08x offset: %04x\n", sp, stack_end, offset);

  //strcat(result, exception);
  strcat(result, cont);
  strcat(result, nctx);
  strcat(result, spi);
  
  //strcpy(buf, getStack(sp + offset, stack_end).c_str());
  //strcat(result, buf);
  strcpy(buf,result);
  eeprom_write_string(0, buf);
  EEPROM.commit();
} 
