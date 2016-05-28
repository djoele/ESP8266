void getStack(uint32_t starter, uint32_t ender){
  //char stack_self[2000];
  char stackline[46];
  
  for (uint32_t pos = starter; pos < ender; pos += 0x10) {
      uint32_t* values = (uint32_t*)(pos);
      //rough indicator: stack frames usually have SP saved as the second word
      bool looksLikeStackFrame = (values[2] == pos + 0x10);
      sprintf(stackline, "%08x:  %08x %08x %08x %08x %c", pos, values[0], values[1], values[2], values[3], (looksLikeStackFrame)?'<':' ');
      sprintf(buf2 + strlen(buf2), "%s", stackline);
  } 
  //strcpy(buf2, stack_self);
}

extern "C" void custom_crash_callback(struct rst_info * rst_info, uint32_t stack, uint32_t stack_end ){  
  register uint32_t sp asm("a1");
  cont_t g_cont __attribute__ ((aligned (16)));
  char result[2000];
  char cont[14];
  char nctx[14];
  char spi[50];
  
  uint32_t cont_stack_start = (uint32_t) &(g_cont.stack);
  uint32_t cont_stack_end = (uint32_t) g_cont.stack_end;
  uint32_t stack_end2 = stack_end;
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
  if (stack > cont_stack_start && stack < cont_stack_end) {
      sprintf(nctx, "ctx: cont");
  }
  else {
      sprintf(nctx, "ctx: sys");
  }
  sprintf(spi, "sp: %08x end: %08x offset: %04x\n", stack, stack_end, offset);
  getStack(stack, stack_end);
  sprintf(result, "%s %s %s %s %s", cont, nctx, spi, buf2);
  strcpy(buf,result);

  eeprom_erase_all();
  eeprom_write_string(0, buf);
  EEPROM.commit();
} 

void crashme(){
  int* i = NULL;
  *i = 80;
}

void crashme2(){
  char *svptr = NULL;
  static char* str_input = NULL;
  const char delim[] = " ";
  char input[] = "bla";
  size_t malloc_amount = (sizeof(char) * 0) & (~3);
  str_input = (char *)malloc(malloc_amount);
  memset(str_input, '\0', 0);
  strcpy(str_input, input);  
}
