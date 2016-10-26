void getStack(uint32_t starter, uint32_t ender){
  char stackline[46];
  for (uint32_t pos = starter; pos < ender; pos += 0x10) {
      uint32_t* values = (uint32_t*)(pos);
      bool looksLikeStackFrame = (values[2] == pos + 0x10);
      sprintf(stackline, "%08x:  %08x %08x %08x %08x %c", pos, values[0], values[1], values[2], values[3], (looksLikeStackFrame)?'<':' ');
      sprintf(buf2 + strlen(buf2), "%s", stackline);
  } 
}

extern "C" void custom_crash_callback(struct rst_info * rst_info, uint32_t stack, uint32_t stack_end ){  
  register uint32_t sp asm("a1");
  cont_t g_cont __attribute__ ((aligned (16)));
  uint32_t cont_stack_start = (uint32_t) &(g_cont.stack);
  uint32_t cont_stack_end = (uint32_t) g_cont.stack_end;
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
      sprintf(buf2 + strlen(buf2), "%s", "ctx: cont ");
  }
  else {
      sprintf(buf2 + strlen(buf2), "%s", "ctx: sys ");
  }
  sprintf(buf2 + strlen(buf2), "sp: %08x end: %08x offset: %04x\n", stack, stack_end, offset);
  getStack(stack, stack_end);

  //Stacktrace alleen overschrijven als de vorige wel opgehaald is
  if (error_sent == 1){
    eeprom_erase_all();
    eeprom_write_string(0, buf2);
    EEPROM.commit();
  }
} 

#ifdef DEBUG2
void crashme(){
  int* i = NULL;
  *i = 80;
}
#endif

#ifdef DEBUG2
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
#endif
